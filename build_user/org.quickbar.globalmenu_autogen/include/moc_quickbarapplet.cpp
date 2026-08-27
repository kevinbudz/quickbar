/****************************************************************************
** Meta object code from reading C++ file 'quickbarapplet.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/quickbarapplet.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'quickbarapplet.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN14QuickBarAppletE_t {};
} // unnamed namespace

template <> constexpr inline auto QuickBarApplet::qt_create_metaobjectdata<qt_meta_tag_ZN14QuickBarAppletE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "QuickBarApplet",
        "modelChanged",
        "",
        "viewChanged",
        "currentIndexChanged",
        "buttonGridChanged",
        "hoverOpensMenuChanged",
        "requestActivateIndex",
        "index",
        "trigger",
        "QQuickItem*",
        "ctx",
        "idx",
        "containment",
        "model",
        "QAbstractItemModel*",
        "view",
        "currentIndex",
        "buttonGrid",
        "hoverOpensMenu"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'modelChanged'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'viewChanged'
        QtMocHelpers::SignalData<void()>(3, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'currentIndexChanged'
        QtMocHelpers::SignalData<void()>(4, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'buttonGridChanged'
        QtMocHelpers::SignalData<void()>(5, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'hoverOpensMenuChanged'
        QtMocHelpers::SignalData<void()>(6, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'requestActivateIndex'
        QtMocHelpers::SignalData<void(int)>(7, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 8 },
        }}),
        // Slot 'trigger'
        QtMocHelpers::SlotData<void(QQuickItem *, int)>(9, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 10, 11 }, { QMetaType::Int, 12 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
        // property 'containment'
        QtMocHelpers::PropertyData<QObject*>(13, QMetaType::QObjectStar, QMC::DefaultPropertyFlags | QMC::Constant),
        // property 'model'
        QtMocHelpers::PropertyData<QAbstractItemModel*>(14, 0x80000000 | 15, QMC::DefaultPropertyFlags | QMC::Writable | QMC::EnumOrFlag | QMC::StdCppSet, 0),
        // property 'view'
        QtMocHelpers::PropertyData<int>(16, QMetaType::Int, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 1),
        // property 'currentIndex'
        QtMocHelpers::PropertyData<int>(17, QMetaType::Int, QMC::DefaultPropertyFlags, 2),
        // property 'buttonGrid'
        QtMocHelpers::PropertyData<QQuickItem*>(18, 0x80000000 | 10, QMC::DefaultPropertyFlags | QMC::Writable | QMC::EnumOrFlag | QMC::StdCppSet, 3),
        // property 'hoverOpensMenu'
        QtMocHelpers::PropertyData<bool>(19, QMetaType::Bool, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 4),
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<QuickBarApplet, qt_meta_tag_ZN14QuickBarAppletE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject QuickBarApplet::staticMetaObject = { {
    QMetaObject::SuperData::link<Plasma::Applet::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN14QuickBarAppletE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN14QuickBarAppletE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN14QuickBarAppletE_t>.metaTypes,
    nullptr
} };

void QuickBarApplet::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<QuickBarApplet *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->modelChanged(); break;
        case 1: _t->viewChanged(); break;
        case 2: _t->currentIndexChanged(); break;
        case 3: _t->buttonGridChanged(); break;
        case 4: _t->hoverOpensMenuChanged(); break;
        case 5: _t->requestActivateIndex((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 6: _t->trigger((*reinterpret_cast<std::add_pointer_t<QQuickItem*>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (QuickBarApplet::*)()>(_a, &QuickBarApplet::modelChanged, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (QuickBarApplet::*)()>(_a, &QuickBarApplet::viewChanged, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (QuickBarApplet::*)()>(_a, &QuickBarApplet::currentIndexChanged, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (QuickBarApplet::*)()>(_a, &QuickBarApplet::buttonGridChanged, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (QuickBarApplet::*)()>(_a, &QuickBarApplet::hoverOpensMenuChanged, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (QuickBarApplet::*)(int )>(_a, &QuickBarApplet::requestActivateIndex, 5))
            return;
    }
    if (_c == QMetaObject::RegisterPropertyMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 1:
            *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QAbstractItemModel* >(); break;
        }
    }
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast<QObject**>(_v) = _t->containment(); break;
        case 1: *reinterpret_cast<QAbstractItemModel**>(_v) = _t->model(); break;
        case 2: *reinterpret_cast<int*>(_v) = _t->view(); break;
        case 3: *reinterpret_cast<int*>(_v) = _t->currentIndex(); break;
        case 4: *reinterpret_cast<QQuickItem**>(_v) = _t->buttonGrid(); break;
        case 5: *reinterpret_cast<bool*>(_v) = _t->hoverOpensMenu(); break;
        default: break;
        }
    }
    if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 1: _t->setModel(*reinterpret_cast<QAbstractItemModel**>(_v)); break;
        case 2: _t->setView(*reinterpret_cast<int*>(_v)); break;
        case 4: _t->setButtonGrid(*reinterpret_cast<QQuickItem**>(_v)); break;
        case 5: _t->setHoverOpensMenu(*reinterpret_cast<bool*>(_v)); break;
        default: break;
        }
    }
}

const QMetaObject *QuickBarApplet::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QuickBarApplet::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN14QuickBarAppletE_t>.strings))
        return static_cast<void*>(this);
    return Plasma::Applet::qt_metacast(_clname);
}

int QuickBarApplet::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Plasma::Applet::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 7;
    }
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void QuickBarApplet::modelChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void QuickBarApplet::viewChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void QuickBarApplet::currentIndexChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void QuickBarApplet::buttonGridChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void QuickBarApplet::hoverOpensMenuChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void QuickBarApplet::requestActivateIndex(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 5, nullptr, _t1);
}
QT_WARNING_POP
