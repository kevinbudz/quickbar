/*
    SPDX-FileCopyrightText: 2016 Kai Uwe Broulik <kde@privat.broulik.de>
    SPDX-FileCopyrightText: 2016 Chinmoy Ranjan Pradhan <chinmoyrp65@gmail.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "appmenumodel.h"

#include "genericmenu.h"
#include "menushortcutsender.h"

#include <QDBusConnection>
#include <QDBusServiceWatcher>
#include <QCoreApplication>
#include <QFileInfo>
#include <QFile>
#include <QGuiApplication>
#include <QMenu>
#include <QStandardPaths>
#include <QUrl>

// Includes for the menu search.
#include <KLocalizedString>
#include <QLineEdit>
#include <QListView>
#include <QWidgetAction>
#include <algorithm>
#include <csignal>
#include <unistd.h>

#include <QTimer>

#include <abstracttasksmodel.h>
#include <dbusmenuimporter.h>

class KDBusMenuImporter : public DBusMenuImporter
{
public:
    KDBusMenuImporter(const QString &service, const QString &path)
        : DBusMenuImporter(service, path)
    {
    }

protected:
    QIcon iconForName(const QString &name) override
    {
        return QIcon::fromTheme(name);
    }
};

namespace
{
constexpr auto genericActionProperty = "quickbarGenericAction";
constexpr auto quitApplicationAction = "quitApplication";
constexpr auto minimizeWindowAction = "minimizeWindow";
constexpr auto maximizeWindowAction = "maximizeWindow";
constexpr auto openAboutAction = "openAbout";

QString visibleMenuText(const QString &text)
{
    QString out;
    out.reserve(text.size());
    for (qsizetype i = 0; i < text.size(); ++i) {
        const QChar ch = text.at(i);
        if (ch == QLatin1Char('&')) {
            if (i + 1 < text.size() && text.at(i + 1) == QLatin1Char('&')) {
                out += QLatin1Char('&');
                ++i;
            }
            continue;
        }
        out += ch;
    }
    return out;
}

void collectMenuActions(QMenu *menu, QList<QAction *> &result, QSet<QMenu *> &visited)
{
    if (!menu || visited.contains(menu)) {
        return;
    }
    visited.insert(menu);

    for (QAction *action : menu->actions()) {
        if (!action || action->isSeparator()) {
            continue;
        }
        if (QMenu *subMenu = action->menu()) {
            collectMenuActions(subMenu, result, visited);
        } else if (!action->text().isEmpty()) {
            result.append(action);
        }
    }
}
} // namespace

AppMenuModel::AppMenuModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_tasksModel(new TaskManager::TasksModel(this))
    , m_shortcutBridge(new MenuShortcutBridge(m_tasksModel, this))
    , m_serviceWatcher(new QDBusServiceWatcher(this))
{
    m_tasksModel->setFilterByScreen(!m_allScreens);
    connect(m_tasksModel, &TaskManager::TasksModel::activeTaskChanged, this, &AppMenuModel::onActiveWindowChanged);
    connect(m_tasksModel,
            &TaskManager::TasksModel::dataChanged,
            [this](const QModelIndex &topLeft, const QModelIndex &bottomRight, const QList<int> &roles = QList<int>()) {
                Q_UNUSED(topLeft)
                Q_UNUSED(bottomRight)
                if (roles.contains(TaskManager::AbstractTasksModel::ApplicationMenuObjectPath)
                    || roles.contains(TaskManager::AbstractTasksModel::ApplicationMenuServiceName)
                    || roles.contains(TaskManager::AbstractTasksModel::AppId)
                    || roles.contains(TaskManager::AbstractTasksModel::AppName)
                    || roles.contains(Qt::DecorationRole)
                    || roles.isEmpty()) {
                    onActiveWindowChanged();
                }
            });
    connect(m_tasksModel, &TaskManager::TasksModel::activityChanged, this, &AppMenuModel::onActiveWindowChanged);
    connect(m_tasksModel, &TaskManager::TasksModel::virtualDesktopChanged, this, &AppMenuModel::onActiveWindowChanged);
    connect(m_tasksModel, &TaskManager::TasksModel::countChanged, this, &AppMenuModel::onActiveWindowChanged);
    connect(m_tasksModel, &TaskManager::TasksModel::screenGeometryChanged, this, &AppMenuModel::screenGeometryChanged);

    connect(this, &AppMenuModel::modelNeedsUpdate, this, [this] {
        if (!m_updatePending) {
            m_updatePending = true;
            QMetaObject::invokeMethod(this, "update", Qt::QueuedConnection);
        }
    });

    onActiveWindowChanged();

    m_serviceWatcher->setConnection(QDBusConnection::sessionBus());
    // if our current DBus connection gets lost, close the menu
    // we'll select the new menu when the focus changes
    connect(m_serviceWatcher, &QDBusServiceWatcher::serviceUnregistered, this, [this](const QString &serviceName) {
        if (serviceName == m_serviceName) {
            setMenuAvailable(false);
            Q_EMIT modelNeedsUpdate();
        }
    });

    m_searchAction = new QAction(this);
    m_searchAction->setText(i18n("Search"));
    m_searchAction->setObjectName(QStringLiteral("appmenu"));

    m_searchMenu.reset(new QMenu);
    auto searchAction = new QWidgetAction(this);
    auto searchBar = new QLineEdit;
    searchBar->setClearButtonEnabled(true);
    searchBar->setPlaceholderText(i18n("Search…"));
    searchBar->setMinimumWidth(200);
    searchBar->setContentsMargins(4, 4, 4, 4);
    connect(m_tasksModel, &TaskManager::TasksModel::activeTaskChanged, searchBar, [searchBar]() {
        searchBar->setText(QString());
    });
    connect(searchBar, &QLineEdit::textChanged, this, [searchBar, this]() mutable {
        insertSearchActionsIntoMenu(searchBar->text());
    });
    connect(searchBar, &QLineEdit::returnPressed, this, [this]() mutable {
        if (!m_currentSearchActions.empty()) {
            m_currentSearchActions.constFirst()->trigger();
        }
    });
    connect(this, &AppMenuModel::modelNeedsUpdate, searchBar, [this, searchBar]() mutable {
        insertSearchActionsIntoMenu(searchBar->text());
    });
    connect(m_searchMenu.get(), &QMenu::aboutToShow, searchBar, [searchBar]() {
        searchBar->setFocus();
    });
    searchAction->setDefaultWidget(searchBar);
    m_searchMenu->addAction(searchAction);
    m_searchMenu->addSeparator();
    m_searchAction->setMenu(m_searchMenu.get());
}

AppMenuModel::~AppMenuModel()
{
    removeSearchActionsFromMenu();
}

bool AppMenuModel::menuAvailable() const
{
    return m_menuAvailable;
}

void AppMenuModel::setMenuAvailable(bool set)
{
    if (m_menuAvailable != set) {
        m_menuAvailable = set;
        if (set) {
            setVisible(true);
        } else {
            setVisible(false);
        }
        Q_EMIT menuAvailableChanged();
        Q_EMIT menuForDisplayChanged();
    }
}

bool AppMenuModel::allScreens() const
{
    return m_allScreens;
}

void AppMenuModel::setallScreens(bool allScreens)
{
    if (m_allScreens == allScreens) {
        return;
    }

    m_allScreens = allScreens;
    m_tasksModel->setFilterByScreen(!m_allScreens);
    Q_EMIT allScreensChanged();
}

bool AppMenuModel::visible() const
{
    return m_visible;
}

bool AppMenuModel::menuForDisplay() const
{
    return m_menuAvailable;
}

bool AppMenuModel::stickyMenuBar() const
{
    return m_stickyMenuBar;
}

void AppMenuModel::setStickyMenuBar(bool sticky)
{
    if (m_stickyMenuBar == sticky) {
        return;
    }

    m_stickyMenuBar = sticky;
    Q_EMIT stickyMenuBarChanged();

    if (!sticky) {
        onActiveWindowChanged();
    } else {
        Q_EMIT menuForDisplayChanged();
    }
}

bool AppMenuModel::showDesktopMenu() const
{
    return m_showDesktopMenu;
}

void AppMenuModel::setShowDesktopMenu(bool show)
{
    if (m_showDesktopMenu == show) {
        return;
    }

    m_showDesktopMenu = show;
    Q_EMIT showDesktopMenuChanged();
    onActiveWindowChanged();
}

bool AppMenuModel::enableGenericMenu() const
{
    return m_enableGenericMenu;
}

void AppMenuModel::setEnableGenericMenu(bool enable)
{
    if (m_enableGenericMenu == enable) {
        return;
    }

    m_enableGenericMenu = enable;
    Q_EMIT enableGenericMenuChanged();
    onActiveWindowChanged();
}

bool AppMenuModel::enableMenuSearch() const
{
    return m_enableMenuSearch;
}

void AppMenuModel::setEnableMenuSearch(bool enable)
{
    if (m_enableMenuSearch == enable) {
        return;
    }

    m_enableMenuSearch = enable;
    Q_EMIT enableMenuSearchChanged();
    Q_EMIT modelNeedsUpdate();
}

QString AppMenuModel::applicationName() const
{
    return m_applicationName;
}

void AppMenuModel::setApplicationName(const QString &name)
{
    if (m_applicationName == name) {
        return;
    }

    m_applicationName = name;
    Q_EMIT applicationNameChanged();
}

QVariant AppMenuModel::applicationIcon() const
{
    return m_applicationIcon;
}

void AppMenuModel::setApplicationIcon(const QVariant &icon)
{
    if (m_applicationIcon == icon) {
        return;
    }

    m_applicationIcon = icon;
    Q_EMIT applicationIconChanged();
}

QString AppMenuModel::excludedItemsRegex() const
{
    return m_excludedItemsRegex;
}

void AppMenuModel::setExcludedItemsRegex(const QString &regex)
{
    if (m_excludedItemsRegex == regex) {
        return;
    }

    m_excludedItemsRegex = regex;
    m_compiledExcludedRegex = QRegularExpression(regex, QRegularExpression::CaseInsensitiveOption);
    Q_EMIT excludedItemsRegexChanged();
    Q_EMIT modelNeedsUpdate();
}

void AppMenuModel::setVisible(bool visible)
{
    if (m_visible != visible) {
        m_visible = visible;
        Q_EMIT visibleChanged();
        Q_EMIT menuForDisplayChanged();
    }
}

QRect AppMenuModel::screenGeometry() const
{
    return m_tasksModel->screenGeometry();
}

void AppMenuModel::setScreenGeometry(QRect geometry)
{
    m_tasksModel->setScreenGeometry(geometry);
}

bool AppMenuModel::includeSearchInModel() const
{
    return m_enableMenuSearch && m_searchAction;
}

QList<QAction *> AppMenuModel::visibleActions() const
{
    QList<QAction *> ret;
    if (!m_menuAvailable || !m_menu) {
        return ret;
    }
    const auto actions = m_menu->actions();
    for (QAction *action : actions) {
        if (!action) {
            continue;
        }
        if (!m_excludedItemsRegex.isEmpty() && m_compiledExcludedRegex.isValid()) {
            const QString rawText = action->text();
            const QString strippedText = visibleMenuText(rawText);
            if (m_compiledExcludedRegex.match(rawText).hasMatch() || m_compiledExcludedRegex.match(strippedText).hasMatch()) {
                continue;
            }
        }
        ret.append(action);
    }
    return ret;
}

int AppMenuModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    if (!m_menuAvailable || !m_menu) {
        return 0;
    }

    const auto actions = visibleActions();
    const int actionCount = actions.count();
    // Avoid showing only "Search" while the DBus menu is still loading or temporarily empty.
    if (actionCount == 0) {
        return 0;
    }

    return actionCount + (includeSearchInModel() ? 1 : 0);
}

void AppMenuModel::removeSearchActionsFromMenu()
{
    if (!m_searchAction || !m_searchAction->menu()) {
        m_currentSearchActions.clear();
        return;
    }
    for (auto action : std::as_const(m_currentSearchActions)) {
        if (action) {
            m_searchAction->menu()->removeAction(action);
        }
    }
    m_currentSearchActions.clear();
}

void AppMenuModel::insertSearchActionsIntoMenu(const QString &filter)
{
    removeSearchActionsFromMenu();
    if (filter.isEmpty() || !m_searchAction || !m_searchAction->menu()) {
        return;
    }
    const auto actions = flatActionList();
    for (const auto &action : actions) {
        if (action->text().contains(filter, Qt::CaseInsensitive)
            || visibleMenuText(action->text()).contains(filter, Qt::CaseInsensitive)) {
            m_searchAction->menu()->addAction(action);
            m_currentSearchActions << action;
        }
    }
}

void AppMenuModel::update()
{
    beginResetModel();
    endResetModel();
    m_updatePending = false;
}

void AppMenuModel::onActiveWindowChanged()
{
    // Do not change active window when panel gets focus
    // See ShellCorona::init() in shell/shellcorona.cpp
    if (m_containmentStatus == Plasma::Types::AcceptingInputStatus) {
        return;
    }

    auto updateAppNameAndIcon = [this](const QModelIndex &index) {
        if (index.isValid()) {
            setApplicationName(m_tasksModel->data(index, TaskManager::AbstractTasksModel::AppName).toString());
            QVariant iconVar = m_tasksModel->data(index, Qt::DecorationRole);
            if (!iconVar.isValid() || iconVar.isNull()) {
                iconVar = m_tasksModel->data(index, TaskManager::AbstractTasksModel::AppId);
            }
            setApplicationIcon(iconVar);
        } else if (m_showDesktopMenu) {
            setApplicationName(i18n("Plasma"));
            setApplicationIcon(QStringLiteral("plasma"));
        } else {
            setApplicationName(QString());
            setApplicationIcon(QVariant());
        }
    };

    const QModelIndex activeTaskIndex = m_tasksModel->activeTask();
    updateAppNameAndIcon(activeTaskIndex);

    if (activeTaskIndex.isValid()) {
        const QString objectPath = m_tasksModel->data(activeTaskIndex, TaskManager::AbstractTasksModel::ApplicationMenuObjectPath).toString();
        const QString serviceName = m_tasksModel->data(activeTaskIndex, TaskManager::AbstractTasksModel::ApplicationMenuServiceName).toString();

        if (!objectPath.isEmpty() && !serviceName.isEmpty()) {
            m_usingGenericMenu = false;
            updateApplicationMenu(serviceName, objectPath);
            return;
        }

        if (shouldUseGenericMenu(activeTaskIndex)) {
            applyGenericMenu();
            return;
        }
    } else if (m_showDesktopMenu && shouldUseGenericMenu(activeTaskIndex)) {
        applyGenericMenu();
        return;
    }

    clearApplicationMenu();
}

bool AppMenuModel::shouldUseGenericMenu(const QModelIndex &activeTaskIndex) const
{
    if (!m_enableGenericMenu) {
        return false;
    }
    if (!activeTaskIndex.isValid()) {
        return m_showDesktopMenu;
    }
    return true;
}

void AppMenuModel::clearApplicationMenu()
{
    m_usingGenericMenu = false;
    setMenuAvailable(false);
    setVisible(false);

    m_serviceName.clear();
    m_menuObjectPath.clear();
    m_serviceWatcher->setWatchedServices({});
    m_importer.reset();
    m_menu.clear();
    Q_EMIT modelNeedsUpdate();
}

void AppMenuModel::quitActiveTask()
{
    if (!m_tasksModel) {
        return;
    }

    const QModelIndex activeTaskIndex = m_tasksModel->activeTask();
    if (!activeTaskIndex.isValid()
        || !m_tasksModel->data(activeTaskIndex, TaskManager::AbstractTasksModel::IsClosable).toBool()) {
        return;
    }

    m_tasksModel->requestClose(activeTaskIndex);
}

void AppMenuModel::minimizeActiveTask()
{
    if (!m_tasksModel) {
        return;
    }

    const QModelIndex activeTaskIndex = m_tasksModel->activeTask();
    if (!activeTaskIndex.isValid()
        || !m_tasksModel->data(activeTaskIndex, TaskManager::AbstractTasksModel::IsMinimizable).toBool()
        || m_tasksModel->data(activeTaskIndex, TaskManager::AbstractTasksModel::IsMinimized).toBool()) {
        return;
    }

    m_tasksModel->requestToggleMinimized(activeTaskIndex);
}

void AppMenuModel::maximizeActiveTask()
{
    if (!m_tasksModel) {
        return;
    }

    const QModelIndex activeTaskIndex = m_tasksModel->activeTask();
    if (!activeTaskIndex.isValid()
        || !m_tasksModel->data(activeTaskIndex, TaskManager::AbstractTasksModel::IsMaximizable).toBool()) {
        return;
    }

    m_tasksModel->requestToggleMaximized(activeTaskIndex);
}

void AppMenuModel::wireGenericMenuActions(QMenu *menu)
{
    if (!menu) {
        return;
    }

    const auto actions = menu->findChildren<QAction *>();
    for (QAction *action : actions) {
        if (action->property(genericActionProperty).toString() == QLatin1String(quitApplicationAction)) {
            connect(action, &QAction::triggered, this, &AppMenuModel::quitActiveTask);
        } else if (action->property(genericActionProperty).toString() == QLatin1String(minimizeWindowAction)) {
            connect(action, &QAction::triggered, this, &AppMenuModel::minimizeActiveTask);
        } else if (action->property(genericActionProperty).toString() == QLatin1String(maximizeWindowAction)) {
            connect(action, &QAction::triggered, this, &AppMenuModel::maximizeActiveTask);
        } else if (action->property(genericActionProperty).toString() == QLatin1String(openAboutAction)) {
            connect(action, &QAction::triggered, this, &AppMenuModel::requestOpenAbout);
        }
    }
}

void AppMenuModel::updateGenericMenuActionState()
{
    if (!m_genericMenu || !m_tasksModel) {
        return;
    }

    const QModelIndex activeTaskIndex = m_tasksModel->activeTask();
    const bool canClose = activeTaskIndex.isValid()
        && m_tasksModel->data(activeTaskIndex, TaskManager::AbstractTasksModel::IsClosable).toBool();
    const bool canMinimize = activeTaskIndex.isValid()
        && m_tasksModel->data(activeTaskIndex, TaskManager::AbstractTasksModel::IsMinimizable).toBool()
        && !m_tasksModel->data(activeTaskIndex, TaskManager::AbstractTasksModel::IsMinimized).toBool();
    const bool canMaximize = activeTaskIndex.isValid()
        && m_tasksModel->data(activeTaskIndex, TaskManager::AbstractTasksModel::IsMaximizable).toBool();

    const auto actions = m_genericMenu->findChildren<QAction *>();
    for (QAction *action : actions) {
        if (action->property(genericActionProperty).toString() == QLatin1String(quitApplicationAction)) {
            action->setEnabled(canClose);
        } else if (action->property(genericActionProperty).toString() == QLatin1String(minimizeWindowAction)) {
            action->setEnabled(canMinimize);
        } else if (action->property(genericActionProperty).toString() == QLatin1String(maximizeWindowAction)) {
            action->setEnabled(canMaximize);
        }
    }
}

bool AppMenuModel::genericMenuHasDuplicateActions(const QMenu *menu) const
{
    if (!menu) {
        return false;
    }

    int aboutCount = 0;
    const auto actions = menu->findChildren<QAction *>();
    for (const QAction *action : actions) {
        if (action->property(genericActionProperty).toString() == QLatin1String(openAboutAction)) {
            if (++aboutCount > 1) {
                return true;
            }
        }
    }
    return false;
}

void AppMenuModel::applyGenericMenu()
{
    if (m_genericMenu && genericMenuHasDuplicateActions(m_genericMenu.get())) {
        // Avoid deleting the generic menu while a proxy clone might be visible.
        // The proxy's clones are connected to the source actions; deleting the source
        // while the proxy is popup-visible would disconnect the clones but is still
        // racy if the menu is being torn down during QMenu::popup's window creation.
        // Defer the reset if we're currently displaying the generic menu.
        if (m_usingGenericMenu && m_menu == m_genericMenu.get() && m_menuAvailable) {
            // Schedule a safe reset for the next event loop and reuse current menu for now.
            QTimer::singleShot(0, this, [this]() {
                if (m_genericMenu && genericMenuHasDuplicateActions(m_genericMenu.get())) {
                    m_genericMenu.reset();
                    if (m_usingGenericMenu) {
                        QMetaObject::invokeMethod(this, [this]() { applyGenericMenu(); }, Qt::QueuedConnection);
                    }
                }
            });
        } else {
            m_genericMenu.reset();
        }
    }

    if (!m_genericMenu) {
        m_genericMenu.reset(GenericMenu::create());

        m_shortcutBridge->wireMenu(m_genericMenu.get());
        wireGenericMenuActions(m_genericMenu.get());

        const auto actions = m_genericMenu->actions();
        for (QAction *action : actions) {
            connect(action, &QAction::changed, this, [this, action] {
                if (!m_menuAvailable || !m_menu) {
                    return;
                }
                const int actionIdx = m_menu->actions().indexOf(action);
                if (actionIdx > -1) {
                    const QModelIndex modelIdx = index(actionIdx, 0);
                    Q_EMIT dataChanged(modelIdx, modelIdx);
                }
            });
            connect(action, &QAction::destroyed, this, &AppMenuModel::modelNeedsUpdate, Qt::UniqueConnection);
        }
    }

    updateGenericMenuActionState();

    if (m_usingGenericMenu && m_menu == m_genericMenu.get() && m_menuAvailable) {
        setVisible(true);
        return;
    }

    m_usingGenericMenu = true;
    m_serviceName.clear();
    m_menuObjectPath.clear();
    m_serviceWatcher->setWatchedServices({});
    m_importer.reset();

    m_menu = m_genericMenu.get();

    setMenuAvailable(true);
    setVisible(true);
    Q_EMIT modelNeedsUpdate();
}

QHash<int, QByteArray> AppMenuModel::roleNames() const
{
    QHash<int, QByteArray> roleNames;
    roleNames[MenuRole] = QByteArrayLiteral("activeMenu");
    roleNames[ActionRole] = QByteArrayLiteral("activeActions");
    return roleNames;
}

QList<QAction *> AppMenuModel::flatActionList()
{
    QList<QAction *> ret;
    if (!m_menuAvailable || !m_menu) {
        return ret;
    }
    QSet<QMenu *> visited;
    collectMenuActions(m_menu.data(), ret, visited);
    return ret;
}

QVariant AppMenuModel::data(const QModelIndex &index, int role) const
{
    if (!m_menuAvailable || !m_menu) {
        return {};
    }

    if (!index.isValid()) {
        if (role == MenuRole) {
            return QString();
        } else if (role == ActionRole) {
            return QVariant::fromValue(m_menu->menuAction());
        }
    }

    const auto actions = visibleActions();
    if (actions.isEmpty()) {
        return {};
    }

    const int row = index.row();
    if (row == actions.count() && includeSearchInModel()) {
        if (role == MenuRole) {
            return m_searchAction->text();
        } else if (role == ActionRole) {
            return QVariant::fromValue(m_searchAction.data());
        }
    }
    if (row >= actions.count()) {
        return {};
    }

    if (role == MenuRole) { // TODO this should be Qt::DisplayRole
        return actions.at(row)->text();
    } else if (role == ActionRole) {
        return QVariant::fromValue(actions.at(row));
    }

    return {};
}

void AppMenuModel::updateApplicationMenu(const QString &serviceName, const QString &menuObjectPath)
{
    if (m_serviceName == serviceName && m_menuObjectPath == menuObjectPath) {
        return;
    }

    if (serviceName.isEmpty() || menuObjectPath.isEmpty()) {
        if (shouldUseGenericMenu(m_tasksModel->activeTask())) {
            applyGenericMenu();
            return;
        }

        if (!m_menuAvailable) {
            return;
        }

        clearApplicationMenu();
    } else {
        m_usingGenericMenu = false;
        m_serviceName = serviceName;
        m_menuObjectPath = menuObjectPath;
        m_serviceWatcher->setWatchedServices(QStringList({m_serviceName}));

        m_importer = std::make_unique<KDBusMenuImporter>(serviceName, menuObjectPath);
        m_menu = m_importer->menu();
        QMetaObject::invokeMethod(m_importer.get(), "updateMenu", Qt::QueuedConnection);

        connect(m_importer.get(), &DBusMenuImporter::menuUpdated, this, [this](QMenu *menu) {
            m_menu = m_importer->menu();
            if (m_menu.isNull() || menu != m_menu) {
                return;
            }

            if (m_menu->isEmpty()) {
                // naughty apps may pretend to have a menu but then don't actually give us one.
                if (shouldUseGenericMenu(m_tasksModel->activeTask())) {
                    applyGenericMenu();
                    return;
                }
                if (m_menuAvailable) {
                    Q_EMIT modelNeedsUpdate();
                    return;
                }
                setMenuAvailable(false);
                return;
            } else {
                // if it somehow comes later, make it available again
                setMenuAvailable(true);
            }

            // cache first layer of sub menus, which we'll be popping up
            const auto actions = m_menu->actions();
            for (QAction *a : actions) {
                // signal dataChanged when the action changes
                connect(a, &QAction::changed, this, [this, a] {
                    if (m_menuAvailable && m_menu) {
                        const int actionIdx = m_menu->actions().indexOf(a);
                        if (actionIdx > -1) {
                            const QModelIndex modelIdx = index(actionIdx, 0);
                            Q_EMIT dataChanged(modelIdx, modelIdx);
                        }
                    }
                });

                connect(a, &QAction::destroyed, this, &AppMenuModel::modelNeedsUpdate);

                if (a->menu()) {
                    m_importer->updateMenu(a->menu());
                }
            }

            setMenuAvailable(true);
            Q_EMIT modelNeedsUpdate();
        });

        connect(m_importer.get(), &DBusMenuImporter::actionActivationRequested, this, [this](QAction *action) {
            if (!m_menuAvailable || !m_menu || !action) {
                return;
            }

            const auto actions = m_menu->actions();
            auto it = std::ranges::find(actions, action);
            if (it != actions.end()) {
                Q_EMIT requestActivateIndex(it - actions.begin());
                return;
            }

            // If action is inside a submenu, find which top-level menu contains it
            for (int i = 0; i < actions.count(); ++i) {
                if (QMenu *sub = actions.at(i)->menu()) {
                    if (sub->findChildren<QAction *>().contains(action)) {
                        Q_EMIT requestActivateIndex(i);
                        return;
                    }
                }
            }
        });

        // Mark available while the layout loads; rowCount stays 0 until top-level actions exist.
        setMenuAvailable(true);
        setVisible(true);
        Q_EMIT modelNeedsUpdate();
    }
}

#include "moc_appmenumodel.cpp"
