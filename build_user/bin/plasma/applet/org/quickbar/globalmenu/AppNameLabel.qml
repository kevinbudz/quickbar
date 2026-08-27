/*
    Application name label for QuickBar
*/
import QtQuick
import org.kde.plasma.components as PC3
import org.kde.kirigami as Kirigami

PC3.Label {
    id: root

    property int fontSize: 0
    property string fontFamily: ""
    property int fontWeight: 0

    padding: 0
    topPadding: 0
    bottomPadding: 0
    verticalAlignment: Text.AlignVCenter
    horizontalAlignment: Text.AlignLeft
    elide: Text.ElideRight
    font.family: fontFamily || Kirigami.Theme.defaultFont.family
    font.pointSize: fontSize > 0 ? fontSize : Kirigami.Theme.defaultFont.pointSize
    font.weight: fontWeight > 0 ? fontWeight : Kirigami.Theme.defaultFont.weight
    color: Kirigami.Theme.textColor
}
