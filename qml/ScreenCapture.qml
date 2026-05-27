import QtQuick
import QtQuick.Window
import QtQuick.Controls

Window {
    id: captureWindow
    flags: Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint
    color: "transparent"

    property int screenIndex: 0
    property int offsetX: 0
    property int offsetY: 0

    Component.onCompleted: {
        if (screenIndex < Qt.application.screens.length) {
            var s = Qt.application.screens[screenIndex]
            screen = s
            x = s.virtualX
            y = s.virtualY
            width = s.width
            height = s.height
            offsetX = s.virtualX
            offsetY = s.virtualY
            visibility = Window.FullScreen
        }
    }

    signal regionSelected(int x, int y, int w, int h)
    signal cancelled()

    Canvas {
        id: selectionCanvas
        anchors.fill: parent

            property real startX: 0
            property real startY: 0
            property real endX: 0
            property real endY: 0
            property bool drawing: false

            onPaint: {
                var ctx = getContext("2d")
                ctx.clearRect(0, 0, width, height)

                ctx.globalAlpha = 0.4
                ctx.fillStyle = "#000000"
                ctx.fillRect(0, 0, width, height)
                ctx.globalAlpha = 1.0

                if (!drawing || (startX === endX && startY === endY)) return

                var rx = Math.min(startX, endX)
                var ry = Math.min(startY, endY)
                var rw = Math.abs(endX - startX)
                var rh = Math.abs(endY - startY)

                ctx.globalCompositeOperation = "destination-out"
                ctx.fillStyle = "#000000"
                ctx.fillRect(rx, ry, rw, rh)

                ctx.globalCompositeOperation = "source-over"
                ctx.strokeStyle = "#2563EB"
                ctx.lineWidth = 2
                ctx.strokeRect(rx, ry, rw, rh)

                ctx.fillStyle = "#2563EB"
                ctx.fillRect(rx - 3, ry - 3, 6, 6)
                ctx.fillRect(rx + rw - 3, ry - 3, 6, 6)
                ctx.fillRect(rx - 3, ry + rh - 3, 6, 6)
                ctx.fillRect(rx + rw - 3, ry + rh - 3, 6, 6)
            }

            MouseArea {
                anchors.fill: parent
                acceptedButtons: Qt.LeftButton | Qt.RightButton
                onPressed: function(mouse) {
                    if (mouse.button === Qt.RightButton) {
                        captureWindow.cancelled()
                        captureWindow.close()
                        return
                    }
                    selectionCanvas.startX = mouse.x
                    selectionCanvas.startY = mouse.y
                    selectionCanvas.endX = mouse.x
                    selectionCanvas.endY = mouse.y
                    selectionCanvas.drawing = true
                    selectionCanvas.requestPaint()
                }
                onPositionChanged: function(mouse) {
                    selectionCanvas.endX = mouse.x
                    selectionCanvas.endY = mouse.y
                    selectionCanvas.requestPaint()
                }
                onReleased: function(mouse) {
                    selectionCanvas.endX = mouse.x
                    selectionCanvas.endY = mouse.y
                    selectionCanvas.drawing = false
                    selectionCanvas.requestPaint()

                    var x = Math.min(selectionCanvas.startX, selectionCanvas.endX)
                    var y = Math.min(selectionCanvas.startY, selectionCanvas.endY)
                    var w = Math.abs(selectionCanvas.endX - selectionCanvas.startX)
                    var h = Math.abs(selectionCanvas.endY - selectionCanvas.startY)

                    if (w > 10 && h > 10) {
                        // Convert to virtual desktop coordinates
                        captureWindow.regionSelected(x + offsetX, y + offsetY, w, h)
                    }
                    captureWindow.close()
                }
            }
        }

        Text {
            anchors.centerIn: parent
            text: selectionCanvas.drawing ? "" : "拖动鼠标选择二维码区域"
            color: "#FFFFFF"
            font.pixelSize: 18
            opacity: 0.8
        }

        Text {
            anchors.top: parent.top
            anchors.topMargin: 16
            anchors.right: parent.right
            anchors.rightMargin: 24
            text: "按 Esc 取消"
            color: "#94A3B8"
            font.pixelSize: 14
        }

    Shortcut {
        sequence: "Escape"
        onActivated: {
            captureWindow.cancelled()
            captureWindow.close()
        }
    }
}
