/****************************************************************************
** Generated QML type registration code
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <QtQml/qqml.h>
#include <QtQml/qqmlmoduleregistration.h>

#if __has_include(<appmenumodel.h>)
#  include <appmenumodel.h>
#endif


#if !defined(QT_STATIC)
#define Q_QMLTYPE_EXPORT Q_DECL_EXPORT
#else
#define Q_QMLTYPE_EXPORT
#endif
Q_QMLTYPE_EXPORT void qml_register_types_plasma_applet_org_quickbar_globalmenu()
{
    qmlRegisterModule("plasma.applet.org.quickbar.globalmenu", 254, 0);
    QT_WARNING_PUSH QT_WARNING_DISABLE_DEPRECATED
    qmlRegisterTypesAndRevisions<AppMenuModel>("plasma.applet.org.quickbar.globalmenu", 254);
    QMetaType::fromType<QAbstractItemModel *>().id();
    qmlRegisterEnum<QAbstractItemModel::LayoutChangeHint>("QAbstractItemModel::LayoutChangeHint");
    qmlRegisterEnum<QAbstractItemModel::CheckIndexOption>("QAbstractItemModel::CheckIndexOption");
    QMetaType::fromType<QAbstractListModel *>().id();
    QT_WARNING_POP
    qmlRegisterModule("plasma.applet.org.quickbar.globalmenu", 254, 254);
}

static const QQmlModuleRegistration plasmaappletorgquickbarglobalmenuRegistration("plasma.applet.org.quickbar.globalmenu", qml_register_types_plasma_applet_org_quickbar_globalmenu);
