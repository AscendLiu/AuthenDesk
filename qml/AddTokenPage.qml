import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window
import QtQuick.Dialogs

Page {
    id: page
    background: Rectangle { color: "#F8FAFC" }

    property bool advancedExpanded: false
    property string entryMode: "manual"
    property string detectedIcon: ""
    property string qrFilePath: ""
    property bool adding: false
    property bool isCapturing: false
    property var captureWindows: []

    header: Rectangle {
        height: 60
        color: "#FFFFFF"
        z: 10

        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: 12
            anchors.rightMargin: 12

            Button {
                text: "< 返回"
                font.pixelSize: 16
                flat: true
                contentItem: Text {
                    text: parent.text
                    color: "#2563EB"
                    font: parent.font
                    verticalAlignment: Text.AlignVCenter
                }
                background: Rectangle { color: "transparent" }
                onClicked: Window.window.popPage()
            }

            Text {
                text: "添加令牌"
                color: "#0F172A"
                font.pixelSize: 20
                font.bold: true
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter
                Layout.alignment: Qt.AlignVCenter
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.topMargin: 12
        spacing: 0

        RowLayout {
            Layout.fillWidth: true
            Layout.leftMargin: 16
            Layout.rightMargin: 16
            spacing: 0

            Button {
                Layout.fillWidth: true
                Layout.preferredHeight: 38
                text: "手动输入"
                font.pixelSize: 15

                background: Rectangle {
                    color: entryMode === "manual" ? "#2563EB" : "#F1F5F9"
                    radius: 10
                }
                contentItem: Text {
                    text: parent.text
                    color: entryMode === "manual" ? "white" : "#64748B"
                    font: parent.font
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                onClicked: entryMode = "manual"
            }

            Item { Layout.preferredWidth: 8 }

            Button {
                Layout.fillWidth: true
                Layout.preferredHeight: 38
                text: "二维码导入"
                font.pixelSize: 15

                background: Rectangle {
                    color: entryMode === "qr" ? "#2563EB" : "#F1F5F9"
                    radius: 10
                }
                contentItem: Text {
                    text: parent.text
                    color: entryMode === "qr" ? "white" : "#64748B"
                    font: parent.font
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                onClicked: entryMode = "qr"
            }
        }

        ScrollView {
            id: scrollView
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.topMargin: 12
            Layout.leftMargin: 16
            Layout.rightMargin: 16
            contentWidth: availableWidth
            clip: true

            ColumnLayout {
                width: scrollView.availableWidth
                spacing: 8

                RowLayout {
                    visible: entryMode === "qr"
                    Layout.fillWidth: true
                    spacing: 8

                    Button {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 50
                        text: qrFilePath ? qrFilePath.split("/").pop() : "选择图片..."
                        font.pixelSize: 14

                        background: Rectangle {
                            color: "#FFFFFF"
                            radius: 10
                            border.color: "#E2E8F0"
                        }
                        contentItem: Text {
                            text: parent.text
                            color: qrFilePath ? "#0F172A" : "#94A3B8"
                            font: parent.font
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                            elide: Text.ElideMiddle
                        }
                        onClicked: fileDialog.open()
                    }

                    Button {
                        Layout.preferredWidth: 100
                        Layout.preferredHeight: 50
                        text: "截屏识别"
                        font.pixelSize: 14

                        background: Rectangle {
                            color: "#FFFFFF"
                            radius: 10
                            border.color: "#E2E8F0"
                        }
                        contentItem: Text {
                            text: parent.text
                            color: "#94A3B8"
                            font: parent.font
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                        onClicked: {
                            if (isCapturing) return
                            isCapturing = true
                            qrStatus.text = "请选择二维码区域..."
                            qrStatus.color = "#64748B"

                            captureWindows = []
                            var comp = Qt.createComponent("qrc:/qml/ScreenCapture.qml")
                            for (var i = 0; i < Qt.application.screens.length; i++) {
                                var win = comp.createObject(page, { screenIndex: i })
                                win.regionSelected.connect(function(x, y, w, h) {
                                    closeAllCaptureWindows()
                                    qrStatus.text = "解码中..."
                                    qrStatus.color = "#64748B"
                                    captureTimer.rectX = x
                                    captureTimer.rectY = y
                                    captureTimer.rectW = w
                                    captureTimer.rectH = h
                                    captureTimer.start()
                                })
                                win.cancelled.connect(function() {
                                    closeAllCaptureWindows()
                                    qrStatus.text = ""
                                })
                                win.show()
                                captureWindows.push(win)
                            }
                        }
                    }
                }

                Text {
                    id: qrStatus
                    visible: text !== ""
                    Layout.fillWidth: true
                    font.pixelSize: 13
                    horizontalAlignment: Text.AlignHCenter
                    wrapMode: Text.WordWrap
                }

                RowLayout {
                    Layout.fillWidth: true
                    spacing: 12

                    Text {
                        text: "名称"
                        color: "#64748B"
                        font.pixelSize: 15
                        Layout.preferredWidth: 60
                    }

                    TextField {
                        id: nameField
                        Layout.fillWidth: true
                        placeholderText: "服务名称"
                        placeholderTextColor: "#94A3B8"
                        color: "#0F172A"
                        font.pixelSize: 16
                        leftPadding: 16

                        background: Rectangle {
                            color: "#FFFFFF"
                            radius: 10
                            border.color: "#E2E8F0"
                        }
                    }
                }

                RowLayout {
                    Layout.fillWidth: true
                    spacing: 12

                    Text {
                        text: "密钥"
                        color: "#64748B"
                        font.pixelSize: 15
                        Layout.preferredWidth: 60
                    }

                    TextField {
                        id: secretField
                        Layout.fillWidth: true
                        placeholderText: "密钥 (Base32)"
                        placeholderTextColor: "#94A3B8"
                        color: "#0F172A"
                        font.pixelSize: 16
                        font.capitalization: Font.AllUppercase
                        leftPadding: 16

                        background: Rectangle {
                            color: "#FFFFFF"
                            radius: 10
                            border.color: "#E2E8F0"
                        }
                    }
                }

                RowLayout {
                    Layout.fillWidth: true
                    spacing: 12

                    Text {
                        text: "账号"
                        color: "#64748B"
                        font.pixelSize: 15
                        Layout.preferredWidth: 60
                    }

                    TextField {
                        id: accountField
                        Layout.fillWidth: true
                        placeholderText: "账号 (可选)"
                        placeholderTextColor: "#94A3B8"
                        color: "#0F172A"
                        font.pixelSize: 16
                        leftPadding: 16

                        background: Rectangle {
                            color: "#FFFFFF"
                            radius: 10
                            border.color: "#E2E8F0"
                        }
                    }
                }

                RowLayout {
                    Layout.fillWidth: true
                    spacing: 12

                    Text {
                        text: "发行者"
                        color: "#64748B"
                        font.pixelSize: 15
                        Layout.preferredWidth: 60
                    }

                    TextField {
                        id: issuerField
                        Layout.fillWidth: true
                        placeholderText: "发行者 (可选)"
                        placeholderTextColor: "#94A3B8"
                        color: "#0F172A"
                        font.pixelSize: 16
                        leftPadding: 16

                        background: Rectangle {
                            color: "#FFFFFF"
                            radius: 10
                            border.color: "#E2E8F0"
                        }
                    }
                }

                Button {
                    Layout.fillWidth: true
                    text: advancedExpanded ? "▼ 高级设置" : "▶ 高级设置"
                    flat: true

                    contentItem: Text {
                        text: parent.text
                        color: "#94A3B8"
                        font.pixelSize: 14
                        horizontalAlignment: Text.AlignLeft
                        verticalAlignment: Text.AlignVCenter
                        leftPadding: 16
                    }

                    background: Rectangle {
                        color: "#FFFFFF"
                        radius: 10
                        border.color: "#E2E8F0"
                    }

                    onClicked: advancedExpanded = !advancedExpanded
                }

                ColumnLayout {
                    visible: advancedExpanded
                    spacing: 8

                    RowLayout {
                        Text {
                            text: "算法"
                            color: "#94A3B8"
                            font.pixelSize: 15
                            Layout.preferredWidth: 120
                        }

                        ComboBox {
                            id: algorithmCombo
                            Layout.fillWidth: true
                            model: ["SHA1", "SHA256", "SHA512"]
                            currentIndex: 0

                            background: Rectangle {
                                color: "#FFFFFF"
                                radius: 10
                                border.color: "#E2E8F0"
                            }
                            contentItem: Text {
                                text: algorithmCombo.currentText
                                color: "#0F172A"
                                font.pixelSize: 15
                                verticalAlignment: Text.AlignVCenter
                                leftPadding: 12
                            }
                        }
                    }

                    RowLayout {
                        Text {
                            text: "周期 (秒)"
                            color: "#94A3B8"
                            font.pixelSize: 15
                            Layout.preferredWidth: 120
                        }

                        ComboBox {
                            id: periodCombo
                            Layout.fillWidth: true
                            model: ["30", "60"]
                            currentIndex: 0

                            background: Rectangle {
                                color: "#FFFFFF"
                                radius: 10
                                border.color: "#E2E8F0"
                            }
                            contentItem: Text {
                                text: periodCombo.currentText
                                color: "#0F172A"
                                font.pixelSize: 15
                                verticalAlignment: Text.AlignVCenter
                                leftPadding: 12
                            }
                        }
                    }

                    RowLayout {
                        Text {
                            text: "位数"
                            color: "#94A3B8"
                            font.pixelSize: 15
                            Layout.preferredWidth: 120
                        }

                        ComboBox {
                            id: digitsCombo
                            Layout.fillWidth: true
                            model: ["6", "8"]
                            currentIndex: 0

                            background: Rectangle {
                                color: "#FFFFFF"
                                radius: 10
                                border.color: "#E2E8F0"
                            }
                            contentItem: Text {
                                text: digitsCombo.currentText
                                color: "#0F172A"
                                font.pixelSize: 15
                                verticalAlignment: Text.AlignVCenter
                                leftPadding: 12
                            }
                        }
                    }
                }

                Button {
                    Layout.fillWidth: true
                    text: "添加令牌"
                    font.pixelSize: 16
                    enabled: !adding

                    background: Rectangle {
                        color: adding ? "#94A3B8" : "#2563EB"
                        radius: 10
                    }
                    contentItem: Text {
                        text: parent.text
                        color: "white"
                        font: parent.font
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }

                    onClicked: {
                        if (adding) return
                        if (nameField.text.trim() === "" || secretField.text.trim() === "") {
                            errorText.visible = true
                            errorText.text = "名称和密钥不能为空"
                            errorTimer.restart()
                            return
                        }

                        adding = true

                        var data = {
                            "name": nameField.text.trim(),
                            "secret": secretField.text.trim(),
                            "account": accountField.text.trim(),
                            "issuer": issuerField.text.trim(),
                            "algorithm": algorithmCombo.currentText,
                            "period": parseInt(periodCombo.currentText),
                            "digits": parseInt(digitsCombo.currentText),
                            "tokenType": "TOTP",
                            "iconCollectionId": iconProvider.matchByName(nameField.text.trim())
                        }

                        if (tokenManager.addToken(data)) {
                            Qt.callLater(function() {
                                Window.window.popPage()
                            })
                        } else {
                            adding = false
                            errorText.visible = true
                            errorText.text = "添加令牌失败"
                            errorTimer.restart()
                        }
                    }
                }

                Text {
                    id: errorText
                    visible: false
                    Layout.fillWidth: true
                    text: ""
                    color: "#DC2626"
                    font.pixelSize: 14
                    horizontalAlignment: Text.AlignHCenter
                }

                Timer {
                    id: errorTimer
                    interval: 3000
                    onTriggered: errorText.visible = false
                }

                Item { Layout.preferredHeight: 16 }
            }
        }
    }

    function closeAllCaptureWindows() {
        captureTimer.stop()
        isCapturing = false
        for (var i = 0; i < captureWindows.length; i++) {
            if (captureWindows[i]) captureWindows[i].close()
        }
        captureWindows = []
    }

    Timer {
        id: captureTimer
        interval: 200
        running: false
        property int rectX: 0
        property int rectY: 0
        property int rectW: 0
        property int rectH: 0
        onTriggered: {
            qrDecoder.captureRect(rectX, rectY, rectW, rectH)
        }
    }

    FileDialog {
        id: fileDialog
        title: "选择二维码截图"
        nameFilters: ["图片文件 (*.png *.jpg *.jpeg *.bmp)", "所有文件 (*)"]
        onAccepted: {
            qrFilePath = selectedFile.toString().replace("file://", "")
            qrStatus.text = "解码中..."
            qrStatus.color = "#64748B"
            qrDecoder.decodeImage(qrFilePath)
        }
    }

    Connections {
        target: qrDecoder
        function onDecodeFinished(result) {
            qrStatus.text = "识别成功"
            qrStatus.color = "#16A34A"
            nameField.text = result.name || ""
            secretField.text = result.secret || ""
            accountField.text = result.account || ""
            issuerField.text = result.issuer || ""
            if (result.algorithm) {
                var alg = result.algorithm
                if (alg === "SHA256") algorithmCombo.currentIndex = 1
                else if (alg === "SHA512") algorithmCombo.currentIndex = 2
                else algorithmCombo.currentIndex = 0
            }
            if (result.period) {
                periodCombo.currentIndex = result.period === 60 ? 1 : 0
            }
            if (result.digits) {
                digitsCombo.currentIndex = result.digits === 8 ? 1 : 0
            }
        }
        function onDecodeError(error) {
            qrStatus.text = error
            qrStatus.color = "#DC2626"
        }
    }
}
