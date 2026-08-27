/*
    SPDX-FileCopyrightText: 2026 QuickBar contributors

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "genericmenu.h"

#include <KLocalizedString>
#include <QAction>
#include <QKeySequence>
#include <QMenu>

#include <csignal>

namespace
{
constexpr auto genericActionProperty = "quickbarGenericAction";
constexpr auto quitApplicationAction = "quitApplication";
constexpr auto minimizeWindowAction = "minimizeWindow";
constexpr auto maximizeWindowAction = "maximizeWindow";
constexpr auto openAboutAction = "openAbout";

QAction *addAction(QMenu *menu, const QString &text, QKeySequence::StandardKey key)
{
    auto *action = menu->addAction(text);
    action->setShortcut(QKeySequence(key));
    return action;
}
} // namespace

QMenu *GenericMenu::create(QObject *parent)
{
    auto *menu = new QMenu;
    if (parent) {
        menu->QObject::setParent(parent);
    }
    menu->setObjectName(QStringLiteral("quickbar-generic-menu"));

    auto *fileMenu = menu->addMenu(i18n("&File"));
    auto *quitAction = fileMenu->addAction(i18n("Quit Application"));
    quitAction->setShortcut(QKeySequence::Close);
    quitAction->setProperty(genericActionProperty, QString::fromLatin1(quitApplicationAction));

    auto *editMenu = menu->addMenu(i18n("&Edit"));
    addAction(editMenu, i18n("&Undo"), QKeySequence::Undo);
    addAction(editMenu, i18n("&Redo"), QKeySequence::Redo);
    editMenu->addSeparator();
    addAction(editMenu, i18n("Cu&t"), QKeySequence::Cut);
    addAction(editMenu, i18n("&Copy"), QKeySequence::Copy);
    addAction(editMenu, i18n("&Paste"), QKeySequence::Paste);
    editMenu->addSeparator();
    addAction(editMenu, i18n("Select &All"), QKeySequence::SelectAll);

    auto *viewMenu = menu->addMenu(i18n("&View"));
    auto *minimizeAction = viewMenu->addAction(i18n("Minimize"));
    minimizeAction->setProperty(genericActionProperty, QString::fromLatin1(minimizeWindowAction));
    auto *maximizeAction = viewMenu->addAction(i18n("Maximize"));
    maximizeAction->setProperty(genericActionProperty, QString::fromLatin1(maximizeWindowAction));

    auto *helpMenu = menu->addMenu(i18n("&Help"));
    auto *aboutAction = helpMenu->addAction(i18n("&About"));
    aboutAction->setProperty(genericActionProperty, QString::fromLatin1(openAboutAction));

    return menu;
}
