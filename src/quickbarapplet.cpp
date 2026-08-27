/*
    SPDX-FileCopyrightText: 2016 Kai Uwe Broulik <kde@privat.broulik.de>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "quickbarapplet.h"
#include "appmenumodel.h"

#include <QAction>
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDebug>
#include <QFontMetrics>
#include <QHoverEvent>
#include <QKeyEvent>
#include <QMenu>
#include <QMouseEvent>
#include <QWidgetAction>
#include <QQuickItem>
#include <QQuickWindow>
#include <QScreen>
#include <QStyleOptionMenuItem>
#include <QTimer>
#include <QWindow>

int QuickBarApplet::s_refs = 0;
namespace
{
QString viewService()
{
    return QStringLiteral("org.kde.kappmenuview");
}

void setTransientParentIfPossible(QMenu *menu, QWindow *parentWindow)
{
    if (!menu || !parentWindow) {
        return;
    }
    if (QWindow *window = menu->windowHandle()) {
        window->setTransientParent(parentWindow);
    }
}

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

void ensureMenuSizing(QMenu *menu)
{
    if (!menu) {
        return;
    }

    for (QAction *action : menu->actions()) {
        if (QMenu *subMenu = action->menu()) {
            ensureMenuSizing(subMenu);
        }
    }

    QStyle *style = menu->style();
    int minWidth = 0;
    for (QAction *action : menu->actions()) {
        if (action->isSeparator()) {
            continue;
        }

        QStyleOptionMenuItem option;
        option.initFrom(menu);
        option.font = menu->font();
        option.fontMetrics = QFontMetrics(option.font);
        option.text = visibleMenuText(action->text());
        option.icon = action->icon();
        option.menuItemType = action->menu() ? QStyleOptionMenuItem::SubMenu : QStyleOptionMenuItem::Normal;
        option.state = action->isEnabled() ? QStyle::State_Enabled : QStyle::State_None;
        option.rect = menu->rect();

        const QSize itemSize = style->sizeFromContents(QStyle::CT_MenuItem, &option, QSize(), menu);
        minWidth = qMax(minWidth, itemSize.width());
    }

    // Do not accumulate width across reused proxy menu. History (wide) would
    // otherwise make File/Edit narrow menus inherit its width.
    if (minWidth > 0) {
        menu->setMinimumWidth(minWidth);
    } else {
        menu->setMinimumWidth(0);
    }
    menu->adjustSize();
}

bool isSearchMenu(QMenu *menu)
{
    if (!menu) {
        return false;
    }
    // Search menu contains a QWidgetAction with a QLineEdit (set in AppMenuModel)
    for (QAction *a : menu->actions()) {
        if (qobject_cast<QWidgetAction *>(a)) {
            return true;
        }
    }
    return false;
}

void cloneMenuStructure(QMenu *sourceMenu, QMenu *destMenu)
{
    // Reset sizing so a previously wide History menu doesn't make narrow menus wide
    destMenu->setMinimumWidth(0);
    destMenu->clear();

    for (QAction *sourceAction : sourceMenu->actions()) {
        if (sourceAction->isSeparator()) {
            destMenu->addSeparator();
            continue;
        }
        // Skip widget actions (search line edit) — search is handled via direct menu
        if (qobject_cast<QWidgetAction *>(sourceAction)) {
            continue;
        }

        const QString label = visibleMenuText(sourceAction->text());

        if (QMenu *sourceSubMenu = sourceAction->menu()) {
            QMenu *destSubMenu = destMenu->addMenu(label);
            // Preserve enabled state for the submenu action
            destSubMenu->menuAction()->setEnabled(sourceAction->isEnabled());
            cloneMenuStructure(sourceSubMenu, destSubMenu);
            continue;
        }

        QAction *clone = destMenu->addAction(label);
        clone->setEnabled(sourceAction->isEnabled());
        clone->setShortcut(sourceAction->shortcut());
        // Copy icon safely if present; keep empty if source icon is null to avoid
        // copying a dangling QIcon private from a DBus importer that may have been re-used.
        if (!sourceAction->icon().isNull()) {
            clone->setIcon(sourceAction->icon());
        }
        clone->setCheckable(sourceAction->isCheckable());
        clone->setChecked(sourceAction->isChecked());
        QObject::connect(clone, &QAction::triggered, sourceAction, &QAction::trigger);
        // For checkable actions, also propagate toggled
        if (sourceAction->isCheckable()) {
            QObject::connect(clone, &QAction::toggled, sourceAction, &QAction::setChecked);
        }
    }

    ensureMenuSizing(destMenu);
}
} // namespace

QuickBarApplet::QuickBarApplet(QObject *parent, const KPluginMetaData &data, const QVariantList &args)
    : Plasma::Applet(parent, data, args)
{
    ++s_refs;
    // if we're the first, register the service
    if (s_refs == 1) {
        QDBusConnection::sessionBus().interface()->registerService(viewService(),
                                                                   QDBusConnectionInterface::QueueService,
                                                                   QDBusConnectionInterface::DontAllowReplacement);
    }
    /*it registers or unregisters the service when the destroyed value of the applet change,
      and not in the dtor, because:
      when we "delete" an applet, it just hides it for about a minute setting its status
      to destroyed, in order to be able to do a clean undo: if we undo, there will be
      another destroyedchanged and destroyed will be false.
      When this happens, if we are the only appmenu applet existing, the dbus interface
      will have to be registered again*/
    connect(this, &Applet::destroyedChanged, this, [](bool destroyed) {
        if (destroyed) {
            // if we were the last, unregister
            if (--s_refs == 0) {
                QDBusConnection::sessionBus().interface()->unregisterService(viewService());
            }
        } else {
            // if we're the first, register the service
            if (++s_refs == 1) {
                QDBusConnection::sessionBus().interface()->registerService(viewService(),
                                                                            QDBusConnectionInterface::QueueService,
                                                                            QDBusConnectionInterface::DontAllowReplacement);
            }
        }
    });
}

