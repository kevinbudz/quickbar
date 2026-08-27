/*
    SPDX-FileCopyrightText: 2016 Kai Uwe Broulik <kde@privat.broulik.de>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include <Plasma/Applet>

#include <QAbstractItemModel>
#include <QPointer>

#include <memory>

class QQuickItem;
class QMenu;

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
    void installMenuEventFilter(QMenu *menu);
    void removeMenuEventFilter(QMenu *menu);

    int m_currentIndex = -1;
    bool m_pendingMenuSwitch = false;
    bool m_hoverOpensMenu = true;
    int m_viewType = FullView;
    QPointer<QMenu> m_currentMenu;
    QPointer<QMenu> m_sourceMenu;
    QPointer<QQuickItem> m_buttonGrid;
    QPointer<QAbstractItemModel> m_model;
    std::unique_ptr<QMenu> m_proxyMenu;
    static int s_refs;
};
