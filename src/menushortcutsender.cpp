/*
    SPDX-FileCopyrightText: 2026 QuickBar contributors

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "menushortcutsender.h"

#include <abstracttasksmodel.h>
#include <tasksmodel.h>

#include <KWindowSystem>
#include <KX11Extras>
#include <QAction>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusObjectPath>
#include <QDBusVariant>
#include <QEventLoop>
#include <QGuiApplication>
#include <QKeyCombination>
#include <QProcess>
#include <QStandardPaths>
#include <QTimer>
#include <QVariantMap>

#include <algorithm>
#include <linux/input-event-codes.h>
#include <utility>

#if QT_CONFIG(xcb)
#include <KKeyServer>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/extensions/XTest.h>
#include <fixx11h.h>
#include <qguiapplication_platform.h>
#endif

namespace
{
#if QT_CONFIG(xcb)
Display *x11Display()
{
    if (!KWindowSystem::isPlatformX11()) {
        return nullptr;
    }
    auto *x11 = qGuiApp->nativeInterface<QNativeInterface::QX11Application>();
    return x11 ? x11->display() : nullptr;
}

void fakeKey(Display *display, int keyCode, bool press)
{
    XTestFakeKeyEvent(display, keyCode, press, CurrentTime);
    XFlush(display);
}

void sendKeyQt(Display *display, int keyQt)
{
    uint modMask = 0;
    if (!KKeyServer::keyQtToModX(keyQt, &modMask)) {
        modMask = 0;
    }

    const QList<int> codes = KKeyServer::keyQtToCodeXs(keyQt);
    if (codes.isEmpty()) {
        return;
    }

    if (modMask & KKeyServer::modXShift()) {
        fakeKey(display, XKeysymToKeycode(display, XK_Shift_L), true);
    }
    if (modMask & KKeyServer::modXCtrl()) {
        fakeKey(display, XKeysymToKeycode(display, XK_Control_L), true);
    }
    if (modMask & KKeyServer::modXAlt()) {
        fakeKey(display, XKeysymToKeycode(display, XK_Alt_L), true);
    }
    if (modMask & KKeyServer::modXMeta()) {
        fakeKey(display, XKeysymToKeycode(display, XK_Super_L), true);
    }

    for (int code : codes) {
        fakeKey(display, code, true);
        fakeKey(display, code, false);
    }

    if (modMask & KKeyServer::modXMeta()) {
        fakeKey(display, XKeysymToKeycode(display, XK_Super_L), false);
    }
    if (modMask & KKeyServer::modXAlt()) {
        fakeKey(display, XKeysymToKeycode(display, XK_Alt_L), false);
    }
    if (modMask & KKeyServer::modXCtrl()) {
        fakeKey(display, XKeysymToKeycode(display, XK_Control_L), false);
    }
    if (modMask & KKeyServer::modXShift()) {
        fakeKey(display, XKeysymToKeycode(display, XK_Shift_L), false);
    }
}

bool sendViaX11(const QKeyCombination &combination)
{
    Display *display = x11Display();
    if (!display) {
        return false;
    }

    KKeyServer::initializeMods();

    const QList<int> codes = KKeyServer::keyQtToCodeXs(int(combination.toCombined()));
    if (codes.isEmpty()) {
        return false;
    }

    const WId active = KX11Extras::activeWindow();
    if (active) {
        KX11Extras::activateWindow(active);
    }

    sendKeyQt(display, int(combination.toCombined()));
    return true;
}
#endif

QString qtKeyToWtypeName(Qt::Key key)
{
    if (key >= Qt::Key_A && key <= Qt::Key_Z) {
        return QString(QChar('a' + (key - Qt::Key_A)));
    }
    if (key >= Qt::Key_0 && key <= Qt::Key_9) {
        return QString(QChar('0' + (key - Qt::Key_0)));
    }
    if (key >= Qt::Key_F1 && key <= Qt::Key_F35) {
        return QStringLiteral("F%1").arg(1 + (key - Qt::Key_F1));
    }

    switch (key) {
    case Qt::Key_Return:
    case Qt::Key_Enter:
        return QStringLiteral("Return");
    case Qt::Key_Escape:
        return QStringLiteral("Escape");
    case Qt::Key_Tab:
        return QStringLiteral("Tab");
    case Qt::Key_Backspace:
        return QStringLiteral("BackSpace");
    case Qt::Key_Delete:
        return QStringLiteral("Delete");
    case Qt::Key_Insert:
        return QStringLiteral("Insert");
    case Qt::Key_Home:
        return QStringLiteral("Home");
    case Qt::Key_End:
        return QStringLiteral("End");
    case Qt::Key_PageUp:
        return QStringLiteral("Page_Up");
    case Qt::Key_PageDown:
        return QStringLiteral("Page_Down");
    case Qt::Key_Left:
        return QStringLiteral("Left");
    case Qt::Key_Right:
        return QStringLiteral("Right");
    case Qt::Key_Up:
        return QStringLiteral("Up");
    case Qt::Key_Down:
        return QStringLiteral("Down");
    case Qt::Key_Space:
        return QStringLiteral("space");
    case Qt::Key_Plus:
        return QStringLiteral("plus");
    case Qt::Key_Minus:
        return QStringLiteral("minus");
    case Qt::Key_Equal:
        return QStringLiteral("equal");
    case Qt::Key_Comma:
        return QStringLiteral("comma");
    case Qt::Key_Period:
        return QStringLiteral("period");
    case Qt::Key_Slash:
        return QStringLiteral("slash");
    case Qt::Key_Backslash:
        return QStringLiteral("backslash");
    case Qt::Key_BracketLeft:
        return QStringLiteral("bracketleft");
    case Qt::Key_BracketRight:
        return QStringLiteral("bracketright");
    case Qt::Key_Semicolon:
        return QStringLiteral("semicolon");
    case Qt::Key_Apostrophe:
        return QStringLiteral("apostrophe");
    case Qt::Key_QuoteLeft:
        return QStringLiteral("grave");
    default:
        return {};
    }
}

bool sendViaWtype(const QKeyCombination &combination)
{
    const QString wtype = QStandardPaths::findExecutable(QStringLiteral("wtype"));
    if (wtype.isEmpty()) {
        return false;
    }

    const int combined = int(combination.toCombined());
    const auto mods = Qt::KeyboardModifiers(combined & Qt::KeyboardModifierMask);
    const auto key = static_cast<Qt::Key>(combined & ~Qt::KeyboardModifierMask);

    const QString keyName = qtKeyToWtypeName(key);
    if (keyName.isEmpty()) {
        return false;
    }

    QStringList args;
    if (mods & Qt::ControlModifier) {
        args << QStringLiteral("-M") << QStringLiteral("ctrl");
    }
    if (mods & Qt::AltModifier) {
        args << QStringLiteral("-M") << QStringLiteral("alt");
    }
    if (mods & Qt::MetaModifier) {
        args << QStringLiteral("-M") << QStringLiteral("super");
    }
    if (mods & Qt::ShiftModifier) {
        args << QStringLiteral("-M") << QStringLiteral("shift");
    }
    args << QStringLiteral("-k") << keyName;

    return QProcess::startDetached(wtype, args);
}

bool sendCombination(const QKeyCombination &combination)
{
    if (KWindowSystem::isPlatformWayland()) {
        return sendViaWtype(combination);
    }

#if QT_CONFIG(xcb)
    if (KWindowSystem::isPlatformX11() && sendViaX11(combination)) {
        return true;
    }
#endif
    return sendViaWtype(combination);
}
} // namespace

MenuShortcutBridge::MenuShortcutBridge(TaskManager::TasksModel *tasksModel, QObject *parent)
    : QObject(parent)
    , m_tasksModel(tasksModel)
{
}

void MenuShortcutBridge::wireMenu(QMenu *menu)
{
    if (!menu) {
        return;
    }

    const auto actions = menu->findChildren<QAction *>();
    for (QAction *action : actions) {
        if (action->isSeparator() || action->shortcut().isEmpty()) {
            continue;
        }

        const QKeySequence shortcut = action->shortcut();
        connect(action, &QAction::triggered, this, [this, shortcut] {
            scheduleSend(shortcut);
        });
    }
}

void MenuShortcutBridge::scheduleSend(const QKeySequence &sequence)
{
    if (sequence.isEmpty()) {
        return;
    }

    m_pendingSequence = sequence;
    activateTarget();

    QTimer::singleShot(100, this, &MenuShortcutBridge::sendPendingShortcut);
}

void MenuShortcutBridge::activateTarget()
{
    if (!m_tasksModel) {
        return;
    }

    const QModelIndex idx = m_tasksModel->activeTask();
    if (idx.isValid()) {
        m_tasksModel->requestActivate(idx);
    }
}

void MenuShortcutBridge::sendPendingShortcut()
{
    MenuShortcutSender::sendToActiveWindow(m_pendingSequence);
}

bool MenuShortcutSender::sendToActiveWindow(const QKeySequence &sequence)
{
    if (sequence.isEmpty()) {
        return false;
    }

    bool sent = false;
    for (int i = 0; i < sequence.count(); ++i) {
        sent = sendCombination(sequence[i]) || sent;
    }
    return sent;
}

#include "moc_menushortcutsender.cpp"