QuickBarApplet::~QuickBarApplet() = default;

void QuickBarApplet::init()
{
}

QAbstractItemModel *QuickBarApplet::model() const
{
    return m_model;
}

void QuickBarApplet::setModel(QAbstractItemModel *model)
{
    if (m_model != model) {
        m_model = model;
        Q_EMIT modelChanged();
    }
}

int QuickBarApplet::view() const
{
    return m_viewType;
}

void QuickBarApplet::setView(int type)
{
    if (m_viewType != type) {
        m_viewType = type;
        Q_EMIT viewChanged();
    }
}

int QuickBarApplet::currentIndex() const
{
    return m_currentIndex;
}

void QuickBarApplet::setCurrentIndex(int currentIndex)
{
    if (m_currentIndex != currentIndex) {
        m_currentIndex = currentIndex;
        Q_EMIT currentIndexChanged();
    }
}

bool QuickBarApplet::hoverOpensMenu() const
{
    return m_hoverOpensMenu;
}

void QuickBarApplet::setHoverOpensMenu(bool hover)
{
    if (m_hoverOpensMenu != hover) {
        m_hoverOpensMenu = hover;
        Q_EMIT hoverOpensMenuChanged();
    }
}

QQuickItem *QuickBarApplet::buttonGrid() const
{
    return m_buttonGrid;
}

void QuickBarApplet::setButtonGrid(QQuickItem *buttonGrid)
{
    if (m_buttonGrid != buttonGrid) {
        m_buttonGrid = buttonGrid;
        Q_EMIT buttonGridChanged();
    }
}

QMenu *QuickBarApplet::createMenu(int idx) const
{
    QMenu *menu = nullptr;

    if (!m_model) {
        return nullptr;
    }

    if (view() == CompactView) {
        if (auto *menuAction = m_model->data(QModelIndex(), AppMenuModel::ActionRole).value<QAction *>()) {
            menu = menuAction->menu();
        }
    } else if (view() == FullView) {
        const QModelIndex index = m_model->index(idx, 0);
        if (auto *action = m_model->data(index, AppMenuModel::ActionRole).value<QAction *>()) {
            menu = action->menu();
        }
    }

    return menu;
}

void QuickBarApplet::installMenuEventFilter(QMenu *menu)
{
    if (!menu) {
        return;
    }

    menu->installEventFilter(this);
    connect(menu, &QMenu::aboutToShow, this, [this, menu]() {
        for (QAction *action : menu->actions()) {
            if (QMenu *subMenu = action->menu()) {
                installMenuEventFilter(subMenu);
            }
        }
    }, Qt::UniqueConnection);

    for (QAction *action : menu->actions()) {
        if (QMenu *subMenu = action->menu()) {
            installMenuEventFilter(subMenu);
        }
    }
}

