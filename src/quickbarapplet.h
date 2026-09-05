/*
    SPDX-FileCopyrightText: 2016 Kai Uwe Broulik <kde@privat.broulik.de>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include <Plasma/Applet>

#include <QAbstractItemModel>
#include <QPointer>
#include <QSet>

class QQuickItem;
class QMenu;
class QDBusServiceWatcher;

class QuickBarApplet : public Plasma::Applet
{
    Q_OBJECT

    Q_PROPERTY(QObject *containment READ containment CONSTANT)
    Q_PROPERTY(QAbstractItemModel *model READ model WRITE setModel NOTIFY modelChanged)

    Q_PROPERTY(int view READ view WRITE setView NOTIFY viewChanged)

    Q_PROPERTY(int currentIndex READ currentIndex NOTIFY currentIndexChanged)

    Q_PROPERTY(QQuickItem *buttonGrid READ buttonGrid WRITE setButtonGrid NOTIFY buttonGridChanged)
    Q_PROPERTY(bool hoverOpensMenu READ hoverOpensMenu WRITE setHoverOpensMenu NOTIFY hoverOpensMenuChanged)

public:
    enum ViewType {
        FullView,
        CompactView,
    };

    explicit QuickBarApplet(QObject *parent, const KPluginMetaData &data, const QVariantList &args);
    ~QuickBarApplet() override;

    void init() override;

    int currentIndex() const;

    QQuickItem *buttonGrid() const;
    void setButtonGrid(QQuickItem *buttonGrid);

    bool hoverOpensMenu() const;
    void setHoverOpensMenu(bool hover);

    QAbstractItemModel *model() const;
    void setModel(QAbstractItemModel *model);

    int view() const;
    void setView(int type);

    static void ensureServiceRegistered();

Q_SIGNALS:
    void modelChanged();
    void viewChanged();
    void currentIndexChanged();
    void buttonGridChanged();
    void hoverOpensMenuChanged();
    void requestActivateIndex(int index);

public Q_SLOTS:
    void trigger(QQuickItem *ctx, int idx);

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    QMenu *createMenu(int idx) const;
    void setCurrentIndex(int currentIndex);
    void onMenuAboutToHide();
    // Return actions stolen into the owned FullView proxy menu to their source
    // menu and re-link the source action. No-op unless we own the proxy.
    void restoreStolenActions();
    // Hide any popup, restore stolen actions and reset the current index.
    // Keeps the (now empty) owned proxy menu for reuse, but forgets the
    // model-owned source menu so a view/model switch can't corrupt it.
    void resetMenuState();

    static void registerService();
    static void unregisterService();
    static void onAppletCreated(QuickBarApplet *applet);
    static void onAppletDestroyed(QuickBarApplet *applet);
    static void onAppletDestroyedChanged(QuickBarApplet *applet, bool destroyed);

    static QSet<QuickBarApplet *> s_activeApplets;
    static QPointer<QDBusServiceWatcher> s_serviceWatcher;

    int m_currentIndex = -1;
    bool m_hoverOpensMenu = true;
    int m_viewType = FullView;
    QPointer<QMenu> m_currentMenu;
    QPointer<QMenu> m_sourceMenu;
    // True only when m_currentMenu is the heap-allocated reusable FullView
    // proxy menu owned by this applet. In CompactView both pointers borrow a
    // model-owned menu and must never be deleted or used as a proxy.
    bool m_ownsCurrentMenu = false;
    QPointer<QQuickItem> m_buttonGrid;
    QPointer<QAbstractItemModel> m_model;
};
