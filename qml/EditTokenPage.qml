import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window

Page {
    id: page
    background: Rectangle { color: "#F8FAFC" }

    property int tokenIndex: -1
    property var tokenData: ({})

    property bool advancedExpanded: false
    property string detectedCollectionId: ""

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
                text: "编辑令牌"
                color: "#0F172A"
                font.pixelSize: 20
                font.bold: true
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter
                Layout.alignment: Qt.AlignVCenter
            }
        }
    }

    ScrollView {
        id: scrollView
        anchors.fill: parent
        anchors.margins: 16
        contentWidth: availableWidth
        clip: true

        ColumnLayout {
            width: scrollView.availableWidth
            spacing: 8

            RowLayout {
                Layout.fillWidth: true
                spacing: 12

                Text {
                    text: "名称"
                    color: "#64748B"
                    font.pixelSize: 15
                    Layout.preferredWidth: 80
                }

                TextField {
                    id: nameField
                    Layout.fillWidth: true
                    placeholderText: "服务名称"
                    placeholderTextColor: "#94A3B8"
                    color: "#0F172A"
                    font.pixelSize: 16
                    leftPadding: 16
                    text: tokenData.name || ""

                    background: Rectangle {
                        color: "#FFFFFF"
                        radius: 10
                        border.color: "#E2E8F0"
                    }

                    onTextChanged: {
                        var collId = iconProvider.matchByName(text)
                        detectedCollectionId = collId || ""
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
                    Layout.preferredWidth: 80
                }

                TextField {
                    id: secretField
                    Layout.fillWidth: true
                    placeholderText: "Base32 密钥"
                    placeholderTextColor: "#94A3B8"
                    color: "#0F172A"
                    font.pixelSize: 16
                    font.capitalization: Font.AllUppercase
                    leftPadding: 16
                    text: tokenData.secret || ""

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
                    Layout.preferredWidth: 80
                }

                TextField {
                    id: accountField
                    Layout.fillWidth: true
                    placeholderText: "可选"
                    placeholderTextColor: "#94A3B8"
                    color: "#0F172A"
                    font.pixelSize: 16
                    leftPadding: 16
                    text: tokenData.account || ""

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
                    Layout.preferredWidth: 80
                }

                TextField {
                    id: issuerField
                    Layout.fillWidth: true
                    placeholderText: "可选"
                    placeholderTextColor: "#94A3B8"
                    color: "#0F172A"
                    font.pixelSize: 16
                    leftPadding: 16
                    text: tokenData.issuer || ""

                    background: Rectangle {
                        color: "#FFFFFF"
                        radius: 10
                        border.color: "#E2E8F0"
                    }
                }
            }

            Button {
                Layout.fillWidth: true
                text: advancedExpanded ? "\u25BC Advanced Settings" : "\u25B6 Advanced Settings"
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
                        currentIndex: {
                            var a = tokenData.algorithm || "SHA1"
                            if (a === "SHA256") return 1
                            if (a === "SHA512") return 2
                            return 0
                        }

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
                        currentIndex: tokenData.period === 60 ? 1 : 0

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
                        currentIndex: tokenData.digits === 8 ? 1 : 0

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
                text: "保存修改"
                font.pixelSize: 16

                background: Rectangle {
                    color: "#2563EB"
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
                    if (nameField.text.trim() === "" || secretField.text.trim() === "") {
                        errorText.visible = true
                        errorText.text = "名称和密钥不能为空"
                        errorTimer.restart()
                        return
                    }

                    var data = {
                        "name": nameField.text.trim(),
                        "secret": secretField.text.trim(),
                        "account": accountField.text.trim(),
                        "issuer": issuerField.text.trim(),
                        "algorithm": algorithmCombo.currentText,
                        "period": parseInt(periodCombo.currentText),
                        "digits": parseInt(digitsCombo.currentText),
                        "iconCollectionId": detectedCollectionId || tokenData.iconCollectionId || ""
                    }

                    if (tokenModel.modelUpdateToken(tokenIndex, data)) {
                        Window.window.popPage()
                    } else {
                        errorText.visible = true
                        errorText.text = "保存修改失败"
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
        }
    }

    onTokenIndexChanged: {
        if (tokenIndex >= 0) {
            tokenData = tokenModel.get(tokenIndex)
        }
    }
}