void QuickBarApplet::removeMenuEventFilter(QMenu *menu)
{
    if (!menu) {
        return;
    }

    menu->removeEventFilter(this);
    for (QAction *action : menu->actions()) {
        if (QMenu *subMenu = action->menu()) {
            removeMenuEventFilter(subMenu);
        }
    }
}

void QuickBarApplet::onMenuAboutToHide()
{
    if (m_pendingMenuSwitch) {
        return;
    }

    if (m_currentMenu) {
        removeMenuEventFilter(m_currentMenu.data());
    }
    setCurrentIndex(-1);
}

Qt::Edges edgeFromLocation(Plasma::Types::Location location)
{
    switch (location) {
    case Plasma::Types::TopEdge:
        return Qt::TopEdge;
    case Plasma::Types::BottomEdge:
        return Qt::BottomEdge;
    case Plasma::Types::LeftEdge:
        return Qt::LeftEdge;
    case Plasma::Types::RightEdge:
        return Qt::RightEdge;
    case Plasma::Types::Floating:
    case Plasma::Types::Desktop:
    case Plasma::Types::FullScreen:
        break;
    }
    return {};
}

void QuickBarApplet::trigger(QQuickItem *ctx, int idx)
{
    if (m_currentIndex == idx) {
        return;
    }

    const bool switchingMenu = m_currentIndex >= 0;

    if (!ctx || !ctx->window() || !ctx->window()->screen()) {
        if (switchingMenu) {
            m_pendingMenuSwitch = false;
        }
        return;
    }
    if (switchingMenu) {
        m_pendingMenuSwitch = true;
    }

    QMenu *actionMenu = createMenu(idx);
    if (actionMenu) {
        // Workaround for mouse release issue when spawning windows
        // https://bugreports.qt.io/browse/QTBUG-59044
        auto ungrabMouseHack = [ctx]() {
            if (ctx && ctx->window() && ctx->window()->mouseGrabberItem()) {
                ctx->window()->mouseGrabberItem()->ungrabMouse();
            }
        };

        const auto &geo = ctx->window()->screen()->availableVirtualGeometry();
        QPoint pos = ctx->window()->mapToGlobal(ctx->mapToScene(QPointF()).toPoint());
        const Qt::Edges edges = edgeFromLocation(location());
        if (location() == Plasma::Types::TopEdge) {
            pos.setY(pos.y() + ctx->height());
        }

        // Search menu contains a QLineEdit widget — cloning loses the widget and
        // makes the popup fade immediately. Use the original menu directly for search.
        const bool useDirectSearchMenu = isSearchMenu(actionMenu);
        if (useDirectSearchMenu) {
            if (m_currentMenu && m_currentMenu->isVisible()) {
                removeMenuEventFilter(m_currentMenu.data());
                m_currentMenu->hide();
            }
            if (!actionMenu->property("_quickbarSearchAboutToHideConnected").toBool()) {
                connect(actionMenu, &QMenu::aboutToHide, this, &QuickBarApplet::onMenuAboutToHide, Qt::UniqueConnection);
                actionMenu->setProperty("_quickbarSearchAboutToHideConnected", true);
            }
            m_currentMenu = actionMenu;
            m_sourceMenu = actionMenu;
            QTimer::singleShot(0, ctx, ungrabMouseHack);
            m_currentMenu->setProperty("_breeze_menu_seamless_edges", QVariant::fromValue(edges));
            // Search menu already has correct sizing (line edit 200px); ensure but don't accumulate
            m_currentMenu->setMinimumWidth(0);
            ensureMenuSizing(m_currentMenu.data());
            pos = QPoint(qBound(geo.x(), pos.x(), geo.x() + geo.width() - m_currentMenu->width()),
                         qBound(geo.y(), pos.y(), geo.y() + geo.height() - m_currentMenu->height()));
            installMenuEventFilter(m_currentMenu.data());
            m_currentMenu->winId();
            setTransientParentIfPossible(m_currentMenu, ctx->window());
            m_currentMenu->popup(pos);
            setCurrentIndex(idx);
            if (switchingMenu) {
                QTimer::singleShot(0, this, [this]() { m_pendingMenuSwitch = false; });
            }
            return;
        }

        // Always clone into a proxy menu. Using the original DBus-imported QMenu
        // directly corrupts the app's menu (e.g. Firefox) and can crash the
        // application or plasmashell when the menu's QIcon/shared data is read
        // during QWidget::create / QMenu::popup. The proxy is a detached copy with
        // safe QIcon handling and no parent.
        if (!m_proxyMenu) {
            m_proxyMenu = std::make_unique<QMenu>();
            connect(m_proxyMenu.get(), &QMenu::aboutToHide, this, &QuickBarApplet::onMenuAboutToHide, Qt::UniqueConnection);
        }

        const bool wasVisible = m_currentMenu && m_currentMenu->isVisible();
        // For non-switching trigger that somehow has a visible menu, hide it first
        // (should not happen due to early return for same index, but keep for safety).
        if (wasVisible && !switchingMenu) {
            removeMenuEventFilter(m_currentMenu.data());
            m_currentMenu->hide();
        }

        // For switching, keep the proxy visible and just swap its content.
        // Hiding then popping causes flicker and the fade-away the user sees.
        // KDE's appmenu does the same: move the existing visible menu.
        const bool keepVisibleForSwitch = switchingMenu && wasVisible && m_currentMenu == m_proxyMenu.get();

        cloneMenuStructure(actionMenu, m_proxyMenu.get());
        m_currentMenu = m_proxyMenu.get();
        m_sourceMenu = actionMenu;

        QTimer::singleShot(0, ctx, ungrabMouseHack);

        m_currentMenu->setProperty("_breeze_menu_seamless_edges", QVariant::fromValue(edges));
        // cloneMenuStructure already calls ensureMenuSizing, but ensure again after
        // setting the seamless edges property
        m_currentMenu->adjustSize();

        pos = QPoint(qBound(geo.x(), pos.x(), geo.x() + geo.width() - m_currentMenu->width()),
                     qBound(geo.y(), pos.y(), geo.y() + geo.height() - m_currentMenu->height()));

        if (view() == FullView) {
            // Ensure filter is installed (UniqueConnection prevents duplicates)
            installMenuEventFilter(m_currentMenu.data());
            m_currentMenu->winId();
            setTransientParentIfPossible(m_currentMenu, ctx->window());
            if (keepVisibleForSwitch) {
                // Move the already-visible popup to the new button's position
                m_currentMenu->move(pos);
            } else if (!wasVisible) {
                m_currentMenu->popup(pos);
            } else {
                // Fallback: wasVisible but not keepVisible (e.g. different proxy) -> popup
                m_currentMenu->popup(pos);
            }
        } else if (view() == CompactView) {
            if (m_currentMenu->isEmpty()) {
                if (switchingMenu) {
                    m_pendingMenuSwitch = false;
                }
                return;
            }
            installMenuEventFilter(m_currentMenu.data());
            m_currentMenu->winId();
            setTransientParentIfPossible(m_currentMenu, ctx->window());
            m_currentMenu->popup(pos);
        }

        setCurrentIndex(idx);

        if (switchingMenu) {
            QTimer::singleShot(0, this, [this]() {
                m_pendingMenuSwitch = false;
            });
        }
    } else if (m_model) { // is it just an action without a menu?
        if (switchingMenu) {
            m_pendingMenuSwitch = false;
        }
        if (auto *action = m_model->index(idx, 0).data(AppMenuModel::ActionRole).value<QAction *>()) {
            Q_ASSERT(!action->menu());
            action->trigger();
        }
    } else if (switchingMenu) {
        m_pendingMenuSwitch = false;
    }
}

