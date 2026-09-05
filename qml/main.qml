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

    fullRepresentation: Item {
        id: fullRoot
        implicitWidth: menuScroller.implicitWidth
        implicitHeight: menuScroller.implicitHeight

        readonly property bool showEmptyPreview: inPanelConfigure && buttonRepeater.count === 0
        readonly property bool effectiveVisible: root.barVisible
        readonly property int configureMinWidth: Math.max(
            Kirigami.Units.gridUnit * 6,
            noMenuPlaceholder.implicitWidth + Kirigami.Units.smallSpacing * 2
        )

        Plasmoid.status: {
            if (!effectiveVisible) {
                return PlasmaCore.Types.HiddenStatus
            }
            if (appMenuModel.menuAvailable && Plasmoid.currentIndex > -1 && buttonRepeater.count > 0) {
                return PlasmaCore.Types.NeedsAttentionStatus
            }
            if (buttonRepeater.count > 0 || showEmptyPreview
                    || (showApplicationName && appMenuModel.applicationName.length > 0)) {
                return PlasmaCore.Types.ActiveStatus
            }
            return PlasmaCore.Types.PassiveStatus
        }

        Layout.minimumWidth: fillWidth ? -1 : Math.max(implicitWidth, showEmptyPreview ? configureMinWidth : 0)
        Layout.minimumHeight: implicitHeight
        Layout.fillWidth: fillWidth
        Layout.fillHeight: true
        Layout.preferredWidth: fillWidth ? -1 : Math.max(implicitWidth, showEmptyPreview ? configureMinWidth : 0)

        Flickable {
            id: menuScroller
            anchors.fill: parent
            clip: true
            contentWidth: buttonGrid.implicitWidth
            contentHeight: buttonGrid.implicitHeight
            interactive: contentWidth > width || contentHeight > height
            flickableDirection: root.vertical ? Flickable.VerticalFlick : Flickable.HorizontalFlick
            // Cap the viewport when maxVisibleItems is set so the applet does not
            // overflow into neighbouring widgets. All buttons stay instantiated -
            // the excess is reachable by scrolling.
            implicitWidth: {
                const fullWidth = Math.max(buttonGrid.implicitWidth, showEmptyPreview ? configureMinWidth : 0)
                if (root.vertical || maxVisibleItems <= 0) {
                    return fullWidth
                }
                const cap = buttonGrid.maxItemWidth * maxVisibleItems + Kirigami.Units.smallSpacing * 2
                return Math.min(fullWidth, cap)
            }
            implicitHeight: {
                const fullHeight = buttonGrid.implicitHeight
                if (!root.vertical || maxVisibleItems <= 0) {
                    return fullHeight
                }
                const cap = buttonGrid.maxItemHeight * maxVisibleItems + Kirigami.Units.smallSpacing * 2
                return Math.min(fullHeight, cap)
            }

            // Let a vertical wheel scroll a horizontal bar.
            WheelHandler {
                acceptedDevices: PointerDevice.Mouse | PointerDevice.TouchPad
                onWheel: {
                    if (root.vertical) {
                        return
                    }
                    if (menuScroller.contentWidth <= menuScroller.width) {
                        return
                    }
                    let delta = 0
                    if (event.pixelDelta.x !== 0 || event.pixelDelta.y !== 0) {
                        delta = event.pixelDelta.x !== 0 ? event.pixelDelta.x : event.pixelDelta.y
                    } else {
                        delta = event.angleDelta.x !== 0 ? event.angleDelta.x : event.angleDelta.y
                    }
                    if (delta !== 0) {
                        menuScroller.contentX = Math.max(0, Math.min(menuScroller.contentX - delta, menuScroller.contentWidth - menuScroller.width))
                        event.accepted = true
                    }
                }
            }

            GridLayout {
                id: buttonGrid
                width: implicitWidth
                height: implicitHeight

                readonly property int maxItemWidth: {
                    let w = Kirigami.Units.gridUnit * 4
                    if (appNameLabel.visible) {
                        w = Math.max(w, appNameLabel.implicitWidth)
                    }
                    if (fullAppIcon.visible) {
                        w = Math.max(w, fullAppIcon.implicitWidth)
                    }
                    for (let i = 0; i < buttonRepeater.count; ++i) {
                        const item = buttonRepeater.itemAt(i)
                        if (item) {
                            w = Math.max(w, item.implicitWidth)
                        }
                    }
                    return w
                }
                readonly property int maxItemHeight: {
                    let h = Kirigami.Units.gridUnit * 2
                    if (appNameLabel.visible) {
                        h = Math.max(h, appNameLabel.implicitHeight)
                    }
                    if (noMenuPlaceholder.visible) {
                        h = Math.max(h, noMenuPlaceholder.implicitHeight)
                    }
                    for (let i = 0; i < buttonRepeater.count; ++i) {
                        const item = buttonRepeater.itemAt(i)
                        if (item) {
                            h = Math.max(h, item.implicitHeight)
                        }
                    }
                    return h
                }

                LayoutMirroring.enabled: Qt.application.layoutDirection === Qt.RightToLeft
                flow: root.vertical ? GridLayout.TopToBottom : GridLayout.LeftToRight
                rowSpacing: root.vertical ? itemSpacing : 0
                columnSpacing: root.vertical ? 0 : itemSpacing

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
                    if (maxVisibleItems > 0) {
                        if (!root.vertical) {
                            if (button.x < menuScroller.contentX) {
                                menuScroller.contentX = button.x
                            } else if (button.x + button.width > menuScroller.contentX + menuScroller.width) {
                                menuScroller.contentX = button.x + button.width - menuScroller.width
                            }
                        } else {
                            if (button.y < menuScroller.contentY) {
                                menuScroller.contentY = button.y
                            } else if (button.y + button.height > menuScroller.contentY + menuScroller.height) {
                                menuScroller.contentY = button.y + button.height - menuScroller.height
                            }
                        }
                    }
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
            visible: showEmptyPreview
            enabled: false
            text: Plasmoid.title
            display: PlasmaComponents3.AbstractButton.TextOnly
            Layout.alignment: Qt.AlignVCenter
            Layout.fillWidth: root.vertical
            Layout.fillHeight: !root.vertical
            Layout.preferredWidth: inPanelConfigure ? configureMinWidth : implicitWidth
        }

        Repeater {
            id: buttonRepeater
            model: appMenuModel.menuAvailable ? appMenuModel : null

            MenuDelegate {
                required property int index
                required property string activeMenu
                required property PlasmaCore.Action activeActions
                readonly property int buttonIndex: index

                Layout.alignment: Qt.AlignVCenter
                Layout.fillWidth: root.vertical
                Layout.fillHeight: !root.vertical
                text: activeMenu
                Kirigami.MnemonicData.active: altState.pressed

                down: Plasmoid.currentIndex === index
                visible: text !== "" && (activeActions?.visible ?? false)

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
            Layout.fillWidth: fillWidth
            Layout.fillHeight: true
            Layout.preferredWidth: fillWidth ? 0 : 0
            Layout.preferredHeight: 0
        }
            }
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
