/****************************************************************************
** Meta object code from reading C++ file 'dbusmenu_interface.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "dbusmenu_interface.h"
#include <QtCore/qmetatype.h>
#include <QtCore/QList>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'dbusmenu_interface.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN17DBusMenuInterfaceE_t {};
} // unnamed namespace

template <> constexpr inline auto DBusMenuInterface::qt_create_metaobjectdata<qt_meta_tag_ZN17DBusMenuInterfaceE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "DBusMenuInterface",
        "ItemActivationRequested",
        "",
        "id",
        "timeStamp",
        "ItemsPropertiesUpdated",
        "DBusMenuItemList",
        "in0",
        "DBusMenuItemKeysList",
        "in1",
        "LayoutUpdated",
        "revision",
        "parentId",
        "AboutToShow",
        "QDBusPendingReply<bool>",
        "Event",
        "Q_NOREPLY",
        "eventId",
        "QDBusVariant",
        "data",
        "timestamp",
        "GetGroupProperties",
        "QDBusPendingReply<DBusMenuItemList>",
        "QList<int>",
        "ids",
        "propertyNames",
        "GetLayout",
        "QDBusPendingReply<uint,DBusMenuLayoutItem>",
        "recursionDepth",
        "QDBusReply<uint>",
        "DBusMenuLayoutItem&",
        "item",
        "GetProperty",
        "QDBusPendingReply<QDBusVariant>",
        "property",
        "Status",
        "Version"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'ItemActivationRequested'
        QtMocHelpers::SignalData<void(int, uint)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 3 }, { QMetaType::UInt, 4 },
        }}),
        // Signal 'ItemsPropertiesUpdated'
        QtMocHelpers::SignalData<void(DBusMenuItemList, DBusMenuItemKeysList)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 6, 7 }, { 0x80000000 | 8, 9 },
        }}),
        // Signal 'LayoutUpdated'
        QtMocHelpers::SignalData<void(uint, int)>(10, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::UInt, 11 }, { QMetaType::Int, 12 },
        }}),
        // Slot 'AboutToShow'
        QtMocHelpers::SlotData<QDBusPendingReply<bool>(int)>(13, 2, QMC::AccessPublic, 0x80000000 | 14, {{
            { QMetaType::Int, 3 },
        }}),
        // Slot 'Event'
        QtMocHelpers::SlotData<void(int, const QString &, const QDBusVariant &, uint)>(15, 16, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 3 }, { QMetaType::QString, 17 }, { 0x80000000 | 18, 19 }, { QMetaType::UInt, 20 },
        }}),
        // Slot 'GetGroupProperties'
        QtMocHelpers::SlotData<QDBusPendingReply<DBusMenuItemList>(const QList<int> &, const QStringList &)>(21, 2, QMC::AccessPublic, 0x80000000 | 22, {{
            { 0x80000000 | 23, 24 }, { QMetaType::QStringList, 25 },
        }}),
        // Slot 'GetLayout'
        QtMocHelpers::SlotData<QDBusPendingReply<uint,DBusMenuLayoutItem>(int, int, const QStringList &)>(26, 2, QMC::AccessPublic, 0x80000000 | 27, {{
            { QMetaType::Int, 12 }, { QMetaType::Int, 28 }, { QMetaType::QStringList, 25 },
        }}),
        // Slot 'GetLayout'
        QtMocHelpers::SlotData<QDBusReply<uint>(int, int, const QStringList &, DBusMenuLayoutItem &)>(26, 2, QMC::AccessPublic, 0x80000000 | 29, {{
            { QMetaType::Int, 12 }, { QMetaType::Int, 28 }, { QMetaType::QStringList, 25 }, { 0x80000000 | 30, 31 },
        }}),
        // Slot 'GetProperty'
        QtMocHelpers::SlotData<QDBusPendingReply<QDBusVariant>(int, const QString &)>(32, 2, QMC::AccessPublic, 0x80000000 | 33, {{
            { QMetaType::Int, 3 }, { QMetaType::QString, 34 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
        // property 'Status'
        QtMocHelpers::PropertyData<QString>(35, QMetaType::QString, QMC::DefaultPropertyFlags),
        // property 'Version'
        QtMocHelpers::PropertyData<uint>(36, QMetaType::UInt, QMC::DefaultPropertyFlags),
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<DBusMenuInterface, qt_meta_tag_ZN17DBusMenuInterfaceE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject DBusMenuInterface::staticMetaObject = { {
    QMetaObject::SuperData::link<QDBusAbstractInterface::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN17DBusMenuInterfaceE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN17DBusMenuInterfaceE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN17DBusMenuInterfaceE_t>.metaTypes,
    nullptr
} };

void DBusMenuInterface::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<DBusMenuInterface *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->ItemActivationRequested((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<uint>>(_a[2]))); break;
        case 1: _t->ItemsPropertiesUpdated((*reinterpret_cast<std::add_pointer_t<DBusMenuItemList>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<DBusMenuItemKeysList>>(_a[2]))); break;
        case 2: _t->LayoutUpdated((*reinterpret_cast<std::add_pointer_t<uint>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2]))); break;
        case 3: { QDBusPendingReply<bool> _r = _t->AboutToShow((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QDBusPendingReply<bool>*>(_a[0]) = std::move(_r); }  break;
        case 4: _t->Event((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QDBusVariant>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<uint>>(_a[4]))); break;
        case 5: { QDBusPendingReply<DBusMenuItemList> _r = _t->GetGroupProperties((*reinterpret_cast<std::add_pointer_t<QList<int>>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QStringList>>(_a[2])));
            if (_a[0]) *reinterpret_cast<QDBusPendingReply<DBusMenuItemList>*>(_a[0]) = std::move(_r); }  break;
        case 6: { QDBusPendingReply<uint,DBusMenuLayoutItem> _r = _t->GetLayout((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QStringList>>(_a[3])));
            if (_a[0]) *reinterpret_cast<QDBusPendingReply<uint,DBusMenuLayoutItem>*>(_a[0]) = std::move(_r); }  break;
        case 7: { QDBusReply<uint> _r = _t->GetLayout((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QStringList>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<DBusMenuLayoutItem&>>(_a[4])));
            if (_a[0]) *reinterpret_cast<QDBusReply<uint>*>(_a[0]) = std::move(_r); }  break;
        case 8: { QDBusPendingReply<QDBusVariant> _r = _t->GetProperty((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])));
            if (_a[0]) *reinterpret_cast<QDBusPendingReply<QDBusVariant>*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 5:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QList<int> >(); break;
            }
            break;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (DBusMenuInterface::*)(int , uint )>(_a, &DBusMenuInterface::ItemActivationRequested, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (DBusMenuInterface::*)(DBusMenuItemList , DBusMenuItemKeysList )>(_a, &DBusMenuInterface::ItemsPropertiesUpdated, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (DBusMenuInterface::*)(uint , int )>(_a, &DBusMenuInterface::LayoutUpdated, 2))
            return;
    }
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast<QString*>(_v) = _t->status(); break;
        case 1: *reinterpret_cast<uint*>(_v) = _t->version(); break;
        default: break;
        }
    }
}

const QMetaObject *DBusMenuInterface::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DBusMenuInterface::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN17DBusMenuInterfaceE_t>.strings))
        return static_cast<void*>(this);
    return QDBusAbstractInterface::qt_metacast(_clname);
}

int DBusMenuInterface::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDBusAbstractInterface::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    }
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void DBusMenuInterface::ItemActivationRequested(int _t1, uint _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1, _t2);
}

// SIGNAL 1
void DBusMenuInterface::ItemsPropertiesUpdated(DBusMenuItemList _t1, DBusMenuItemKeysList _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1, _t2);
}

// SIGNAL 2
void DBusMenuInterface::LayoutUpdated(uint _t1, int _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1, _t2);
}
QT_WARNING_POP
