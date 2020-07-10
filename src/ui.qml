import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Universal 2.12

import Qt.labs.platform 1.1

import io.qt.examples.backend 1.0
import Theme 1.0
import "Components"


Window {
    visible: true
    width: 360
    height: 200
    x: wnd.screen.width - wnd.width
    y: wnd.screen.height - wnd.height - 40

    color: Qt.rgba(0, 0, 0, 0.8)

    title: qsTr("Shlyapp")
    id:wnd
    flags: 
        Qt.WA_TranslucentBackground |
        Qt.FramelessWindowHint | 
        Qt.MSWindowsFixedSizeDialogHint | 
        Qt.WindowStaysOnTopHint |
        Qt.Tool |
        Qt.Popup

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 0

        RowLayout {
            Layout.alignment: Qt.AlignRight
            spacing:10
            Appbutton {
                src:"qrc:/images/collapse.png"
                onClicked: wnd.showMinimized()
                }
            Appbutton {
                src:"qrc:/images/close.png"
                onClicked: {
                        trIcon.hide()
                        wnd.close()
                    }
                }
        }

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 40

            Label {
                Layout.fillWidth: true
                text: "Enable Dimming"
                color: Universal.foreground
                font.pointSize: 21
                font.weight: Font.DemiBold
            }

            RowLayout{
                Layout.fillWidth: true
                Switch {
                    id:enabledSwitch
                    width: 20
                    checked: BackEnd.enabled
                    onCheckedChanged: BackEnd.enabled = checked
                }
                Label {
                    Layout.fillWidth: true
                    text: enabledSwitch.checked ? "On" : "Off"
                    font.pointSize: 16
                    font.weight: Font.DemiBold
                }
            }

            RowLayout{
                Layout.fillWidth: true
                Slider {
                        from: 0
                        to: 100
                        value: BackEnd.opacity
                        onMoved: BackEnd.opacity = value
                        stepSize: 1
                }
                Label {
                    Layout.fillWidth: true
                    text: BackEnd.opacity + "%"
                    font.pointSize: 16
                    font.weight: Font.DemiBold
                }
            }
        } 
    }
    
    SystemTrayIcon {
        visible: true
        id:trIcon
        icon.source: "qrc:/images/monitor_dimmer_icon.ico"

        onActivated: {
            wnd.show()
            flick.restart()
            wnd.raise()
            wnd.requestActivate()
        }
    }

    NumberAnimation on y {
        id: flick
        from: wnd.screen.height - wnd.height + 200
        to: wnd.screen.height - wnd.height - 40
        duration: 500
        easing {type: Easing.OutQuint;}
    }

    onActiveChanged: {
            if (!active) {
                wnd.visible = false;
            }
        }

    // states: [
    //     State{ name: "VISIBLE"
    //     PropertyChanges {target: wnd; y:100}
    //     }
    //     State{ name: "INVISIBLE"
    //     PropertyChanges {target: wnd; y:200}
    //     }
    // ]

    // transitions: [
    //     Transition {
    //         from: "VISIBLE"
    //         to: "INVISIBLE"
    //         NumberAnimation {
    //             id: visibleanimation
    //             easing {
    //                 type: Easing.OutElastic
    //                 amplitude: 1.0
    //                 period: 0.5
    //             }
    //         }
    //     }
    // ]
}
