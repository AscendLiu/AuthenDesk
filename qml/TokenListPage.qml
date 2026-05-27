import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window

Page {
    id: page
    background: Rectangle { color: "#F8FAFC" }

    onVisibleChanged: {
        if (visible) {
            listView.model = null
            listView.model = tokenModel
        }
    }

    header: Rectangle {
        height: 60
        color: "#FFFFFF"
        z: 10

        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: 16
            anchors.rightMargin: 16
            spacing: 0

            TextField {
                id: searchField
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignVCenter
                Layout.rightMargin: 8
                placeholderText: "搜索令牌..."
                placeholderTextColor: "#94A3B8"
                color: "#0F172A"
                font.pixelSize: 16
                leftPadding: 16
                rightPadding: 36

                background: Rectangle {
                    color: "#F1F5F9"
                    radius: 10
                    border.color: "#E2E8F0"
                }

                onTextChanged: tokenModel.filterText = text
            }

            Item {
                parent: searchField
                z: 10
                visible: searchField.text !== ""
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right
                anchors.rightMargin: 10
                width: 20
                height: 20

                Text {
                    anchors.centerIn: parent
                    text: "\u2715"
                    color: "#94A3B8"
                    font.pixelSize: 16
                }

                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        searchField.text = ""
                        tokenModel.filterText = ""
                    }
                }
            }

            Button {
                Layout.alignment: Qt.AlignVCenter
                Layout.preferredWidth: 36
                Layout.preferredHeight: 36
                flat: true

                contentItem: Text {
                    text: "\u22EE"
                    color: "#64748B"
                    font.pixelSize: 20
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                background: Rectangle { color: "transparent" }

                onClicked: menuPopup.open()

                Menu {
                    id: menuPopup
                    y: parent.height

                    MenuItem {
                        text: "添加令牌"
                        font.pixelSize: 14
                        onTriggered: Window.window.navigateTo("add")
                    }
                    MenuItem {
                        text: "备份与还原"
                        font.pixelSize: 14
                        onTriggered: Window.window.navigateTo("backup")
                    }
                    MenuSeparator {}
                    MenuItem {
                        text: "清空全部令牌"
                        font.pixelSize: 14
                        onTriggered: clearDialog.open()
                    }
                    MenuSeparator {}
                    MenuItem {
                        text: "关于"
                        font.pixelSize: 14
                        onTriggered: aboutDialog.open()
                    }
                }
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 1
            color: "#E2E8F0"
        }

        ListView {
            id: listView
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.margins: 0
            model: tokenModel
            clip: true
            spacing: 2

            delegate: TokenDelegate {}
        }
    }

    Dialog {
        id: clearDialog
        title: "清空全部令牌"
        anchors.centerIn: parent
        modal: true

        background: Rectangle {
            color: "#FFFFFF"
            radius: 12
            border.color: "#E2E8F0"
        }

        header: Text {
            text: "清空全部令牌"
            color: "#0F172A"
            font.pixelSize: 18
            font.bold: true
            leftPadding: 24
            topPadding: 24
        }

        contentItem: ColumnLayout {
            spacing: 16
            width: 360

            Text {
                text: "确定要删除全部令牌吗？此操作不可撤销。"
                color: "#64748B"
                font.pixelSize: 14
                wrapMode: Text.WordWrap
                Layout.fillWidth: true
            }

            RowLayout {
                Layout.fillWidth: true
                spacing: 12

                Button {
                    Layout.fillWidth: true
                    text: "取消"
                    font.pixelSize: 14

                    background: Rectangle {
                        color: "#F1F5F9"
                        radius: 8
                        border.color: "#E2E8F0"
                    }
                    contentItem: Text {
                        text: parent.text
                        color: "#0F172A"
                        font: parent.font
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    onClicked: clearDialog.close()
                }

                Button {
                    Layout.fillWidth: true
                    text: "清空"
                    font.pixelSize: 14

                    background: Rectangle {
                        color: "#DC2626"
                        radius: 8
                    }
                    contentItem: Text {
                        text: parent.text
                        color: "white"
                        font: parent.font
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    onClicked: {
                        tokenManager.clearAllTokens()
                        clearDialog.close()
                    }
                }
            }
        }
    }

    Dialog {
        id: aboutDialog
        title: "关于"
        anchors.centerIn: parent
        modal: true
        width: 320

        background: Rectangle {
            color: "#FFFFFF"
            radius: 12
            border.color: "#E2E8F0"
        }

        contentItem: ColumnLayout {
            spacing: 12
            width: parent.width

            Image {
                Layout.preferredWidth: 80
                Layout.preferredHeight: 80
                Layout.alignment: Qt.AlignHCenter
                Layout.topMargin: 8
                source: "qrc:/assets/app_icon.png"
                fillMode: Image.PreserveAspectFit
            }

            Text {
                text: "AuthenDesk"
                color: "#0F172A"
                font.pixelSize: 20
                font.bold: true
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter
            }

            Text {
                text: "版本 1.0.0"
                color: "#64748B"
                font.pixelSize: 14
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter
            }

            Text {
                text: "基于 Qt 6.8 + QML 的桌面双因素认证令牌管理器。\n支持 TOTP/HOTP，从 2FAS 备份导入导出，\n二维码识别导入。"
                color: "#64748B"
                font.pixelSize: 13
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
                Layout.leftMargin: 16
                Layout.rightMargin: 16
            }

            Button {
                Layout.fillWidth: true
                Layout.preferredHeight: 40
                text: "确定"
                font.pixelSize: 14

                background: Rectangle {
                    color: "#2563EB"
                    radius: 8
                }
                contentItem: Text {
                    text: parent.text
                    color: "white"
                    font: parent.font
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                onClicked: aboutDialog.close()
            }
        }
    }
}
