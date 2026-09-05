/*
    SPDX-FileCopyrightText: 2016 Kai Uwe Broulik <kde@privat.broulik.de>
    SPDX-FileCopyrightText: 2026 QuickBar contributors

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "quickbarapplet.h"
#include "appmenumodel.h"

#include <QAction>
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusServiceWatcher>
#include <QKeyEvent>
#include <QMenu>
#include <QMouseEvent>
#include <QQuickItem>
#include <QQuickWindow>
#include <QScreen>
#include <QTimer>
#include <QWindow>

QSet<QuickBarApplet *> QuickBarApplet::s_activeApplets;
QPointer<QDBusServiceWatcher> QuickBarApplet::s_serviceWatcher;

namespace
{
QString viewService()
{
    return QStringLiteral("org.kde.kappmenuview");
}
}

void QuickBarApplet::registerService()
{
    auto bus = QDBusConnection::sessionBus();
    if (!bus.isConnected() || !bus.interface()) {
        return;
    }
    if (!bus.interface()->isServiceRegistered(viewService())) {
        bus.interface()->registerService(viewService(),
                                         QDBusConnectionInterface::QueueService,
                                         QDBusConnectionInterface::DontAllowReplacement);
    }
}

void QuickBarApplet::unregisterService()
{
    auto bus = QDBusConnection::sessionBus();
    if (!bus.isConnected() || !bus.interface()) {
        return;
    }
    if (bus.interface()->isServiceRegistered(viewService())) {
        bus.interface()->unregisterService(viewService());
    }
}

void QuickBarApplet::ensureServiceRegistered()
{
    if (!s_activeApplets.isEmpty()) {
        registerService();
    }
}

void QuickBarApplet::onAppletCreated(QuickBarApplet *applet)
{
    s_activeApplets.insert(applet);
    registerService();

    if (!s_serviceWatcher) {
        s_serviceWatcher = new QDBusServiceWatcher(viewService(),
                                                   QDBusConnection::sessionBus(),
                                                   QDBusServiceWatcher::WatchForUnregistration);
        QObject::connect(s_serviceWatcher.data(), &QDBusServiceWatcher::serviceUnregistered, [](const QString &service) {
            if (service == viewService() && !s_activeApplets.isEmpty()) {
                registerService();
            }
        });
    }
}

void QuickBarApplet::onAppletDestroyed(QuickBarApplet *applet)
{
    s_activeApplets.remove(applet);
    if (s_activeApplets.isEmpty()) {
        unregisterService();
        if (s_serviceWatcher) {
            delete s_serviceWatcher.data();
            s_serviceWatcher = nullptr;
        }
    }
}

void QuickBarApplet::onAppletDestroyedChanged(QuickBarApplet *applet, bool destroyed)
{
    if (destroyed) {
        s_activeApplets.remove(applet);
        if (s_activeApplets.isEmpty()) {
            unregisterService();
        }
    } else {
        s_activeApplets.insert(applet);
        registerService();
    }
}

QuickBarApplet::QuickBarApplet(QObject *parent, const KPluginMetaData &data, const QVariantList &args)
    : Plasma::Applet(parent, data, args)
{
    onAppletCreated(this);

    connect(this, &Applet::destroyedChanged, this, [this](bool destroyed) {
        onAppletDestroyedChanged(this, destroyed);
    });
}

QuickBarApplet::~QuickBarApplet()
{
    onAppletDestroyed(this);

    if (m_currentMenu && m_sourceMenu && m_currentMenu != m_sourceMenu) {
        auto menuAction = m_currentMenu->menuAction();
        for (QAction *action : m_currentMenu->actions()) {
            m_currentMenu->removeAction(action);
            if (m_sourceMenu) {
                m_sourceMenu->addAction(action);
            }
        }
        if (m_sourceMenu && menuAction) {
            menuAction->setMenu(m_sourceMenu);
        }
    }
    delete m_currentMenu.data();
}

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
        if (m_model) {
            disconnect(m_model, nullptr, this, nullptr);
        }
        m_model = model;
        if (m_model) {
            connect(m_model, &QAbstractItemModel::modelReset, this, [this] {
                if (m_currentMenu && m_currentMenu->isVisible()) {
                    m_currentMenu->hide();
                }
            });
        }
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

void QuickBarApplet::onMenuAboutToHide()
{
    if (m_currentMenu) {
        auto menuAction = m_currentMenu->menuAction();
        if (menuAction && m_sourceMenu) {
            menuAction->setMenu(m_sourceMenu);
        }
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

    if (!ctx || !ctx->window() || !ctx->window()->screen()) {
        return;
    }

    QMenu *actionMenu = createMenu(idx);
    if (actionMenu) {
        // this is a workaround where Qt will fail to realize a mouse has been released
        // this happens if a window which does not accept focus spawns a new window that takes focus and X grab
        // whilst the mouse is depressed
        // https://bugreports.qt.io/browse/QTBUG-59044
        // this causes the next click to go missing

        // by releasing manually we avoid that situation
        auto ungrabMouseHack = [ctx]() {
            if (ctx && ctx->window() && ctx->window()->mouseGrabberItem()) {
                // FIXME event forge thing enters press and hold move mode :/
                ctx->window()->mouseGrabberItem()->ungrabMouse();
            }
        };

        if (view() == FullView) {
            if (!m_currentMenu) {
                m_currentMenu = new QMenu(qobject_cast<QWidget *>(actionMenu->parent()));
                connect(m_currentMenu, &QMenu::aboutToHide, this, &QuickBarApplet::onMenuAboutToHide, Qt::UniqueConnection);
            } else if (m_sourceMenu != actionMenu) {
                auto menuAction = m_currentMenu->menuAction();
                for (QAction *action : m_currentMenu->actions()) {
                    m_currentMenu->removeAction(action);
                    if (m_sourceMenu) {
                        m_sourceMenu->addAction(action);
                    }
                }
                if (m_sourceMenu && menuAction) {
                    menuAction->setMenu(m_sourceMenu);
                }
            }
            m_sourceMenu = actionMenu;
            auto menuAction = m_sourceMenu->menuAction();
            for (QAction *action : m_sourceMenu->actions()) {
                m_sourceMenu->removeAction(action);
                m_currentMenu->addAction(action);
            }
            if (menuAction) {
                menuAction->setMenu(m_currentMenu);
            }
        } else {
            m_currentMenu = actionMenu;
            m_sourceMenu = actionMenu;
        }

        QTimer::singleShot(0, ctx, ungrabMouseHack);
        // end workaround

        const auto &geo = ctx->window()->screen()->availableVirtualGeometry();

        QPoint pos = ctx->window()->mapToGlobal(ctx->mapToScene(QPointF()).toPoint());

        const Qt::Edges edges = edgeFromLocation(location());
        m_currentMenu->setProperty("_breeze_menu_seamless_edges", QVariant::fromValue(edges));

        if (location() == Plasma::Types::TopEdge) {
            pos.setY(pos.y() + ctx->height());
        }

        m_currentMenu->adjustSize();

        pos = QPoint(qBound(geo.x(), pos.x(), geo.x() + geo.width() - m_currentMenu->width()),
                     qBound(geo.y(), pos.y(), geo.y() + geo.height() - m_currentMenu->height()));

        if (view() == FullView) {
            if (m_currentMenu->isVisible()) {
                m_currentMenu->move(pos);
            } else {
                m_currentMenu->installEventFilter(this);
                m_currentMenu->winId(); // create window handle
                m_currentMenu->windowHandle()->setTransientParent(ctx->window());
                m_currentMenu->popup(pos);
            }
        } else if (view() == CompactView) {
            if (m_currentMenu->isEmpty()) {
                // don't try to popup an empty menu in case the app gives us one
                return;
            }
            m_currentMenu->popup(pos);
            connect(actionMenu, &QMenu::aboutToHide, this, &QuickBarApplet::onMenuAboutToHide, Qt::UniqueConnection);
        }

        setCurrentIndex(idx);

        // FIXME TODO connect only once
    } else if (m_model) { // is it just an action without a menu?
        if (auto *action = m_model->index(idx, 0).data(AppMenuModel::ActionRole).value<QAction *>()) {
            Q_ASSERT(!action->menu());
            action->trigger();
        }
    }
}

// FIXME TODO doesn't work on submenu
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
        if (!m_hoverOpensMenu) {
            return false;
        }

        auto *e = static_cast<QMouseEvent *>(event);

        if (!m_buttonGrid || !m_buttonGrid->window()) {
            return false;
        }

        // FIXME the panel margin breaks Fitt's law :(
        const QPointF &windowLocalPos = m_buttonGrid->window()->mapFromGlobal(e->globalPosition());
        if (auto *contentItem = m_buttonGrid->parentItem()) {
            if (auto *scroller = contentItem->parentItem()) {
                const QPointF scrollerPos = scroller->mapFromScene(windowLocalPos);
                if (scrollerPos.x() < 0 || scrollerPos.y() < 0 || scrollerPos.x() > scroller->width() || scrollerPos.y() > scroller->height()) {
                    return false;
                }
            }
        }
        const QPointF &buttonGridLocalPos = m_buttonGrid->mapFromScene(windowLocalPos);
        auto *item = m_buttonGrid->childAt(buttonGridLocalPos.x(), buttonGridLocalPos.y());
        if (!item) {
            return false;
        }

        int buttonIndex = -1;
        bool ok = false;
        for (QQuickItem *cur = item; cur && cur != m_buttonGrid; cur = cur->parentItem()) {
            if (cur->property("buttonIndex").isValid()) {
                buttonIndex = cur->property("buttonIndex").toInt(&ok);
                if (ok) {
                    break;
                }
            }
        }
        if (!ok || buttonIndex < 0 || buttonIndex == m_currentIndex) {
            return false;
        }

        Q_EMIT requestActivateIndex(buttonIndex);
    }

    return false;
}

K_PLUGIN_CLASS_WITH_JSON(QuickBarApplet, "metadata.json")

#include "quickbarapplet.moc"
#include "moc_quickbarapplet.cpp"
