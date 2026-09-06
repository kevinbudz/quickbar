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
#include <QGuiApplication>
#include <QKeyEvent>
#include <QMenu>
#include <QMouseEvent>
#include <QProxyStyle>
#include <QQuickItem>
#include <QQuickWindow>
#include <QScreen>
#include <QStyle>
#include <QStyleOptionMenuItem>
#include <QTimer>
#include <QTimerEvent>
#include <QWheelEvent>
#include <QWindow>
#include <QtWidgets/private/qmenu_p.h>
#include <KConfigGroup>

class MenuClampStyle : public QProxyStyle
{
public:
    int maxMenuWidth = 600;

    explicit MenuClampStyle(QStyle *baseStyle = nullptr)
        : QProxyStyle(baseStyle)
    {
    }

    QSize sizeFromContents(ContentsType type, const QStyleOption *opt,
                           const QSize &contentsSize, const QWidget *widget) const override
    {
        QSize sz = QProxyStyle::sizeFromContents(type, opt, contentsSize, widget);
        if (type == CT_MenuItem) {
            int reservedShortcut = 0;
            if (const auto *m = qstyleoption_cast<const QStyleOptionMenuItem *>(opt)) {
                reservedShortcut = m->reservedShortcutWidth;
            }
            const int maxItemWidth = maxMenuWidth - reservedShortcut - 24;
            if (maxItemWidth > 50) {
                sz.setWidth(qMin(sz.width(), maxItemWidth));
            }
        }
        return sz;
    }

    void drawControl(ControlElement element, const QStyleOption *opt,
                     QPainter *p, const QWidget *widget) const override
    {
        if (element == CE_MenuItem) {
            if (const auto *menuItem = qstyleoption_cast<const QStyleOptionMenuItem *>(opt)) {
                QStyleOptionMenuItem copy = *menuItem;
                const int reservedRight = (copy.reservedShortcutWidth > 0 ? copy.reservedShortcutWidth + 16 : 0)
                                        + (copy.menuItemType == QStyleOptionMenuItem::SubMenu ? 16 : 0);
                const int reservedLeft = copy.maxIconWidth > 0 ? copy.maxIconWidth + 12 : 8;
                const int maxTextWidth = copy.rect.width() - reservedLeft - reservedRight - 16;

                QString text = copy.text;
                const int tabIndex = text.indexOf(QLatin1Char('\t'));
                if (tabIndex != -1) {
                    QString label = text.left(tabIndex);
                    const QString shortcut = text.mid(tabIndex);
                    if (maxTextWidth > 40 && copy.fontMetrics.horizontalAdvance(label) > maxTextWidth) {
                        label = copy.fontMetrics.elidedText(label, Qt::ElideRight, maxTextWidth);
                    }
                    copy.text = label + shortcut;
                } else {
                    if (maxTextWidth > 40 && copy.fontMetrics.horizontalAdvance(text) > maxTextWidth) {
                        copy.text = copy.fontMetrics.elidedText(text, Qt::ElideRight, maxTextWidth);
                    }
                }
                QProxyStyle::drawControl(element, &copy, p, widget);
                return;
            }
        }
        QProxyStyle::drawControl(element, opt, p, widget);
    }
};

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
    m_menuStyle = std::make_unique<MenuClampStyle>();
    onAppletCreated(this);

    connect(this, &Applet::destroyedChanged, this, [this](bool destroyed) {
        onAppletDestroyedChanged(this, destroyed);
    });
}

QuickBarApplet::~QuickBarApplet()
{
    onAppletDestroyed(this);

    resetMenuState();
    // Only the FullView proxy menu is owned by the applet. A CompactView
    // menu is borrowed from the model and must never be deleted here.
    if (m_ownsCurrentMenu && m_currentMenu) {
        m_currentMenu->removeEventFilter(this);
        delete m_currentMenu.data();
    }
    m_currentMenu.clear();
    m_sourceMenu.clear();
    m_ownsCurrentMenu = false;
}

