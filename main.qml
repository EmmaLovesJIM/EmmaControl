import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import io.qt.examples.backend 1.0

ApplicationWindow {
    id: applicationWindow
    visible: true
    width: 1280
    height: 770
    title: qsTr("Emma Control")

    BackEnd {
        id: backend
    }

    RowLayout {
        id: row
        anchors.fill: parent
        GridLayout {
            columns: 2
            rows: 2
            flow: GridLayout.TopToBottom
            Layout.margins: 20
            Layout.preferredWidth: 40

            Text {
                id: speedtext
                Layout.alignment: Qt.AlignHCenter
                width: 40
                text: speedSlider.value.toString() + " A"
                horizontalAlignment: Text.AlignHCenter
                font.pixelSize: 12
            }
            Slider {
                id: speedSlider
                Layout.alignment: Qt.AlignHCenter
                Layout.fillHeight: true
                width: 40
                orientation: Qt.Vertical
                padding: 6
                wheelEnabled: false
                stepSize: 1
                to: 18
                from: -25
                rotation: 0
                value: 0
                onValueChanged: backend.speed = value
            }
            RoundButton {
                id: nullSpeedRoundButton
                Layout.rowSpan: 2
                width: 40
                text: "0"
                Layout.alignment: Qt.AlignLeft
                Layout.bottomMargin: (speedSlider.height - speedtext.height) * -0.36
                highlighted: false
                onClicked: {
                    speedSlider.value = 0
                }
            }
        }

        ColumnLayout {
            Layout.alignment: Qt.AlignTop
            Layout.topMargin: 30

            Dial {
                id: speedDial
                Layout.alignment: Qt.AlignHCenter
                width: 200
                height: 200
                value: backend.currentSpeed
                to: 2000
                wheelEnabled: false
                hoverEnabled: false
                enabled: false

                Text {
                    id: text1
                    text: (backend.currentSpeed/100.0).toFixed(1) + " km/h"
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    anchors.fill: parent
                    font.pixelSize: 24
                }
            }
        }
        ColumnLayout {
            Layout.margins: 20

            RowLayout {
                Layout.alignment: Qt.AlignHCenter
                Layout.fillHeight: true

                CheckBox {
                    id: connectedCheckBox
                    text: qsTr("")
                    checked: backend.connected
                    enabled: false
                }

                Button {
                    id: connectButton
                    text: qsTr("Connect")
                    onClicked: {
                        backend.on_connectButton_clicked()
                    }
                }
            }

            SwitchDelegate {
                id: directionSwitchDelegate
                Layout.alignment: Qt.AlignHCenter
                text: qsTr("Direction")
                highlighted: false
                autoExclusive: false
                checked: false
                onCheckedChanged: {
                    backend.direction = checked
                }
            }

            SwitchDelegate {
                id: compressorSwitchDelegate
                Layout.alignment: Qt.AlignHCenter
                text: qsTr("Compressor")
                checked: false
                onCheckedChanged: {
                    backend.compressor = checked
                }
            }

            Button {
                id: audibleWarningButton
                Layout.alignment: Qt.AlignHCenter
                text: qsTr("Audible Warning")
                checkable: false
                checked: false
                highlighted: false
                onPressed: {
                    backend.horn = true
                }
                onReleased: {
                    backend.horn = false
                }
            }
            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
            }

            Button {
                id: stopButton
                Layout.preferredWidth: 249
                Layout.preferredHeight: 80
                Layout.alignment: Qt.AlignHCenter
                text: qsTr("EMERGENCY STOP")
                checked: false
                font.pointSize: 20
                highlighted: true
                checkable: true
                onClicked: {
                    //backend.on_connectButton_clicked()
                }
                background: Rectangle {
                    color: "#b60000"
                }
            }

        }
        ColumnLayout {
            Layout.alignment: Qt.AlignTop
            Layout.topMargin: 30

            Dial {
                id: airPressureDial
                Layout.alignment: Qt.AlignHCenter
                width: 200
                height: 200
                to: 1000
                value: backend.currentAirPressure
                hoverEnabled: false
                enabled: false
                wheelEnabled: false

                Text {
                    id: text2
                    text: (backend.currentAirPressure/100.0).toFixed(2) + " Bar"
                    verticalAlignment: Text.AlignVCenter
                    font.pixelSize: 24
                    horizontalAlignment: Text.AlignHCenter
                    anchors.fill: parent
                }
            }
            RowLayout {
                Layout.alignment: Qt.AlignHCenter
                Text {
                    text: "DG0:"
                    font.pointSize: 16
                    horizontalAlignment: Text.AlignLeft
                }
                Text {
                    text: (backend.currentAirPressureDG0/100.0).toFixed(2) + " Bar"
                    font.pointSize: 16
                    horizontalAlignment: Text.AlignRight
                }
            }
            RowLayout {
                Layout.alignment: Qt.AlignHCenter
                Text {
                    text: "DG1:"
                    font.pointSize: 16
                    horizontalAlignment: Text.AlignLeft
                }
                Text {
                    text: (backend.currentAirPressureDG1/100.0).toFixed(2) + " Bar"
                    font.pointSize: 16
                    horizontalAlignment: Text.AlignRight
                }
            }
        }
        ColumnLayout {
            Layout.margins: 20
            Layout.leftMargin: 40
            Layout.preferredWidth: 40
            Text {
                id: breakText
                Layout.alignment: Qt.AlignHCenter
                width: 40
                text: breakSlider.value.toString()
                font.pixelSize: 12
            }
            Slider {
                id: breakSlider
                Layout.alignment: Qt.AlignHCenter
                Layout.fillHeight: true
                width: 40
                orientation: Qt.Vertical
                rotation: 0
                from: 0
                stepSize: 1
                value: 0
                to: 4
                onValueChanged: {
                    backend.breakLevel = value
                    if (value > 0)
                        speedSlider.value = 0
                }
            }
        }


    }

}
