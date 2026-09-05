/*
    SPDX-FileCopyrightText: 2016 Chinmoy Ranjan Pradhan <chinmoyrp65@gmail.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include <KWindowSystem>
#include <QAbstractListModel>
#include <QAction>
#include <QPointer>
#include <QRect>
#include <QStringList>
#include <qqmlregistration.h>
#include <tasksmodel.h>

#include <Plasma/Containment>

class QMenu;
class QModelIndex;
class QDBusServiceWatcher;
class KDBusMenuImporter;
class MenuShortcutBridge;

class AppMenuModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(bool menuAvailable READ menuAvailable WRITE setMenuAvailable NOTIFY menuAvailableChanged)
    Q_PROPERTY(bool visible READ visible NOTIFY visibleChanged)
    Q_PROPERTY(bool menuForDisplay READ menuForDisplay NOTIFY menuForDisplayChanged)
    Q_PROPERTY(bool stickyMenuBar READ stickyMenuBar WRITE setStickyMenuBar NOTIFY stickyMenuBarChanged)
    Q_PROPERTY(bool showDesktopMenu READ showDesktopMenu WRITE setShowDesktopMenu NOTIFY showDesktopMenuChanged)
    Q_PROPERTY(bool enableGenericMenu READ enableGenericMenu WRITE setEnableGenericMenu NOTIFY enableGenericMenuChanged)
    Q_PROPERTY(bool enableMenuSearch READ enableMenuSearch WRITE setEnableMenuSearch NOTIFY enableMenuSearchChanged)
    Q_PROPERTY(QString applicationName READ applicationName NOTIFY applicationNameChanged)
    Q_PROPERTY(QVariant applicationIcon READ applicationIcon NOTIFY applicationIconChanged)
    Q_PROPERTY(QString excludedItemsRegex READ excludedItemsRegex WRITE setExcludedItemsRegex NOTIFY excludedItemsRegexChanged)
    Q_PROPERTY(bool allScreens READ allScreens WRITE setallScreens NOTIFY allScreensChanged)

    Q_PROPERTY(Plasma::Types::ItemStatus containmentStatus MEMBER m_containmentStatus NOTIFY containmentStatusChanged)
    Q_PROPERTY(QRect screenGeometry READ screenGeometry WRITE setScreenGeometry NOTIFY screenGeometryChanged)

public:
    explicit AppMenuModel(QObject *parent = nullptr);
    ~AppMenuModel() override;

    enum AppMenuRole {
        MenuRole = Qt::UserRole + 1, // TODO this should be Qt::DisplayRole
        ActionRole,
    };

    QVariant data(const QModelIndex &index, int role) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QHash<int, QByteArray> roleNames() const override;

    void updateApplicationMenu(const QString &serviceName, const QString &menuObjectPath);

    bool menuAvailable() const;
    void setMenuAvailable(bool set);

    bool allScreens() const;
    void setallScreens(bool allScreens);

    bool visible() const;
    bool menuForDisplay() const;

    bool stickyMenuBar() const;
    void setStickyMenuBar(bool sticky);

    bool showDesktopMenu() const;
    void setShowDesktopMenu(bool show);

    bool enableGenericMenu() const;
    void setEnableGenericMenu(bool enable);

    bool enableMenuSearch() const;
    void setEnableMenuSearch(bool enable);

    QString applicationName() const;
    QVariant applicationIcon() const;

    QString excludedItemsRegex() const;
    void setExcludedItemsRegex(const QString &regex);

    QRect screenGeometry() const;
    void setScreenGeometry(QRect geometry);
    QList<QAction *> flatActionList();
    QList<QAction *> visibleActions() const;

Q_SIGNALS:
    void requestActivateIndex(int index);
    void bringToFocus(int index);
    void requestOpenAbout();
    void menuUpdated(QMenu *menu);

private Q_SLOTS:
    void onActiveWindowChanged();
    void setVisible(bool visible);
    void update();

Q_SIGNALS:
    void allScreensChanged();
    void menuAvailableChanged();
    void menuForDisplayChanged();
    void stickyMenuBarChanged();
    void showDesktopMenuChanged();
    void enableGenericMenuChanged();
    void enableMenuSearchChanged();
    void applicationNameChanged();
    void applicationIconChanged();
    void excludedItemsRegexChanged();
    void modelNeedsUpdate();
    void containmentStatusChanged();
    void screenGeometryChanged();
    void visibleChanged();

private:
    bool m_menuAvailable;
    bool m_allScreens = true;
    bool m_updatePending = false;
    bool m_visible = true;
    bool m_stickyMenuBar = false;
    bool m_showDesktopMenu = true;
    bool m_enableGenericMenu = true;
    bool m_enableMenuSearch = true;
    bool m_usingGenericMenu = false;
    QString m_applicationName;
    QVariant m_applicationIcon;
    QString m_excludedItemsRegex;
    QRegularExpression m_compiledExcludedRegex;

    Plasma::Types::ItemStatus m_containmentStatus = Plasma::Types::PassiveStatus;

    void setApplicationName(const QString &name);
    void setApplicationIcon(const QVariant &icon);
    void applyGenericMenu();
    void clearApplicationMenu();
    bool shouldUseGenericMenu(const QModelIndex &activeTaskIndex) const;
    void quitActiveTask();
    void minimizeActiveTask();
    void maximizeActiveTask();
    void wireGenericMenuActions(QMenu *menu);
    void updateGenericMenuActionState();
    bool genericMenuHasDuplicateActions(const QMenu *menu) const;
    TaskManager::TasksModel *m_tasksModel;

    std::unique_ptr<QMenu> m_searchMenu;
    std::unique_ptr<QMenu> m_genericMenu;
    MenuShortcutBridge *m_shortcutBridge = nullptr;
    QPointer<QMenu> m_menu;
    QPointer<QAction> m_searchAction;
    QList<QAction *> m_currentSearchActions;

    void removeSearchActionsFromMenu();
    void insertSearchActionsIntoMenu(const QString &filter = QString());
    bool includeSearchInModel() const;

    QDBusServiceWatcher *m_serviceWatcher;
    QString m_serviceName;
    QString m_menuObjectPath;

    std::unique_ptr<KDBusMenuImporter> m_importer;
};
