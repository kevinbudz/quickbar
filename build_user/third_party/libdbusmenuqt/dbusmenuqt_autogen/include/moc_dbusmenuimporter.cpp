/****************************************************************************
** Meta object code from reading C++ file 'dbusmenuimporter.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../third_party/libdbusmenuqt/dbusmenuimporter.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'dbusmenuimporter.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN16DBusMenuImporterE_t {};
} // unnamed namespace

template <> constexpr inline auto DBusMenuImporter::qt_create_metaobjectdata<qt_meta_tag_ZN16DBusMenuImporterE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "DBusMenuImporter",
        "menuUpdated",
        "",
        "QMenu*",
        "actionActivationRequested",
        "QAction*",
        "updateMenu",
        "menu",
        "slotMenuAboutToShow",
        "slotMenuAboutToHide",
        "slotAboutToShowDBusCallFinished",
        "QDBusPendingCallWatcher*",
        "slotItemActivationRequested",
        "id",
        "timestamp",
        "processPendingLayoutUpdates",
        "slotLayoutUpdated",
        "revision",
        "parentId",
        "slotGetLayoutFinished",
        "slotItemsPropertiesUpdated",
        "DBusMenuItemList",
        "updatedList",
        "DBusMenuItemKeysList",
        "removedList"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'menuUpdated'
        QtMocHelpers::SignalData<void(QMenu *)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 2 },
        }}),
        // Signal 'actionActivationRequested'
        QtMocHelpers::SignalData<void(QAction *)>(4, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 5, 2 },
        }}),
        // Slot 'updateMenu'
        QtMocHelpers::SlotData<void()>(6, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'updateMenu'
        QtMocHelpers::SlotData<void(QMenu *)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 7 },
        }}),
        // Slot 'slotMenuAboutToShow'
        QtMocHelpers::SlotData<void()>(8, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'slotMenuAboutToHide'
        QtMocHelpers::SlotData<void()>(9, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'slotAboutToShowDBusCallFinished'
        QtMocHelpers::SlotData<void(QDBusPendingCallWatcher *)>(10, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 11, 2 },
        }}),
        // Slot 'slotItemActivationRequested'
        QtMocHelpers::SlotData<void(int, uint)>(12, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 13 }, { QMetaType::UInt, 14 },
        }}),
        // Slot 'processPendingLayoutUpdates'
        QtMocHelpers::SlotData<void()>(15, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'slotLayoutUpdated'
        QtMocHelpers::SlotData<void(uint, int)>(16, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::UInt, 17 }, { QMetaType::Int, 18 },
        }}),
        // Slot 'slotGetLayoutFinished'
        QtMocHelpers::SlotData<void(QDBusPendingCallWatcher *)>(19, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 11, 2 },
        }}),
        // Slot 'slotItemsPropertiesUpdated'
        QtMocHelpers::SlotData<void(const DBusMenuItemList &, const DBusMenuItemKeysList &)>(20, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 21, 22 }, { 0x80000000 | 23, 24 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<DBusMenuImporter, qt_meta_tag_ZN16DBusMenuImporterE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject DBusMenuImporter::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN16DBusMenuImporterE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN16DBusMenuImporterE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN16DBusMenuImporterE_t>.metaTypes,
    nullptr
} };

void DBusMenuImporter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<DBusMenuImporter *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->menuUpdated((*reinterpret_cast<std::add_pointer_t<QMenu*>>(_a[1]))); break;
        case 1: _t->actionActivationRequested((*reinterpret_cast<std::add_pointer_t<QAction*>>(_a[1]))); break;
        case 2: _t->updateMenu(); break;
        case 3: _t->updateMenu((*reinterpret_cast<std::add_pointer_t<QMenu*>>(_a[1]))); break;
        case 4: _t->slotMenuAboutToShow(); break;
        case 5: _t->slotMenuAboutToHide(); break;
        case 6: _t->slotAboutToShowDBusCallFinished((*reinterpret_cast<std::add_pointer_t<QDBusPendingCallWatcher*>>(_a[1]))); break;
        case 7: _t->slotItemActivationRequested((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<uint>>(_a[2]))); break;
        case 8: _t->processPendingLayoutUpdates(); break;
        case 9: _t->slotLayoutUpdated((*reinterpret_cast<std::add_pointer_t<uint>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2]))); break;
        case 10: _t->slotGetLayoutFinished((*reinterpret_cast<std::add_pointer_t<QDBusPendingCallWatcher*>>(_a[1]))); break;
        case 11: _t->d->slotItemsPropertiesUpdated((*reinterpret_cast<std::add_pointer_t<DBusMenuItemList>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<DBusMenuItemKeysList>>(_a[2]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (DBusMenuImporter::*)(QMenu * )>(_a, &DBusMenuImporter::menuUpdated, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (DBusMenuImporter::*)(QAction * )>(_a, &DBusMenuImporter::actionActivationRequested, 1))
            return;
    }
}

const QMetaObject *DBusMenuImporter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DBusMenuImporter::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN16DBusMenuImporterE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int DBusMenuImporter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 12)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 12;
    }
    return _id;
}

// SIGNAL 0
void DBusMenuImporter::menuUpdated(QMenu * _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void DBusMenuImporter::actionActivationRequested(QAction * _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}
QT_WARNING_POP