void QuickBarApplet::restoreStolenActions()
{
    if (!m_ownsCurrentMenu || !m_currentMenu) {
        return;
    }
    QMenu *current = m_currentMenu.data();
    QMenu *source = m_sourceMenu.data();
    auto menuAction = current->menuAction();
    const QList<QAction *> actions = current->actions();
    for (QAction *action : actions) {
        current->removeAction(action);
        if (source) {
            source->addAction(action);
        }
    }
    if (source && menuAction) {
        menuAction->setMenu(source);
    }
}

void QuickBarApplet::resetMenuState()
{
    if (m_currentMenu) {
        m_currentMenu->removeEventFilter(this);
        if (m_currentMenu->isVisible()) {
            m_currentMenu->hide();
        }
        auto d = QMenuPrivate::get(m_currentMenu.data());
        if (d && d->scroll) {
            d->scroll->scrollOffset = 0;
            d->scroll->scrollFlags = QMenuPrivate::QMenuScroller::ScrollNone;
        }
        m_currentMenu->setMaximumWidth(QWIDGETSIZE_MAX);
        m_currentMenu->setMaximumHeight(QWIDGETSIZE_MAX);
        if (!m_ownsCurrentMenu) {
            m_currentMenu->setStyle(nullptr);
        }
    }
    restoreStolenActions();
    setCurrentIndex(-1);
}

void QuickBarApplet::init()
{
    m_maxMenuCells = config().readEntry("maxMenuCells", DefaultMaxMenuCells);
}

QAbstractItemModel *QuickBarApplet::model() const
{
    return m_model;
}