bool QuickBarApplet::eventFilter(QObject *watched, QEvent *event)
{
    auto *menu = qobject_cast<QMenu *>(watched);
    if (!menu) {
        return false;
    }

    if (event->type() == QEvent::KeyPress) {
        auto *e = static_cast<QKeyEvent *>(event);

        // TODO right to left languages
        if (e->key() == Qt::Key_Left) {
            int desiredIndex = m_currentIndex - 1;
            Q_EMIT requestActivateIndex(desiredIndex);
            return true;
        } else if (e->key() == Qt::Key_Right) {
            if (menu->activeAction() && menu->activeAction()->menu()) {
                return false;
            }

            int desiredIndex = m_currentIndex + 1;
            Q_EMIT requestActivateIndex(desiredIndex);
            return true;
        }

    } else if (event->type() == QEvent::MouseMove) {
        if (!m_buttonGrid || !m_buttonGrid->window()) {
            return false;
        }
        if (!m_hoverOpensMenu) {
            return false;
        }

        auto *e = static_cast<QMouseEvent *>(event);

        const QPointF globalPos = e->globalPosition();
        const QPointF windowLocalPos = m_buttonGrid->window()->mapFromGlobal(globalPos);
        const QPointF buttonGridLocalPos = m_buttonGrid->mapFromScene(windowLocalPos);
        // Only switch when the mouse is actually over the menubar, not over its
        // popup submenu (which is below/overlaps). This fixes "hovering over View
        // submenu opens Edit".
        if (buttonGridLocalPos.x() < 0 || buttonGridLocalPos.y() < 0
            || buttonGridLocalPos.x() > m_buttonGrid->width()
            || buttonGridLocalPos.y() > m_buttonGrid->height()) {
            return false;
        }
        auto *item = m_buttonGrid->childAt(buttonGridLocalPos.x(), buttonGridLocalPos.y());
        int buttonIndex = -1;
        bool ok = false;
        if (item) {
            buttonIndex = item->property("buttonIndex").toInt(&ok);
        }
        // Fallback: iterate all children and test contains(), handles spacing/margins
        // Use mapFromItem with buttonGridLocalPos (already in buttonGrid coords) for accuracy.
        if (!ok || buttonIndex < 0) {
            for (QQuickItem *child : m_buttonGrid->childItems()) {
                if (!child || !child->isVisible()) {
                    continue;
                }
                bool childOk = false;
                int idx = child->property("buttonIndex").toInt(&childOk);
                if (!childOk || idx < 0) {
                    continue;
                }
                // buttonGridLocalPos is in buttonGrid's coords; map it to child's coords
                const QPointF posInChild = child->mapFromItem(m_buttonGrid, buttonGridLocalPos);
                if (child->contains(posInChild)) {
                    buttonIndex = idx;
                    ok = true;
                    break;
                }
            }
        }
        if (!ok || buttonIndex < 0 || buttonIndex == m_currentIndex) {
            return false;
        }

        qDebug() << "QuickBar hover switch:" << m_currentIndex << "->" << buttonIndex << "global" << e->globalPosition() << "hover" << m_hoverOpensMenu << "grid" << m_buttonGrid;
        Q_EMIT requestActivateIndex(buttonIndex);
        return true;
    } else if (event->type() == QEvent::HoverMove) {
        if (!m_buttonGrid || !m_buttonGrid->window() || !m_hoverOpensMenu) {
            return false;
        }
        auto *e = static_cast<QHoverEvent *>(event);
        const QPointF globalPos = e->globalPosition();
        const QPointF windowLocalPos = m_buttonGrid->window()->mapFromGlobal(globalPos);
        const QPointF buttonGridLocalPos = m_buttonGrid->mapFromScene(windowLocalPos);
        if (buttonGridLocalPos.x() < 0 || buttonGridLocalPos.y() < 0
            || buttonGridLocalPos.x() > m_buttonGrid->width()
            || buttonGridLocalPos.y() > m_buttonGrid->height()) {
            return false;
        }
        auto *item = m_buttonGrid->childAt(buttonGridLocalPos.x(), buttonGridLocalPos.y());
        int buttonIndex = -1;
        bool ok = false;
        if (item) {
            buttonIndex = item->property("buttonIndex").toInt(&ok);
        }
        if (!ok || buttonIndex < 0) {
            for (QQuickItem *child : m_buttonGrid->childItems()) {
                if (!child || !child->isVisible()) continue;
                bool childOk = false;
                int idx = child->property("buttonIndex").toInt(&childOk);
                if (!childOk || idx < 0) continue;
                const QPointF posInChild = child->mapFromItem(m_buttonGrid, buttonGridLocalPos);
                if (child->contains(posInChild)) { buttonIndex = idx; ok = true; break; }
            }
        }
        if (!ok || buttonIndex < 0 || buttonIndex == m_currentIndex) return false;
        qDebug() << "QuickBar HoverMove switch:" << m_currentIndex << "->" << buttonIndex << "global" << globalPos;
        Q_EMIT requestActivateIndex(buttonIndex);
        return true;
    }

    return false;
}

K_PLUGIN_CLASS_WITH_JSON(QuickBarApplet, "metadata.json")

#include "quickbarapplet.moc"
#include "moc_quickbarapplet.cpp"
