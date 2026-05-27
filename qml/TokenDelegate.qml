import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import AuthenDesk

ItemDelegate {
    id: delegateRoot
    width: ListView.view ? ListView.view.width : 400
    height: 100

    Rectangle {
        anchors.fill: parent
        color: "#FFFFFF"
    }

    RowLayout {
        anchors.fill: parent
        spacing: 0

        Rectangle {
            Layout.preferredWidth: 44
            Layout.preferredHeight: 44
            Layout.alignment: Qt.AlignTop
            Layout.leftMargin: 14
            Layout.topMargin: 14
            radius: 22

            Image {
                anchors.fill: parent
                anchors.margins: 4
                source: iconPath || ""
                visible: iconPath !== ""
                fillMode: Image.PreserveAspectFit
            }

            color: iconPath ? "transparent" : (badgeColor || "#3B82F6")

            Text {
                anchors.centerIn: parent
                text: iconPath ? "" : (name ? name.substring(0, 2).toUpperCase() : "??")
                color: "white"
                font.pixelSize: 15
                font.bold: true
            }
        }

        ColumnLayout {
            Layout.fillWidth: true
            Layout.leftMargin: 12
            Layout.topMargin: 10
            Layout.bottomMargin: 10
            spacing: 2

            Text {
                Layout.fillWidth: true
                text: issuer || ""
                color: "#0F172A"
                font.pixelSize: 14
                font.bold: true
                maximumLineCount: 1
                elide: Text.ElideRight
            }
            Text {
                Layout.fillWidth: true
                text: account || name || ""
                color: "#64748B"
                font.pixelSize: 12
                maximumLineCount: 1
                elide: Text.ElideRight
            }
            RowLayout {
                Layout.fillWidth: true
                spacing: 8

                Text {
                    Layout.alignment: Qt.AlignBottom
                    text: code || "------"
                    color: progress * period <= 5 ? "#DC2626" : "#0F172A"
                    font.pixelSize: 26
                    font.bold: true
                    font.family: "monospace"
                    font.letterSpacing: 5
                }
                Text {
                    Layout.alignment: Qt.AlignBottom
                    text: nextCode || ""
                    color: "#94A3B8"
                    font.pixelSize: 16
                    font.family: "monospace"
                    font.letterSpacing: 3
                    maximumLineCount: 1
                    elide: Text.ElideRight
                }
            }
        }

        Canvas {
            id: countdownCanvas
            Layout.preferredWidth: 40
            Layout.preferredHeight: 40
            Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
            Layout.rightMargin: 14

            Component.onCompleted: requestPaint()

            Timer {
                interval: 500
                running: true
                repeat: true
                onTriggered: countdownCanvas.requestPaint()
            }

            onPaint: {
                var ctx = getContext("2d")
                var w = width
                var h = height
                var cx = w / 2
                var cy = h / 2
                var r = w / 2 - 3
                ctx.clearRect(0, 0, w, h)

                ctx.beginPath()
                ctx.arc(cx, cy, r, 0, Math.PI * 2)
                ctx.lineWidth = 3
                ctx.strokeStyle = "#E2E8F0"
                ctx.stroke()

                var t = progress
                if (t > 0.001) {
                    var startAngle = -Math.PI / 2
                    var endAngle = startAngle + t * Math.PI * 2
                    ctx.beginPath()
                    ctx.arc(cx, cy, r, startAngle, endAngle)
                    ctx.lineWidth = 3
                    ctx.lineCap = "round"
                    ctx.strokeStyle = t > 0.25 ? (t > 0.5 ? "#16A34A" : "#D97706") : "#DC2626"
                    ctx.stroke()
                }

                ctx.font = "bold 13px sans-serif"
                ctx.fillStyle = "#0F172A"
                ctx.textAlign = "center"
                ctx.textBaseline = "middle"
                ctx.fillText(Math.ceil(t * period), cx, cy)
            }
        }
    }

    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        onClicked: function(mouse) {
            if (mouse.button === Qt.LeftButton) {
                tokenModel.copyToClipboard(index)
                copyTooltip.visible = true
                copyTooltipTimer.restart()
            } else if (mouse.button === Qt.RightButton) {
                actionSheet.show()
            }
        }
        onPressAndHold: {
            actionSheet.show()
        }
    }

    Rectangle {
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        height: 1
        color: "#F1F5F9"
    }

    Rectangle {
        id: copyTooltip
        visible: false
        anchors.centerIn: parent
        width: 120
        height: 40
        radius: 8
        color: "#16A34A"
        z: 100

        Text {
            anchors.centerIn: parent
            text: Strings.copied
            color: "white"
            font.pixelSize: 14
            font.bold: true
        }
    }

    Timer {
        id: copyTooltipTimer
        interval: 1500
        onTriggered: copyTooltip.visible = false
    }

    Popup {
        id: actionSheet
        parent: Overlay.overlay
        width: parent.width - 32
        x: 16
        y: parent.height
        padding: 0
        modal: true
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

        function show() {
            open()
        }

        onOpened: {
            slideIn.from = y
            slideIn.to = parent.height - height - 10
            slideIn.start()
        }

        NumberAnimation {
            id: slideIn
            target: actionSheet
            property: "y"
            duration: 600
            easing.type: Easing.OutCubic
        }

        NumberAnimation {
            id: slideOut
            target: actionSheet
            property: "y"
            duration: 400
            easing.type: Easing.InCubic
            onFinished: actionSheet.close()
        }

        function hide() {
            slideOut.to = actionSheet.parent ? actionSheet.parent.height : 0
            slideOut.start()
        }

        background: Rectangle {
            id: sheetBg
            color: "#FFFFFF"
            radius: 16
            border.color: "#E2E8F0"
        }

        contentItem: ColumnLayout {
            spacing: 0
            width: parent.width

            Rectangle {
                Layout.preferredWidth: 36
                Layout.preferredHeight: 5
                Layout.alignment: Qt.AlignHCenter
                Layout.topMargin: 12
                Layout.bottomMargin: 8
                radius: 2.5
                color: "#E2E8F0"
            }

            ColumnLayout {
                Layout.fillWidth: true
                Layout.leftMargin: 20
                Layout.rightMargin: 20
                spacing: 4

                Text {
                    text: issuer || ""
                    color: "#0F172A"
                    font.pixelSize: 18
                    font.bold: true
                }
                Text {
                    text: (account || name) || ""
                    color: "#64748B"
                    font.pixelSize: 14
                }
                Text {
                    text: code || "------"
                    color: "#0F172A"
                    font.pixelSize: 28
                    font.bold: true
                    font.family: "monospace"
                    font.letterSpacing: 4
                }
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 1
                Layout.topMargin: 16
                Layout.leftMargin: 20
                Layout.rightMargin: 20
                color: "#F1F5F9"
            }

            Button {
                Layout.fillWidth: true
                Layout.preferredHeight: 50
                flat: true
                contentItem: Text {
                    text: Strings.edit
                    color: "#0F172A"
                    font.pixelSize: 16
                    verticalAlignment: Text.AlignVCenter
                    leftPadding: 20
                }
                background: Rectangle { color: "transparent" }

                onClicked: {
                    actionSheet.hide()
                    Window.window.navigateToEdit(index)
                }
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 1
                Layout.leftMargin: 20
                Layout.rightMargin: 20
                color: "#F1F5F9"
            }

            Button {
                Layout.fillWidth: true
                Layout.preferredHeight: 50
                flat: true
                contentItem: Text {
                    text: Strings.copy
                    color: "#0F172A"
                    font.pixelSize: 16
                    verticalAlignment: Text.AlignVCenter
                    leftPadding: 20
                }
                background: Rectangle { color: "transparent" }

                onClicked: {
                    actionSheet.hide()
                    tokenModel.copyTokenJsonToClipboard(index)
                }
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 1
                Layout.leftMargin: 20
                Layout.rightMargin: 20
                color: "#F1F5F9"
            }

            Button {
                Layout.fillWidth: true
                Layout.preferredHeight: 50
                flat: true
                contentItem: Text {
                    text: Strings.deleteToken
                    color: "#DC2626"
                    font.pixelSize: 16
                    verticalAlignment: Text.AlignVCenter
                    leftPadding: 20
                }
                background: Rectangle { color: "transparent" }

                onClicked: {
                    actionSheet.hide()
                    tokenModel.remove(index)
                }
            }

            Item { Layout.preferredHeight: 12 }
        }
    }
}