void QuickBarApplet::setModel(QAbstractItemModel *model)
{
    if (m_model != model) {
        // The stolen actions (if any) belong to the old model: give them back
        // before switching, and forget the old model-owned source menu.
        resetMenuState();
        m_sourceMenu.clear();
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
        // Switching between the single-button menu and the full menubar must
        // not carry menu state across: the FullView proxy holds actions stolen
        // from one submenu, while CompactView borrows a model-owned menu.
        // Mixing the two corrupts the model and crashes plasmashell.
        resetMenuState();
        if (m_ownsCurrentMenu) {
            // Keep the (now empty) reusable proxy for the next FullView use,
            // but forget the model-owned source menu.
            m_sourceMenu.clear();
        } else {
            // Borrowed model-owned menus: drop without deleting or mutating.
            m_currentMenu.clear();
            m_sourceMenu.clear();
        }
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

int QuickBarApplet::maxMenuCells() const
{
    return m_maxMenuCells;
}

void QuickBarApplet::setMaxMenuCells(int cells)
{
    if (m_maxMenuCells != cells) {
        m_maxMenuCells = cells;
        Q_EMIT maxMenuCellsChanged();
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
        m_currentMenu->removeEventFilter(this);
        auto d = QMenuPrivate::get(m_currentMenu.data());
        if (d && d->scroll) {
            d->scroll->scrollOffset = 0;
            d->scroll->scrollFlags = QMenuPrivate::QMenuScroller::ScrollNone;
        }
        m_currentMenu->setMaximumWidth(QWIDGETSIZE_MAX);
        m_currentMenu->setMaximumHeight(QWIDGETSIZE_MAX);
        if (!m_ownsCurrentMenu) {
            m_currentMenu->setStyle(nullptr);
        }
        auto menuAction = m_currentMenu->menuAction();
        if (menuAction && m_sourceMenu) {
            menuAction->setMenu(m_sourceMenu);
        }
    }
    setCurrentIndex(-1);
}

int QuickBarApplet::calculateMaxHeightForCells(QMenu *menu, int maxCells) const
{
    if (!menu) {
        return 0;
    }

    menu->ensurePolished();

    int cellHeight = 0;
    for (const QAction *action : menu->actions()) {
        if (!action->isSeparator() && action->isVisible()) {
            const QRect rect = menu->actionGeometry(const_cast<QAction *>(action));
            if (rect.height() > 0) {
                cellHeight = rect.height();
                break;
            }
        }
    }
    if (cellHeight <= 0) {
        cellHeight = menu->fontMetrics().height() + 12;
    }

    int cellsSeen = 0;
    int totalHeight = 0;
    for (const QAction *action : menu->actions()) {
        if (!action->isVisible()) {
            continue;
        }
        const QRect rect = menu->actionGeometry(const_cast<QAction *>(action));
        const int h = rect.height() > 0 ? rect.height() : (action->isSeparator() ? 6 : cellHeight);
        totalHeight += h;
        if (!action->isSeparator()) {
            cellsSeen++;
            if (cellsSeen >= maxCells) {
                break;
            }
        }
    }

    QStyle *style = menu->style();
    const int vmargin = style ? style->pixelMetric(QStyle::PM_MenuVMargin, nullptr, menu) : 2;
    const int fw = style ? style->pixelMetric(QStyle::PM_MenuPanelWidth, nullptr, menu) : 2;
    totalHeight += (vmargin + fw) * 2;

    return totalHeight;
}

int QuickBarApplet::totalContentHeight(QMenu *menu) const
{
    if (!menu) {
        return 0;
    }
    auto d = QMenuPrivate::get(menu);
    if (!d || d->actionRects.isEmpty()) {
        return menu->sizeHint().height();
    }
    int total = 0;
    for (const QRect &r : d->actionRects) {
        total += r.height();
    }
    QStyle *style = menu->style();
    const int vmargin = style ? style->pixelMetric(QStyle::PM_MenuVMargin, nullptr, menu) : 2;
    const int fw = style ? style->pixelMetric(QStyle::PM_MenuPanelWidth, nullptr, menu) : 2;
    return total + (vmargin + fw) * 2;
}

bool QuickBarApplet::handleMenuWheel(QMenu *menu, QWheelEvent *e)
{
    if (!menu) {
        return false;
    }

    auto d = QMenuPrivate::get(menu);
    if (!d) {
        return false;
    }

    if (!d->scroll) {
        d->scroll = new QMenuPrivate::QMenuScroller;
    }

    const int totalHeight = totalContentHeight(menu);
    const int visibleHeight = menu->height();

    int deltaY = e->pixelDelta().y();
    if (deltaY == 0) {
        deltaY = e->angleDelta().y();
    }

    if (totalHeight > visibleHeight && deltaY != 0) {
        int cellHeight = 29;
        for (int i = 0; i < d->actions.size(); ++i) {
            if (!d->actionRects.at(i).isNull()) {
                cellHeight = d->actionRects.at(i).height();
                break;
            }
        }

        int step = 0;
        if (e->pixelDelta().y() != 0) {
            step = e->pixelDelta().y();
        } else {
            step = (e->angleDelta().y() / 120) * cellHeight * 3;
            if (step == 0) {
                step = e->angleDelta().y() > 0 ? cellHeight : -cellHeight;
            }
        }

        const int minOffset = visibleHeight - totalHeight;
        const int oldOffset = d->scroll->scrollOffset;
        const int newOffset = qBound(minOffset, oldOffset + step, 0);
        const int shift = newOffset - oldOffset;

        if (shift != 0) {
            for (int i = 0; i < d->actionRects.size(); ++i) {
                d->actionRects[i].moveTop(d->actionRects[i].top() + shift);
                if (QWidget *w = d->widgetItems.value(d->actions.at(i))) {
                    w->setGeometry(d->actionRects[i]);
                }
            }
            d->scroll->scrollOffset = newOffset;
            uint newFlags = QMenuPrivate::QMenuScroller::ScrollNone;
            if (newOffset < 0) {
                newFlags |= QMenuPrivate::QMenuScroller::ScrollUp;
            }
            if (newOffset > minOffset) {
                newFlags |= QMenuPrivate::QMenuScroller::ScrollDown;
            }
            d->scroll->scrollFlags = newFlags;
            menu->update();
        }
    }

    e->accept();
    return true;
}

void QuickBarApplet::clampActionRects(QMenu *menu) const
{
    if (!menu) {
        return;
    }
    auto d = QMenuPrivate::get(menu);
    if (!d) {
        return;
    }
    QStyle *style = menu->style();
    const int fw = style ? style->pixelMetric(QStyle::PM_MenuPanelWidth, nullptr, menu) : 2;
    const int hmargin = style ? style->pixelMetric(QStyle::PM_MenuHMargin, nullptr, menu) : 2;
    const int maxInnerWidth = menu->width() - (fw + hmargin) * 2;
    for (int i = 0; i < d->actionRects.size(); ++i) {
        if (d->actionRects[i].width() > maxInnerWidth) {
            d->actionRects[i].setWidth(maxInnerWidth);
            if (QWidget *w = d->widgetItems.value(d->actions.at(i))) {
                w->setGeometry(d->actionRects[i]);
            }
        }
    }
}

void QuickBarApplet::clampSubmenu(QMenu *sub)
{
    if (!sub) {
        return;
    }
    if (m_menuStyle) {
        sub->setStyle(m_menuStyle.get());
    }
    QScreen *screen = nullptr;
    if (m_buttonGrid && m_buttonGrid->window() && m_buttonGrid->window()->screen()) {
        screen = m_buttonGrid->window()->screen();
    } else {
        screen = QGuiApplication::primaryScreen();
    }
    const QRect geo = screen ? screen->availableVirtualGeometry() : QRect(0, 0, 1920, 1080);
    const int clampedWidth = qBound(200, MaxMenuWidthPx, geo.width() - 40);
    sub->setMaximumWidth(clampedWidth);

    int clampedHeight = geo.height() - 40;
    if (m_maxMenuCells > 0) {
        const int cellsHeight = calculateMaxHeightForCells(sub, m_maxMenuCells);
        clampedHeight = qMin(cellsHeight, geo.height() - 40);
    }
    if (clampedHeight > 0) {
        sub->setMaximumHeight(clampedHeight);
    }
    sub->adjustSize();

    auto d = QMenuPrivate::get(sub);
    if (d) {
        if (!d->scroll) {
            d->scroll = new QMenuPrivate::QMenuScroller;
        }
        d->scroll->scrollOffset = 0;
        if (totalContentHeight(sub) > sub->height()) {
            d->scroll->scrollFlags = QMenuPrivate::QMenuScroller::ScrollDown;
        } else {
            d->scroll->scrollFlags = QMenuPrivate::QMenuScroller::ScrollNone;
        }
    }
    clampActionRects(sub);
    hookSubmenus(sub);
}

void QuickBarApplet::hookSubmenus(QMenu *menu)
{
    if (!menu) {
        return;
    }
    for (QAction *action : menu->actions()) {
        if (action->toolTip().isEmpty()) {
            action->setToolTip(action->text());
        }
        if (QMenu *sub = action->menu()) {
            if (m_menuStyle) {
                sub->setStyle(m_menuStyle.get());
            }
            sub->removeEventFilter(this);
            sub->installEventFilter(this);
            connect(sub, &QMenu::aboutToShow, this, [this, sub]() {
                clampSubmenu(sub);
            }, Qt::UniqueConnection);
            connect(sub, &QMenu::aboutToHide, this, [sub]() {
                auto d = QMenuPrivate::get(sub);
                if (d && d->scroll) {
                    d->scroll->scrollOffset = 0;
                    d->scroll->scrollFlags = QMenuPrivate::QMenuScroller::ScrollNone;
                }
                sub->setMaximumWidth(QWIDGETSIZE_MAX);
                sub->setMaximumHeight(QWIDGETSIZE_MAX);
            }, Qt::UniqueConnection);
            hookSubmenus(sub);
        }
    }
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
            if (!m_currentMenu || !m_ownsCurrentMenu) {
                // Fresh state or returning from CompactView, whose pointers
                // borrow a model-owned menu: never reuse that as the proxy.
                // Drop the borrowed refs and allocate our own reusable menu.
                m_currentMenu.clear();
                m_sourceMenu.clear();
                m_currentMenu = new QMenu(qobject_cast<QWidget *>(actionMenu->parent()));
                if (m_menuStyle) {
                    m_currentMenu->setStyle(m_menuStyle.get());
                }
                m_ownsCurrentMenu = true;
                connect(m_currentMenu, &QMenu::aboutToHide, this, &QuickBarApplet::onMenuAboutToHide, Qt::UniqueConnection);
            } else if (m_sourceMenu != actionMenu) {
                m_currentMenu->setMaximumWidth(QWIDGETSIZE_MAX);
                m_currentMenu->setMaximumHeight(QWIDGETSIZE_MAX);
                auto d = QMenuPrivate::get(m_currentMenu.data());
                if (d && d->scroll) {
                    d->scroll->scrollOffset = 0;
                    d->scroll->scrollFlags = QMenuPrivate::QMenuScroller::ScrollNone;
                }
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
            // CompactView borrows the model-owned menu directly. If a FullView
            // proxy still holds stolen actions, return them back to the model
            // first — otherwise the source submenu is left empty and the next
            // FullView use reads freed/corrupted state and crashes.
            restoreStolenActions();
            if (m_ownsCurrentMenu && m_currentMenu) {
                m_currentMenu->removeEventFilter(this);
                delete m_currentMenu.data();
            }
            m_currentMenu.clear();
            m_currentMenu = actionMenu;
            if (m_menuStyle) {
                m_currentMenu->setStyle(m_menuStyle.get());
            }
            m_sourceMenu = actionMenu;
            m_ownsCurrentMenu = false;
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

        const int clampedWidth = qBound(200, MaxMenuWidthPx, geo.width() - 40);
        if (m_menuStyle) {
            m_menuStyle->maxMenuWidth = clampedWidth;
        }
        m_currentMenu->setMaximumWidth(clampedWidth);

        int screenMaxHeight = geo.height() - 20;
        if (location() == Plasma::Types::TopEdge) {
            screenMaxHeight = geo.y() + geo.height() - pos.y() - 10;
        } else if (location() == Plasma::Types::BottomEdge) {
            screenMaxHeight = pos.y() - geo.y() - 10;
        }
        const int clampedHeight = (m_maxMenuCells > 0)
            ? qMin(calculateMaxHeightForCells(m_currentMenu.data(), m_maxMenuCells), screenMaxHeight)
            : screenMaxHeight;
        if (clampedHeight > 0) {
            m_currentMenu->setMaximumHeight(clampedHeight);
        }

        for (QAction *action : m_currentMenu->actions()) {
            if (action->toolTip().isEmpty()) {
                action->setToolTip(action->text());
            }
        }

        m_currentMenu->adjustSize();

        const int maxX = qMax(geo.x(), geo.x() + geo.width() - m_currentMenu->width());
        const int maxY = qMax(geo.y(), geo.y() + geo.height() - m_currentMenu->height());
        pos = QPoint(qBound(geo.x(), pos.x(), maxX),
                     qBound(geo.y(), pos.y(), maxY));

        auto d = QMenuPrivate::get(m_currentMenu.data());
        if (d && !d->scroll) {
            d->scroll = new QMenuPrivate::QMenuScroller;
        }

        if (view() == FullView) {
            m_currentMenu->removeEventFilter(this);
            m_currentMenu->installEventFilter(this);
            m_currentMenu->winId(); // create window handle
            m_currentMenu->windowHandle()->setTransientParent(ctx->window());
            m_currentMenu->popup(pos);

            auto d = QMenuPrivate::get(m_currentMenu.data());
            if (d) {
                d->scroll->scrollOffset = 0;
                if (totalContentHeight(m_currentMenu.data()) > m_currentMenu->height()) {
                    d->scroll->scrollFlags = QMenuPrivate::QMenuScroller::ScrollDown;
                } else {
                    d->scroll->scrollFlags = QMenuPrivate::QMenuScroller::ScrollNone;
                }
            }
            clampActionRects(m_currentMenu.data());
            hookSubmenus(m_currentMenu.data());
        } else if (view() == CompactView) {
            if (m_currentMenu->isEmpty()) {
                // don't try to popup an empty menu in case the app gives us one
                return;
            }
            m_currentMenu->removeEventFilter(this);
            m_currentMenu->installEventFilter(this);
            m_currentMenu->popup(pos);

            auto d = QMenuPrivate::get(m_currentMenu.data());
            if (d) {
                d->scroll->scrollOffset = 0;
                if (totalContentHeight(m_currentMenu.data()) > m_currentMenu->height()) {
                    d->scroll->scrollFlags = QMenuPrivate::QMenuScroller::ScrollDown;
                } else {
                    d->scroll->scrollFlags = QMenuPrivate::QMenuScroller::ScrollNone;
                }
            }
            clampActionRects(m_currentMenu.data());
            hookSubmenus(m_currentMenu.data());
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

bool QuickBarApplet::eventFilter(QObject *watched, QEvent *event)
{
    auto *menu = qobject_cast<QMenu *>(watched);
    if (!menu) {
        return false;
    }

    if (event->type() == QEvent::Paint || event->type() == QEvent::LayoutRequest || event->type() == QEvent::Show) {
        clampActionRects(menu);
    }

    if (event->type() == QEvent::Wheel) {
        auto *e = static_cast<QWheelEvent *>(event);
        handleMenuWheel(menu, e);
        return true;
    }

    if (event->type() == QEvent::Timer) {
        auto d = QMenuPrivate::get(menu);
        if (d && d->scroll && static_cast<QTimerEvent *>(event)->timerId() == d->scroll->scrollTimer.timerId()) {
            const int totalHeight = totalContentHeight(menu);
            const int visibleHeight = menu->height();
            if (totalHeight > visibleHeight) {
                int cellHeight = 29;
                for (int i = 0; i < d->actions.size(); ++i) {
                    if (!d->actionRects.at(i).isNull()) {
                        cellHeight = d->actionRects.at(i).height();
                        break;
                    }
                }
                const int step = (d->scroll->scrollDirection == QMenuPrivate::QMenuScroller::ScrollUp) ? cellHeight : -cellHeight;
                const int minOffset = visibleHeight - totalHeight;
                const int oldOffset = d->scroll->scrollOffset;
                const int newOffset = qBound(minOffset, oldOffset + step, 0);
                const int shift = newOffset - oldOffset;
                if (shift != 0) {
                    for (int i = 0; i < d->actionRects.size(); ++i) {
                        d->actionRects[i].moveTop(d->actionRects[i].top() + shift);
                        if (QWidget *w = d->widgetItems.value(d->actions.at(i))) {
                            w->setGeometry(d->actionRects[i]);
                        }
                    }
                    d->scroll->scrollOffset = newOffset;
                    uint newFlags = QMenuPrivate::QMenuScroller::ScrollNone;
                    if (newOffset < 0) {
                        newFlags |= QMenuPrivate::QMenuScroller::ScrollUp;
                    }
                    if (newOffset > minOffset) {
                        newFlags |= QMenuPrivate::QMenuScroller::ScrollDown;
                    }
                    d->scroll->scrollFlags = newFlags;
                    menu->update();
                } else {
                    d->scroll->scrollTimer.stop();
                }
            }
            return true;
        }
    }

    if (menu != m_currentMenu) {
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
