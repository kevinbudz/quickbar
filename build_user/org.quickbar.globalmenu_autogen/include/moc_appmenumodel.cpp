/****************************************************************************
** Meta object code from reading C++ file 'appmenumodel.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/appmenumodel.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'appmenumodel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.11.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN12AppMenuModelE_t {};
} // unnamed namespace

template <> constexpr inline auto AppMenuModel::qt_create_metaobjectdata<qt_meta_tag_ZN12AppMenuModelE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "AppMenuModel",
        "QML.Element",
        "auto",
        "requestActivateIndex",
        "",
        "index",
        "bringToFocus",
        "requestOpenAbout",
        "allScreensChanged",
        "menuAvailableChanged",
        "menuForDisplayChanged",
        "stickyMenuBarChanged",
        "showDesktopMenuChanged",
        "enableGenericMenuChanged",
        "enableMenuSearchChanged",
        "applicationNameChanged",
        "applicationIconChanged",
        "excludedItemsRegexChanged",
        "modelNeedsUpdate",
        "containmentStatusChanged",
        "screenGeometryChanged",
        "visibleChanged",
        "onActiveWindowChanged",
        "setVisible",
        "visible",
        "update",
        "menuAvailable",
        "menuForDisplay",
        "stickyMenuBar",
        "showDesktopMenu",
        "enableGenericMenu",
        "enableMenuSearch",
        "applicationName",
        "applicationIcon",
        "QVariant",
        "excludedItemsRegex",
        "allScreens",
        "containmentStatus",
        "Plasma::Types::ItemStatus",
        "screenGeometry",
        "QRect"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'requestActivateIndex'
        QtMocHelpers::SignalData<void(int)>(3, 4, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 5 },
        }}),
        // Signal 'bringToFocus'
        QtMocHelpers::SignalData<void(int)>(6, 4, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 5 },
        }}),
        // Signal 'requestOpenAbout'
        QtMocHelpers::SignalData<void()>(7, 4, QMC::AccessPublic, QMetaType::Void),
        // Signal 'allScreensChanged'
        QtMocHelpers::SignalData<void()>(8, 4, QMC::AccessPublic, QMetaType::Void),
        // Signal 'menuAvailableChanged'
        QtMocHelpers::SignalData<void()>(9, 4, QMC::AccessPublic, QMetaType::Void),
        // Signal 'menuForDisplayChanged'
        QtMocHelpers::SignalData<void()>(10, 4, QMC::AccessPublic, QMetaType::Void),
        // Signal 'stickyMenuBarChanged'
        QtMocHelpers::SignalData<void()>(11, 4, QMC::AccessPublic, QMetaType::Void),
        // Signal 'showDesktopMenuChanged'
        QtMocHelpers::SignalData<void()>(12, 4, QMC::AccessPublic, QMetaType::Void),
        // Signal 'enableGenericMenuChanged'
        QtMocHelpers::SignalData<void()>(13, 4, QMC::AccessPublic, QMetaType::Void),
        // Signal 'enableMenuSearchChanged'
        QtMocHelpers::SignalData<void()>(14, 4, QMC::AccessPublic, QMetaType::Void),
        // Signal 'applicationNameChanged'
        QtMocHelpers::SignalData<void()>(15, 4, QMC::AccessPublic, QMetaType::Void),
        // Signal 'applicationIconChanged'
        QtMocHelpers::SignalData<void()>(16, 4, QMC::AccessPublic, QMetaType::Void),
        // Signal 'excludedItemsRegexChanged'
        QtMocHelpers::SignalData<void()>(17, 4, QMC::AccessPublic, QMetaType::Void),
        // Signal 'modelNeedsUpdate'
        QtMocHelpers::SignalData<void()>(18, 4, QMC::AccessPublic, QMetaType::Void),
        // Signal 'containmentStatusChanged'
        QtMocHelpers::SignalData<void()>(19, 4, QMC::AccessPublic, QMetaType::Void),
        // Signal 'screenGeometryChanged'
        QtMocHelpers::SignalData<void()>(20, 4, QMC::AccessPublic, QMetaType::Void),
        // Signal 'visibleChanged'
        QtMocHelpers::SignalData<void()>(21, 4, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onActiveWindowChanged'
        QtMocHelpers::SlotData<void()>(22, 4, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'setVisible'
        QtMocHelpers::SlotData<void(bool)>(23, 4, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Bool, 24 },
        }}),
        // Slot 'update'
        QtMocHelpers::SlotData<void()>(25, 4, QMC::AccessPrivate, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
        // property 'menuAvailable'
        QtMocHelpers::PropertyData<bool>(26, QMetaType::Bool, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 4),
        // property 'visible'
        QtMocHelpers::PropertyData<bool>(24, QMetaType::Bool, QMC::DefaultPropertyFlags, 16),
        // property 'menuForDisplay'
        QtMocHelpers::PropertyData<bool>(27, QMetaType::Bool, QMC::DefaultPropertyFlags, 5),
        // property 'stickyMenuBar'
        QtMocHelpers::PropertyData<bool>(28, QMetaType::Bool, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 6),
        // property 'showDesktopMenu'
        QtMocHelpers::PropertyData<bool>(29, QMetaType::Bool, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 7),
        // property 'enableGenericMenu'
        QtMocHelpers::PropertyData<bool>(30, QMetaType::Bool, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 8),
        // property 'enableMenuSearch'
        QtMocHelpers::PropertyData<bool>(31, QMetaType::Bool, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 9),
        // property 'applicationName'
        QtMocHelpers::PropertyData<QString>(32, QMetaType::QString, QMC::DefaultPropertyFlags, 10),
        // property 'applicationIcon'
        QtMocHelpers::PropertyData<QVariant>(33, 0x80000000 | 34, QMC::DefaultPropertyFlags | QMC::EnumOrFlag, 11),
        // property 'excludedItemsRegex'
        QtMocHelpers::PropertyData<QString>(35, QMetaType::QString, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 12),
        // property 'allScreens'
        QtMocHelpers::PropertyData<bool>(36, QMetaType::Bool, QMC::DefaultPropertyFlags | QMC::Writable, 3),
        // property 'containmentStatus'
        QtMocHelpers::PropertyData<Plasma::Types::ItemStatus>(37, 0x80000000 | 38, QMC::DefaultPropertyFlags | QMC::Writable | QMC::EnumOrFlag, 14),
        // property 'screenGeometry'
        QtMocHelpers::PropertyData<QRect>(39, 0x80000000 | 40, QMC::DefaultPropertyFlags | QMC::Writable | QMC::EnumOrFlag | QMC::StdCppSet, 15),
    };
    QtMocHelpers::UintData qt_enums {
    };
    QtMocHelpers::UintData qt_constructors {};
    QtMocHelpers::ClassInfos qt_classinfo({
            {    1,    2 },
    });
    return QtMocHelpers::metaObjectData<AppMenuModel, void>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums, qt_constructors, qt_classinfo);
}
Q_CONSTINIT static const QMetaObject::SuperData qt_meta_extradata_ZN12AppMenuModelE[] = {
    QMetaObject::SuperData::link<Plasma::Types::staticMetaObject>(),
    nullptr
};

Q_CONSTINIT const QMetaObject AppMenuModel::staticMetaObject = { {
    QMetaObject::SuperData::link<QAbstractListModel::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12AppMenuModelE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12AppMenuModelE_t>.data,
    qt_static_metacall,
    qt_meta_extradata_ZN12AppMenuModelE,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN12AppMenuModelE_t>.metaTypes,
    nullptr
} };

void AppMenuModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<AppMenuModel *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->requestActivateIndex((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 1: _t->bringToFocus((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 2: _t->requestOpenAbout(); break;
        case 3: _t->allScreensChanged(); break;
        case 4: _t->menuAvailableChanged(); break;
        case 5: _t->menuForDisplayChanged(); break;
        case 6: _t->stickyMenuBarChanged(); break;
        case 7: _t->showDesktopMenuChanged(); break;
        case 8: _t->enableGenericMenuChanged(); break;
        case 9: _t->enableMenuSearchChanged(); break;
        case 10: _t->applicationNameChanged(); break;
        case 11: _t->applicationIconChanged(); break;
        case 12: _t->excludedItemsRegexChanged(); break;
        case 13: _t->modelNeedsUpdate(); break;
        case 14: _t->containmentStatusChanged(); break;
        case 15: _t->screenGeometryChanged(); break;
        case 16: _t->visibleChanged(); break;
        case 17: _t->onActiveWindowChanged(); break;
        case 18: _t->setVisible((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 19: _t->update(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (AppMenuModel::*)(int )>(_a, &AppMenuModel::requestActivateIndex, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (AppMenuModel::*)(int )>(_a, &AppMenuModel::bringToFocus, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (AppMenuModel::*)()>(_a, &AppMenuModel::requestOpenAbout, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (AppMenuModel::*)()>(_a, &AppMenuModel::allScreensChanged, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (AppMenuModel::*)()>(_a, &AppMenuModel::menuAvailableChanged, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (AppMenuModel::*)()>(_a, &AppMenuModel::menuForDisplayChanged, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (AppMenuModel::*)()>(_a, &AppMenuModel::stickyMenuBarChanged, 6))
            return;
        if (QtMocHelpers::indexOfMethod<void (AppMenuModel::*)()>(_a, &AppMenuModel::showDesktopMenuChanged, 7))
            return;
        if (QtMocHelpers::indexOfMethod<void (AppMenuModel::*)()>(_a, &AppMenuModel::enableGenericMenuChanged, 8))
            return;
        if (QtMocHelpers::indexOfMethod<void (AppMenuModel::*)()>(_a, &AppMenuModel::enableMenuSearchChanged, 9))
            return;
        if (QtMocHelpers::indexOfMethod<void (AppMenuModel::*)()>(_a, &AppMenuModel::applicationNameChanged, 10))
            return;
        if (QtMocHelpers::indexOfMethod<void (AppMenuModel::*)()>(_a, &AppMenuModel::applicationIconChanged, 11))
            return;
        if (QtMocHelpers::indexOfMethod<void (AppMenuModel::*)()>(_a, &AppMenuModel::excludedItemsRegexChanged, 12))
            return;
        if (QtMocHelpers::indexOfMethod<void (AppMenuModel::*)()>(_a, &AppMenuModel::modelNeedsUpdate, 13))
            return;
        if (QtMocHelpers::indexOfMethod<void (AppMenuModel::*)()>(_a, &AppMenuModel::containmentStatusChanged, 14))
            return;
        if (QtMocHelpers::indexOfMethod<void (AppMenuModel::*)()>(_a, &AppMenuModel::screenGeometryChanged, 15))
            return;
        if (QtMocHelpers::indexOfMethod<void (AppMenuModel::*)()>(_a, &AppMenuModel::visibleChanged, 16))
            return;
    }
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast<bool*>(_v) = _t->menuAvailable(); break;
        case 1: *reinterpret_cast<bool*>(_v) = _t->visible(); break;
        case 2: *reinterpret_cast<bool*>(_v) = _t->menuForDisplay(); break;
        case 3: *reinterpret_cast<bool*>(_v) = _t->stickyMenuBar(); break;
        case 4: *reinterpret_cast<bool*>(_v) = _t->showDesktopMenu(); break;
        case 5: *reinterpret_cast<bool*>(_v) = _t->enableGenericMenu(); break;
        case 6: *reinterpret_cast<bool*>(_v) = _t->enableMenuSearch(); break;
        case 7: *reinterpret_cast<QString*>(_v) = _t->applicationName(); break;
        case 8: *reinterpret_cast<QVariant*>(_v) = _t->applicationIcon(); break;
        case 9: *reinterpret_cast<QString*>(_v) = _t->excludedItemsRegex(); break;
        case 10: *reinterpret_cast<bool*>(_v) = _t->allScreens(); break;
        case 11: *reinterpret_cast<Plasma::Types::ItemStatus*>(_v) = _t->m_containmentStatus; break;
        case 12: *reinterpret_cast<QRect*>(_v) = _t->screenGeometry(); break;
        default: break;
        }
    }
    if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setMenuAvailable(*reinterpret_cast<bool*>(_v)); break;
        case 3: _t->setStickyMenuBar(*reinterpret_cast<bool*>(_v)); break;
        case 4: _t->setShowDesktopMenu(*reinterpret_cast<bool*>(_v)); break;
        case 5: _t->setEnableGenericMenu(*reinterpret_cast<bool*>(_v)); break;
        case 6: _t->setEnableMenuSearch(*reinterpret_cast<bool*>(_v)); break;
        case 9: _t->setExcludedItemsRegex(*reinterpret_cast<QString*>(_v)); break;
        case 10: _t->setallScreens(*reinterpret_cast<bool*>(_v)); break;
        case 11:
            if (QtMocHelpers::setProperty(_t->m_containmentStatus, *reinterpret_cast<Plasma::Types::ItemStatus*>(_v)))
                Q_EMIT _t->containmentStatusChanged();
            break;
        case 12: _t->setScreenGeometry(*reinterpret_cast<QRect*>(_v)); break;
        default: break;
        }
    }
}

const QMetaObject *AppMenuModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *AppMenuModel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12AppMenuModelE_t>.strings))
        return static_cast<void*>(this);
    return QAbstractListModel::qt_metacast(_clname);
}

int AppMenuModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractListModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 20)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 20;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 20)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 20;
    }
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    }
    return _id;
}

// SIGNAL 0
void AppMenuModel::requestActivateIndex(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void AppMenuModel::bringToFocus(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void AppMenuModel::requestOpenAbout()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void AppMenuModel::allScreensChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void AppMenuModel::menuAvailableChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void AppMenuModel::menuForDisplayChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void AppMenuModel::stickyMenuBarChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}

// SIGNAL 7
void AppMenuModel::showDesktopMenuChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 7, nullptr);
}

// SIGNAL 8
void AppMenuModel::enableGenericMenuChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 8, nullptr);
}

// SIGNAL 9
void AppMenuModel::enableMenuSearchChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 9, nullptr);
}

// SIGNAL 10
void AppMenuModel::applicationNameChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 10, nullptr);
}

// SIGNAL 11
void AppMenuModel::applicationIconChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 11, nullptr);
}

// SIGNAL 12
void AppMenuModel::excludedItemsRegexChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 12, nullptr);
}

// SIGNAL 13
void AppMenuModel::modelNeedsUpdate()
{
    QMetaObject::activate(this, &staticMetaObject, 13, nullptr);
}

// SIGNAL 14
void AppMenuModel::containmentStatusChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 14, nullptr);
}

// SIGNAL 15
void AppMenuModel::screenGeometryChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 15, nullptr);
}

// SIGNAL 16
void AppMenuModel::visibleChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 16, nullptr);
}
QT_WARNING_POP
