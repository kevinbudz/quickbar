/*
    QuickBar — configurable global menu for Plasma 6+
    Based on KDE Plasma Global Menu (GPL-2.0-or-later)
*/
pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import QtQml

import org.kde.plasma.plasmoid
import org.kde.plasma.core as PlasmaCore
import org.kde.plasma.private.keyboardindicator as KeyboardIndicator
import org.kde.plasma.components as PlasmaComponents3
import org.kde.kirigami as Kirigami
import plasma.applet.org.quickbar.globalmenu

PlasmoidItem {
    id: root

    readonly property bool vertical: Plasmoid.formFactor === PlasmaCore.Types.Vertical
    readonly property bool compact: Plasmoid.configuration.compactView
    readonly property bool fillWidth: Plasmoid.configuration.fillWidth
    readonly property bool hideWhenEmpty: Plasmoid.configuration.hideWhenEmpty
    readonly property bool showApplicationName: Plasmoid.configuration.showApplicationName
    readonly property bool showApplicationIcon: Plasmoid.configuration.showApplicationIcon
    readonly property int appNameMarginBefore: Plasmoid.configuration.appNameMarginBefore
    readonly property int appNameMarginAfter: Plasmoid.configuration.appNameMarginAfter
    readonly property bool stickyMenuBar: Plasmoid.configuration.stickyMenuBar
    readonly property bool showDesktopMenu: Plasmoid.configuration.showDesktopMenu
    readonly property int maxVisibleItems: Plasmoid.configuration.maxVisibleItems
    readonly property int itemSpacing: Plasmoid.configuration.itemSpacing
    readonly property bool inPanelConfigure: Plasmoid.userConfiguring
        || (Plasmoid.containment?.corona?.editMode ?? false)

    readonly property bool barVisible: {
        if (inPanelConfigure) {
            return true
        }
        if (!appMenuModel.menuAvailable && hideWhenEmpty) {
            // Keep the bar visible for the app name (including "Plasma" on desktop)
            if (showApplicationName && appMenuModel.applicationName.length > 0) {
                return true
            }
            return false
        }
        if (Plasmoid.configuration.filterByActive && !appMenuModel.visible && !stickyMenuBar) {
            return false
        }
        return true
    }

    onCompactChanged: Plasmoid.view = compact

    Component.onCompleted: Plasmoid.view = compact

    Plasmoid.constraintHints: Plasmoid.CanFillArea
    preferredRepresentation: compact ? compactRepresentation : fullRepresentation

    compactRepresentation: RowLayout {
        id: compactRoot
        spacing: Kirigami.Units.smallSpacing

        readonly property bool showAppName: root.showApplicationName
            && appMenuModel.applicationName.length > 0
            && (root.barVisible || root.inPanelConfigure)
        readonly property bool showAppIcon: root.showApplicationIcon
            && appMenuModel.applicationIcon !== undefined
            && appMenuModel.applicationIcon !== null
            && appMenuModel.applicationIcon !== ""

        Kirigami.Icon {
            id: compactAppIcon
            visible: compactRoot.showAppName && compactRoot.showAppIcon
            source: appMenuModel.applicationIcon
            Layout.alignment: Qt.AlignVCenter
            implicitWidth: Kirigami.Units.iconSizes.small
            implicitHeight: Kirigami.Units.iconSizes.small
            Layout.leftMargin: root.vertical ? 0 : root.appNameMarginBefore
            Layout.topMargin: root.vertical ? root.appNameMarginBefore : 0
        }

        AppNameLabel {
            id: compactAppName
            visible: compactRoot.showAppName
            text: appMenuModel.applicationName
            fontSize: Plasmoid.configuration.appNameFontSize
            fontFamily: Plasmoid.configuration.appNameFontFamily
            fontWeight: Plasmoid.configuration.appNameFontWeight
            Layout.alignment: Qt.AlignVCenter
            Layout.leftMargin: root.vertical ? 0 : (compactAppIcon.visible ? Kirigami.Units.smallSpacing : root.appNameMarginBefore)
            Layout.rightMargin: root.vertical ? 0 : root.appNameMarginAfter
            Layout.topMargin: root.vertical ? (compactAppIcon.visible ? Kirigami.Units.smallSpacing : root.appNameMarginBefore) : 0
            Layout.bottomMargin: root.vertical ? root.appNameMarginAfter : 0
        }

        PlasmaComponents3.ToolButton {
            id: compactMenuButton
            readonly property int fakeIndex: 0
            Layout.alignment: Qt.AlignVCenter
            Layout.fillWidth: false
            Layout.fillHeight: false
            Layout.minimumWidth: implicitWidth
            Layout.maximumWidth: implicitWidth
            enabled: appMenuModel.menuAvailable || root.inPanelConfigure
            checkable: appMenuModel.menuAvailable && Plasmoid.currentIndex === fakeIndex
            checked: checkable
            icon.name: "application-menu"

            display: PlasmaComponents3.AbstractButton.IconOnly
            text: Plasmoid.title
            Accessible.description: root.toolTipSubText

            onClicked: Plasmoid.trigger(this, 0)
        }
    }

    fullRepresentation: GridLayout {
        id: buttonGrid

        Plasmoid.status: {
            if (!root.barVisible) {
                return PlasmaCore.Types.HiddenStatus
            }
            if (appMenuModel.menuAvailable && Plasmoid.currentIndex > -1 && buttonRepeater.count > 0) {
                return PlasmaCore.Types.NeedsAttentionStatus
            }
            if (buttonRepeater.count > 0 || Plasmoid.configuration.compactView
                    || (root.showApplicationName && appMenuModel.applicationName.length > 0)) {
                return PlasmaCore.Types.ActiveStatus
            }
            return PlasmaCore.Types.HiddenStatus
        }

        LayoutMirroring.enabled: Application.layoutDirection === Qt.RightToLeft
        Layout.minimumWidth: implicitWidth
        Layout.minimumHeight: implicitHeight
        Layout.fillWidth: Plasmoid.configuration.fillWidth
        Layout.fillHeight: true

        flow: root.vertical ? GridLayout.TopToBottom : GridLayout.LeftToRight
        rowSpacing: root.vertical ? Plasmoid.configuration.itemSpacing : 0
        columnSpacing: root.vertical ? 0 : Plasmoid.configuration.itemSpacing

        Binding {
            target: Plasmoid
            property: "buttonGrid"
            value: buttonGrid
            restoreMode: Binding.RestoreNone
        }

        Connections {
            target: Plasmoid
            function onRequestActivateIndex(index: int) {
                const button = buttonRepeater.itemAt(index) as MenuDelegate
                if (button) {
                    button.activated()
                }
            }
        }

        Connections {
            target: Plasmoid
            function onActivated() {
                const button = buttonRepeater.itemAt(0) as MenuDelegate
                if (button) {
                    button.activated()
                }
            }
        }

        Kirigami.Icon {
            id: fullAppIcon
            visible: root.showApplicationName && root.showApplicationIcon
                && appMenuModel.applicationIcon !== undefined
                && appMenuModel.applicationIcon !== null
                && appMenuModel.applicationIcon !== ""
                && appMenuModel.applicationName.length > 0
                && (root.barVisible || root.inPanelConfigure)
            source: appMenuModel.applicationIcon
            Layout.alignment: Qt.AlignVCenter
            implicitWidth: Kirigami.Units.iconSizes.small
            implicitHeight: Kirigami.Units.iconSizes.small
            Layout.leftMargin: root.vertical ? 0 : root.appNameMarginBefore
            Layout.topMargin: root.vertical ? root.appNameMarginBefore : 0
        }

        AppNameLabel {
            id: appNameLabel
            visible: root.showApplicationName && appMenuModel.applicationName.length > 0
                && (root.barVisible || root.inPanelConfigure)
            text: appMenuModel.applicationName
            fontSize: Plasmoid.configuration.appNameFontSize
            fontFamily: Plasmoid.configuration.appNameFontFamily
            fontWeight: Plasmoid.configuration.appNameFontWeight
            Layout.alignment: Qt.AlignVCenter
            Layout.leftMargin: root.vertical ? 0 : (fullAppIcon.visible ? Kirigami.Units.smallSpacing : root.appNameMarginBefore)
            Layout.rightMargin: root.vertical ? 0 : root.appNameMarginAfter
            Layout.topMargin: root.vertical ? (fullAppIcon.visible ? Kirigami.Units.smallSpacing : root.appNameMarginBefore) : 0
            Layout.bottomMargin: root.vertical ? root.appNameMarginAfter : 0
        }

        PlasmaComponents3.ToolButton {
            id: noMenuPlaceholder
            visible: buttonRepeater.count === 0 && (root.inPanelConfigure || (!root.showApplicationName && !root.hideWhenEmpty))
            text: Plasmoid.title
            Layout.fillWidth: root.vertical
            Layout.fillHeight: !root.vertical
        }

        Repeater {
            id: buttonRepeater
            model: appMenuModel.visible ? appMenuModel : null

            MenuDelegate {
                required property int index
                required property string activeMenu
                required property PlasmaCore.Action activeActions
                readonly property int buttonIndex: index

                Layout.fillWidth: root.vertical
                Layout.fillHeight: !root.vertical
                text: activeMenu
                Kirigami.MnemonicData.active: altState.pressed

                down: Plasmoid.currentIndex === index
                visible: (Plasmoid.configuration.maxVisibleItems <= 0 || index < Plasmoid.configuration.maxVisibleItems)
                    && text !== "" && (activeActions?.visible ?? false)

                menuIsOpen: Plasmoid.currentIndex !== -1
                hoverOpensMenu: Plasmoid.configuration.hoverOpensMenu
                hoverCornerRadius: Plasmoid.configuration.hoverCornerRadius
                fontSize: Plasmoid.configuration.fontSize
                fontFamily: Plasmoid.configuration.fontFamily
                fontWeight: Plasmoid.configuration.fontWeight
                textColor: Plasmoid.configuration.textColor
                hoverTextColor: Plasmoid.configuration.hoverTextColor

                onActivated: Plasmoid.trigger(this, index)

                KeyboardIndicator.KeyState {
                    id: altState
                    key: Qt.Key_Alt
                }
            }
        }

        Item {
            Layout.preferredWidth: 0
            Layout.preferredHeight: 0
            Layout.fillWidth: Plasmoid.configuration.fillWidth
            Layout.fillHeight: true
        }
    }

    AppMenuModel {
        id: appMenuModel
        containmentStatus: Plasmoid.containment.status
        screenGeometry: root.screenGeometry
        allScreens: Plasmoid.configuration.allScreens
        stickyMenuBar: root.stickyMenuBar
        showDesktopMenu: root.showDesktopMenu
        enableGenericMenu: Plasmoid.configuration.enableGenericMenu
        enableMenuSearch: Plasmoid.configuration.enableMenuSearch
        excludedItemsRegex: Plasmoid.configuration.excludedItemsRegex
        onRequestActivateIndex: Plasmoid.requestActivateIndex(index)
        Component.onCompleted: {
            Plasmoid.model = appMenuModel
            Plasmoid.hoverOpensMenu = Plasmoid.configuration.hoverOpensMenu
        }
    }

    property bool pendingOpenAbout: false

    function findAppletConfiguration() {
        let parent = root.parent
        while (parent) {
            if (typeof parent.open === "function" && parent.configDialog !== undefined) {
                return parent
            }
            parent = parent.parent
        }
        return null
    }

    function openAboutSettingsPage() {
        const appletConfig = findAppletConfiguration()
        if (!appletConfig) {
            return
        }
        appletConfig.open({
            name: i18nc("@title:window About this widget", "About"),
            source: Qt.resolvedUrl("configAbout.qml"),
        })
        root.pendingOpenAbout = false
    }

    Connections {
        target: appMenuModel
        function onRequestOpenAbout() {
            root.pendingOpenAbout = true
            const configure = Plasmoid.internalAction("configure")
            if (configure) {
                configure.trigger()
            }
        }
    }

    Connections {
        target: Plasmoid
        function onUserConfiguringChanged(configuring) {
            if (configuring && root.pendingOpenAbout) {
                Qt.callLater(openAboutSettingsPage)
            } else if (!configuring) {
                root.pendingOpenAbout = false
            }
        }
    }

    Connections {
        target: Plasmoid.configuration
        function onHoverOpensMenuChanged() {
            Plasmoid.hoverOpensMenu = Plasmoid.configuration.hoverOpensMenu
        }
        function onShowDesktopMenuChanged() {
            appMenuModel.showDesktopMenu = Plasmoid.configuration.showDesktopMenu
        }
        function onEnableGenericMenuChanged() {
            appMenuModel.enableGenericMenu = Plasmoid.configuration.enableGenericMenu
        }
        function onEnableMenuSearchChanged() {
            appMenuModel.enableMenuSearch = Plasmoid.configuration.enableMenuSearch
        }
        function onExcludedItemsRegexChanged() {
            appMenuModel.excludedItemsRegex = Plasmoid.configuration.excludedItemsRegex
        }
    }
}
