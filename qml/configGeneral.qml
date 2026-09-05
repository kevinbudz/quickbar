import QtQuick
import QtQuick.Controls
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import Qt.labs.platform as Platform

import org.kde.kcmutils as KCMUtils
import org.kde.kirigami as Kirigami

KCMUtils.SimpleKCM {
    id: generalPage

    // Defaults from main.xml (used by Plasma for reset / change highlighting)
    readonly property bool cfg_compactViewDefault: false
    readonly property bool cfg_allScreensDefault: true
    readonly property bool cfg_fillWidthDefault: false
    readonly property bool cfg_hideWhenEmptyDefault: false
    readonly property bool cfg_showApplicationNameDefault: false
    readonly property bool cfg_showApplicationIconDefault: false
    readonly property int cfg_appNameMarginBeforeDefault: 0
    readonly property int cfg_appNameMarginAfterDefault: 4
    readonly property int cfg_itemSpacingDefault: 8
    readonly property int cfg_hoverCornerRadiusDefault: 4
    readonly property int cfg_fontSizeDefault: 0
    readonly property string cfg_fontFamilyDefault: ""
    readonly property int cfg_fontWeightDefault: 0
    readonly property int cfg_appNameFontSizeDefault: 0
    readonly property string cfg_appNameFontFamilyDefault: ""
    readonly property int cfg_appNameFontWeightDefault: 700
    readonly property string cfg_textColorDefault: ""
    readonly property string cfg_hoverTextColorDefault: ""
    readonly property int cfg_maxVisibleItemsDefault: 0
    readonly property bool cfg_filterByActiveDefault: false
    readonly property bool cfg_stickyMenuBarDefault: true
    readonly property bool cfg_showDesktopMenuDefault: true
    readonly property bool cfg_hoverOpensMenuDefault: true
    readonly property bool cfg_enableMenuSearchDefault: true
    readonly property bool cfg_enableGenericMenuDefault: true
    readonly property string cfg_excludedItemsRegexDefault: ""

    property alias cfg_compactView: compactViewRadio.checked
    property alias cfg_allScreens: allScreensCheck.checked
    property alias cfg_fillWidth: fillWidthCheck.checked
    property alias cfg_hideWhenEmpty: hideWhenEmptyCheck.checked
    property alias cfg_filterByActive: filterByActiveCheck.checked
    property alias cfg_stickyMenuBar: stickyMenuBarCheck.checked
    property alias cfg_showDesktopMenu: showDesktopMenuCheck.checked
    property alias cfg_hoverOpensMenu: hoverOpensMenuCheck.checked
    property alias cfg_enableMenuSearch: enableMenuSearchCheck.checked
    property alias cfg_enableGenericMenu: enableGenericMenuCheck.checked
    property alias cfg_excludedItemsRegex: excludedItemsRegexField.text
    property alias cfg_showApplicationName: showApplicationNameCheck.checked
    property alias cfg_showApplicationIcon: showApplicationIconCheck.checked
    property alias cfg_itemSpacing: itemSpacingSpin.value
    property alias cfg_hoverCornerRadius: hoverCornerRadiusSpin.value
    property alias cfg_fontSize: menuFontSettings.pointSize
    property alias cfg_fontFamily: menuFontSettings.family
    property alias cfg_fontWeight: menuFontSettings.weight
    property alias cfg_textColor: textColorField.text
    property alias cfg_hoverTextColor: hoverTextColorField.text
    property alias cfg_maxVisibleItems: maxVisibleItemsSpin.value
    property alias cfg_appNameFontSize: appNameFontSettings.pointSize
    property alias cfg_appNameFontFamily: appNameFontSettings.family
    property alias cfg_appNameFontWeight: appNameFontSettings.weight
    property alias cfg_appNameMarginBefore: appNameMarginBeforeSpin.value
    property alias cfg_appNameMarginAfter: appNameMarginAfterSpin.value

    readonly property var visibilityPresetModel: [
        { value: "always", text: i18n("Always show the menu bar") },
        { value: "focused", text: i18n("Only when the app window is active") },
        { value: "hideEmpty", text: i18n("Hide when no application menu is available") },
    ]

    property bool _syncingPreset: false

    QtObject {
        id: menuFontSettings
        property string family: ""
        property int pointSize: 0
        property int weight: 0
    }

    QtObject {
        id: appNameFontSettings
        property string family: ""
        property int pointSize: 0
        property int weight: 700
    }

    function weightLabel(weight) {
        if (weight <= 0) {
            return i18n("default weight")
        }
        if (weight <= Font.Thin) {
            return i18n("thin")
        }
        if (weight <= Font.ExtraLight) {
            return i18n("extra light")
        }
        if (weight <= Font.Light) {
            return i18n("light")
        }
        if (weight <= Font.Normal) {
            return i18n("regular")
        }
        if (weight <= Font.Medium) {
            return i18n("medium")
        }
        if (weight <= Font.DemiBold) {
            return i18n("semi bold")
        }
        if (weight <= Font.Bold) {
            return i18n("bold")
        }
        if (weight <= Font.ExtraBold) {
            return i18n("extra bold")
        }
        return i18n("black")
    }

    function fontSummary(settings) {
        const family = settings.family || i18n("System default")
        const size = settings.pointSize > 0
            ? i18nc("@label font size in points", "%1 pt", settings.pointSize)
            : i18n("default size")
        const style = generalPage.weightLabel(settings.weight)
        return i18nc("@info selected font, %1 family, %2 size, %3 style", "%1, %2, %3", family, size, style)
    }

    function configFont(settings) {
        const themeFont = Kirigami.Theme.defaultFont
        return Qt.font({
            family: settings.family || themeFont.family,
            pointSize: settings.pointSize > 0 ? settings.pointSize : themeFont.pointSize,
            weight: settings.weight > 0 ? settings.weight : themeFont.weight,
        })
    }

    function applyFontFromDialog(settings, chosen) {
        settings.family = chosen.family
        settings.pointSize = chosen.pointSize > 0 ? Math.round(chosen.pointSize) : 0
        settings.weight = chosen.weight > 0 ? chosen.weight : 0
    }

    function openFontDialog(dialog, settings) {
        dialog.currentFont = configFont(settings)
        dialog.open()
    }

    function resetFont(settings, defaultWeight) {
        settings.family = ""
        settings.pointSize = 0
        settings.weight = defaultWeight
    }

    function detectVisibilityPreset() {
        const sticky = stickyMenuBarCheck.checked
        const filter = filterByActiveCheck.checked
        const hideEmpty = hideWhenEmptyCheck.checked

        if (sticky && !filter && !hideEmpty) {
            return "always"
        }
        if (!sticky && filter && !hideEmpty) {
            return "focused"
        }
        if (!sticky && !filter && hideEmpty) {
            return "hideEmpty"
        }
        return ""
    }

    function applyVisibilityPreset(preset) {
        _syncingPreset = true
        switch (preset) {
        case "always":
            stickyMenuBarCheck.checked = true
            filterByActiveCheck.checked = false
            hideWhenEmptyCheck.checked = false
            break
        case "focused":
            stickyMenuBarCheck.checked = false
            filterByActiveCheck.checked = true
            hideWhenEmptyCheck.checked = false
            break
        case "hideEmpty":
            stickyMenuBarCheck.checked = false
            filterByActiveCheck.checked = false
            hideWhenEmptyCheck.checked = true
            break
        }
        _syncingPreset = false
        visibilityPresetCombo.currentIndex = visibilityPresetCombo.indexOfValue(preset)
    }

    function syncVisibilityPresetCombo() {
        const preset = detectVisibilityPreset()
        const idx = visibilityPresetCombo.indexOfValue(preset)
        visibilityPresetCombo.currentIndex = idx >= 0 ? idx : 0
    }

    readonly property string menuFontSummary: fontSummary(menuFontSettings)
    readonly property string appNameFontSummary: fontSummary(appNameFontSettings)

    Component.onCompleted: syncVisibilityPresetCombo()

    Kirigami.FormLayout {
        Kirigami.InlineMessage {
            Layout.fillWidth: true
            Layout.columnSpan: 2
            type: Kirigami.MessageType.Information
            text: i18n("Remove Plasma’s stock Global Menu from this panel before using QuickBar. Both widgets register the same menu service and cannot run together.")
            actions: [
                Kirigami.Action {
                    text: i18nc("@action:button", "Learn more")
                    icon.name: "help-about"
                    onTriggered: Qt.openUrlExternally("https://github.com/kevin/quickbar#important-replace-stock-global-menu")
                },
            ]
        }

        ColumnLayout {
            Kirigami.FormData.label: " "
            Layout.fillWidth: true
            spacing: Kirigami.Units.largeSpacing

            ColumnLayout {
                Layout.fillWidth: true
                spacing: Kirigami.Units.smallSpacing

                Kirigami.Heading {
                    Layout.fillWidth: true
                    level: 4
                    type: Kirigami.Heading.Type.Primary
                    text: i18n("Menu bar")
                }

                Kirigami.FormLayout {
                    Layout.fillWidth: true

                    ColumnLayout {
                        Kirigami.FormData.label: i18n("Display:")
                        spacing: 0

                        QQC2.RadioButton {
                            id: fullViewRadio
                            text: i18n("Full menubar (File, Edit, …)")
                            checked: !compactViewRadio.checked
                            onToggled: if (checked) compactViewRadio.checked = false
                        }
                        QQC2.RadioButton {
                            id: compactViewRadio
                            text: i18n("Single application-menu button")
                            onToggled: if (checked) fullViewRadio.checked = false
                        }
                    }

                    Item {
                        Kirigami.FormData.label: " "
                        implicitHeight: Kirigami.Units.largeSpacing
                    }

                    QQC2.CheckBox {
                        id: fillWidthCheck
                        Kirigami.FormData.label: " "
                        text: i18n("Fill remaining panel width")
                    }

                    QQC2.CheckBox {
                        id: allScreensCheck
                        Kirigami.FormData.label: " "
                        text: i18n("Show menus for apps on all screens")
                    }

                    QQC2.CheckBox {
                        id: hoverOpensMenuCheck
                        Kirigami.FormData.label: " "
                        text: i18n("Open menus on hover while a menu is already open")
                    }

                    QQC2.CheckBox {
                        id: enableGenericMenuCheck
                        Kirigami.FormData.label: " "
                        text: i18n("Generic menu for apps without global menu support")
                    }

                    QQC2.TextField {
                        id: excludedItemsRegexField
                        Kirigami.FormData.label: i18n("Exclude items (regex):")
                        placeholderText: i18n("e.g. ^(Search|Help)$")
                        Layout.fillWidth: true
                    }
                }
            }

            Kirigami.Separator {
                Layout.fillWidth: true
            }

            ColumnLayout {
                Layout.fillWidth: true
                spacing: Kirigami.Units.smallSpacing

                Kirigami.Heading {
                    Layout.fillWidth: true
                    level: 4
                    type: Kirigami.Heading.Type.Primary
                    text: i18n("Visibility")
                }

                Kirigami.FormLayout {
                    Layout.fillWidth: true

                    QQC2.ComboBox {
                        id: visibilityPresetCombo
                        Kirigami.FormData.label: i18n("When to show:")
                        textRole: "text"
                        valueRole: "value"
                        model: generalPage.visibilityPresetModel
                        onActivated: generalPage.applyVisibilityPreset(currentValue)
                    }

                    QQC2.CheckBox {
                        id: showApplicationNameCheck
                        Kirigami.FormData.label: " "
                        text: i18n("Show application name before the menu")
                    }

                    QQC2.CheckBox {
                        id: showApplicationIconCheck
                        Kirigami.FormData.label: " "
                        text: i18n("Show application icon before the menu")
                        enabled: showApplicationNameCheck.checked
                    }

                    QQC2.CheckBox {
                        id: showDesktopMenuCheck
                        Kirigami.FormData.label: " "
                        text: i18n("Show menu bar on Desktop")
                    }

                    QQC2.CheckBox {
                        id: stickyMenuBarCheck
                        visible: false
                        onCheckedChanged: if (!_syncingPreset) generalPage.syncVisibilityPresetCombo()
                    }
                    QQC2.CheckBox {
                        id: filterByActiveCheck
                        visible: false
                        onCheckedChanged: if (!_syncingPreset) generalPage.syncVisibilityPresetCombo()
                    }
                    QQC2.CheckBox {
                        id: hideWhenEmptyCheck
                        visible: false
                        onCheckedChanged: if (!_syncingPreset) generalPage.syncVisibilityPresetCombo()
                    }

                    QQC2.CheckBox {
                        id: enableMenuSearchCheck
                        visible: false
                        checked: true
                    }
                }
            }

            Kirigami.Separator {
                Layout.fillWidth: true
            }

            ColumnLayout {
                Layout.fillWidth: true
                spacing: Kirigami.Units.smallSpacing

                Kirigami.Heading {
                    Layout.fillWidth: true
                    level: 4
                    type: Kirigami.Heading.Type.Primary
                    text: i18n("Button style")
                }

                Kirigami.FormLayout {
                    Layout.fillWidth: true

                    QQC2.SpinBox {
                        id: itemSpacingSpin
                        Kirigami.FormData.label: i18n("Spacing (px):")
                        from: 0
                        to: 32
                    }

                    QQC2.SpinBox {
                        id: hoverCornerRadiusSpin
                        Kirigami.FormData.label: i18n("Hover corner radius (px):")
                        from: 0
                        to: 32
                    }

                    QQC2.SpinBox {
                        id: maxVisibleItemsSpin
                        Kirigami.FormData.label: i18n("Max visible items:")
                        from: 0
                        to: 50
                    }
                }

                // NOTE: keep this helper OUT of the FormLayout above. FormLayout
                // flips to narrow mode (labels stacked above controls) when its
                // width drops below its widest field's unwrapped implicitWidth,
                // and this long wrapping label would single-handedly trip that
                // threshold — longer translations would too.
                QQC2.Label {
                    Layout.fillWidth: true
                    wrapMode: Text.WordWrap
                    opacity: 0.75
                    text: i18n("Show at most N top-level menus. Drag, use the mouse wheel, or drag the thin scrollbar to reach the rest. Set to 0 for no limit.")
                }
            }

            Kirigami.Separator {
                Layout.fillWidth: true
            }

            ColumnLayout {
                Layout.fillWidth: true
                spacing: Kirigami.Units.smallSpacing

                Kirigami.Heading {
                    Layout.fillWidth: true
                    level: 4
                    type: Kirigami.Heading.Type.Primary
                    text: i18n("Text")
                }

                Kirigami.FormLayout {
                    Layout.fillWidth: true

                    RowLayout {
                        Kirigami.FormData.label: i18n("Name margins (px):")
                        Kirigami.FormData.buddyFor: appNameMarginBeforeSpin
                        Layout.fillWidth: true
                        spacing: Kirigami.Units.largeSpacing
                        enabled: showApplicationNameCheck.checked

                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: Kirigami.Units.smallSpacing

                            QQC2.Label {
                                opacity: 0.8
                                text: i18n("Before:")
                            }

                            QQC2.SpinBox {
                                id: appNameMarginBeforeSpin
                                Layout.fillWidth: true
                                from: 0
                                to: 64
                            }
                        }

                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: Kirigami.Units.smallSpacing

                            QQC2.Label {
                                opacity: 0.8
                                text: i18n("After:")
                            }

                            QQC2.SpinBox {
                                id: appNameMarginAfterSpin
                                Layout.fillWidth: true
                                from: 0
                                to: 64
                            }
                        }
                    }

                    RowLayout {
                        Kirigami.FormData.label: i18n("Colors:")
                        Kirigami.FormData.buddyFor: textColorField
                        Layout.fillWidth: true
                        spacing: Kirigami.Units.largeSpacing

                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: Kirigami.Units.smallSpacing

                            QQC2.Label {
                                opacity: 0.8
                                text: i18n("Text:")
                            }

                            QQC2.TextField {
                                id: textColorField
                                Layout.fillWidth: true
                                placeholderText: i18n("#RRGGBB or empty")
                            }
                        }

                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: Kirigami.Units.smallSpacing

                            QQC2.Label {
                                opacity: 0.8
                                text: i18n("Hover:")
                            }

                            QQC2.TextField {
                                id: hoverTextColorField
                                Layout.fillWidth: true
                                placeholderText: i18n("#RRGGBB or empty")
                            }
                        }
                    }

                    Item {
                        Kirigami.FormData.label: " "
                        implicitHeight: Kirigami.Units.largeSpacing
                    }

                    RowLayout {
                        Kirigami.FormData.label: i18n("Application name font:")
                        spacing: Kirigami.Units.smallSpacing
                        enabled: showApplicationNameCheck.checked

                        QQC2.Label {
                            Layout.fillWidth: true
                            elide: Text.ElideRight
                            text: generalPage.appNameFontSummary
                        }

                        QQC2.Button {
                            text: i18nc("@action:button", "Choose Font…")
                            onClicked: generalPage.openFontDialog(appNameFontDialog, appNameFontSettings)
                        }

                        QQC2.ToolButton {
                            icon.name: "edit-clear"
                            display: AbstractButton.IconOnly
                            ToolTip.text: i18nc("@action:button", "Use system default font")
                            onClicked: generalPage.resetFont(appNameFontSettings, 700)
                        }
                    }

                    RowLayout {
                        Kirigami.FormData.label: i18n("Menu font:")
                        spacing: Kirigami.Units.smallSpacing

                        QQC2.Label {
                            Layout.fillWidth: true
                            elide: Text.ElideRight
                            text: generalPage.menuFontSummary
                        }

                        QQC2.Button {
                            text: i18nc("@action:button", "Choose Font…")
                            onClicked: generalPage.openFontDialog(menuFontDialog, menuFontSettings)
                        }

                        QQC2.ToolButton {
                            icon.name: "edit-clear"
                            display: AbstractButton.IconOnly
                            ToolTip.text: i18nc("@action:button", "Use system default font")
                            onClicked: generalPage.resetFont(menuFontSettings, 0)
                        }
                    }
                }
            }
        }
    }

    Platform.FontDialog {
        id: menuFontDialog

        title: i18nc("@title:window", "Choose Font")
        modality: Qt.WindowModal
        parentWindow: generalPage.Window.window

        onAccepted: generalPage.applyFontFromDialog(menuFontSettings, font)
    }

    Platform.FontDialog {
        id: appNameFontDialog

        title: i18nc("@title:window", "Choose Application Name Font")
        modality: Qt.WindowModal
        parentWindow: generalPage.Window.window

        onAccepted: generalPage.applyFontFromDialog(appNameFontSettings, font)
    }
}
