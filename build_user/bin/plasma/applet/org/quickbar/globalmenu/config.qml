import QtQuick

import org.kde.plasma.configuration 2.0

ConfigModel {
    ConfigCategory {
        name: i18n("General")
        icon: "configure"
        source: "configGeneral.qml"
    }
    ConfigCategory {
        name: i18nc("@title:window About this widget", "About")
        icon: "help-about"
        source: "configAbout.qml"
    }
}
