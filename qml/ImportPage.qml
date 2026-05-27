import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import QtQuick.Window
import AuthenDesk 1.0

Page {
    id: page
    background: Rectangle { color: "#F8FAFC" }

    property string filePath: ""
    property string statusMessage: ""
    property string activeTab: "import"
    property string exportPath: ""

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
                text: "备份与还原"
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
        anchors.margins: 16
        spacing: 16

        RowLayout {
            Layout.fillWidth: true
            spacing: 0

            Button {
                Layout.fillWidth: true
                Layout.preferredHeight: 38
                text: "导入"
                font.pixelSize: 15

                background: Rectangle {
                    color: activeTab === "import" ? "#2563EB" : "#F1F5F9"
                    radius: 10
                }
                contentItem: Text {
                    text: parent.text
                    color: activeTab === "import" ? "white" : "#64748B"
                    font: parent.font
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                onClicked: activeTab = "import"
            }

            Item { Layout.preferredWidth: 8 }

            Button {
                Layout.fillWidth: true
                Layout.preferredHeight: 38
                text: "导出"
                font.pixelSize: 15

                background: Rectangle {
                    color: activeTab === "export" ? "#2563EB" : "#F1F5F9"
                    radius: 10
                }
                contentItem: Text {
                    text: parent.text
                    color: activeTab === "export" ? "white" : "#64748B"
                    font: parent.font
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                onClicked: activeTab = "export"
            }
        }

        StackLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            currentIndex: activeTab === "import" ? 0 : 1

            // ---- Import Tab ----
            ColumnLayout {
                Item { Layout.fillHeight: true }

                Text {
                    text: "从 2FAS 备份文件导入令牌"
                    color: "#64748B"
                    font.pixelSize: 15
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                    wrapMode: Text.WordWrap
                }

                Button {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 50
                    font.pixelSize: 16

                    background: Rectangle {
                        color: "#FFFFFF"
                        radius: 10
                        border.color: "#E2E8F0"
                    }
                    contentItem: Text {
                        text: filePath ? fileDialog.selectedFile.toString().split("/").pop() : "选择 .2fas 文件..."
                        color: filePath ? "#0F172A" : "#94A3B8"
                        font: parent.font
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        elide: Text.ElideMiddle
                        width: parent.width - 24
                    }
                    onClicked: fileDialog.open()
                }

                TextField {
                    id: passwordField
                    Layout.fillWidth: true
                    placeholderText: "密码 (如已加密)"
                    placeholderTextColor: "#94A3B8"
                    color: "#0F172A"
                    echoMode: TextInput.Password
                    font.pixelSize: 16
                    leftPadding: 16

                    background: Rectangle {
                        color: "#FFFFFF"
                        radius: 10
                        border.color: "#E2E8F0"
                    }
                }

                Button {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 45
                    text: "开始导入"
                    font.pixelSize: 16
                    enabled: filePath !== ""

                    background: Rectangle {
                        color: filePath ? "#2563EB" : "#E2E8F0"
                        radius: 10
                    }
                    contentItem: Text {
                        text: parent.text
                        color: filePath ? "white" : "#94A3B8"
                        font: parent.font
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }

                    onClicked: {
                        var success = ImportParser.importFromFile(filePath, passwordField.text, tokenManager)
                        if (success) {
                            statusText.text = "导入成功！"
                            statusText.color = "#16A34A"
                        } else {
                            statusText.text = "导入失败，请检查文件和密码。"
                            statusText.color = "#DC2626"
                        }
                    }
                }

                Text {
                    id: statusText
                    visible: text !== ""
                    font.pixelSize: 14
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                    wrapMode: Text.WordWrap
                }

                Item { Layout.fillHeight: true }
            }

            // ---- Export Tab ----
            ColumnLayout {
                Item { Layout.fillHeight: true }

                Text {
                    text: "导出令牌到 2FAS 备份文件"
                    color: "#64748B"
                    font.pixelSize: 15
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                    wrapMode: Text.WordWrap
                }

                Text {
                    text: "当前共 " + tokenModel.count + " 个令牌"
                    color: "#94A3B8"
                    font.pixelSize: 14
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                }

                Button {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 50
                    font.pixelSize: 16

                    background: Rectangle {
                        color: "#FFFFFF"
                        radius: 10
                        border.color: "#E2E8F0"
                    }
                    contentItem: Text {
                        text: exportPath ? exportPath.split("/").pop() : "选择保存位置..."
                        color: exportPath ? "#0F172A" : "#94A3B8"
                        font: parent.font
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        elide: Text.ElideMiddle
                        width: parent.width - 24
                    }
                    onClicked: saveDialog.open()
                }

                Button {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 45
                    text: "导出"
                    font.pixelSize: 16
                    enabled: exportPath !== ""

                    background: Rectangle {
                        color: exportPath ? "#2563EB" : "#E2E8F0"
                        radius: 10
                    }
                    contentItem: Text {
                        text: parent.text
                        color: exportPath ? "white" : "#94A3B8"
                        font: parent.font
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }

                    onClicked: {
                        if (tokenManager.exportToFile(exportPath)) {
                            exportStatus.text = "导出成功！"
                            exportStatus.color = "#16A34A"
                        } else {
                            exportStatus.text = "导出失败"
                            exportStatus.color = "#DC2626"
                        }
                    }
                }

                Text {
                    id: exportStatus
                    visible: text !== ""
                    font.pixelSize: 14
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                    wrapMode: Text.WordWrap
                }

                Item { Layout.fillHeight: true }
            }
        }
    }

    FileDialog {
        id: fileDialog
        title: "选择 2FAS 备份文件"
        nameFilters: ["2FAS 备份 (*.2fas)", "所有文件 (*)"]
        fileMode: FileDialog.OpenFile
        onAccepted: {
            filePath = selectedFile.toString().replace("file://", "")
        }
    }

    FileDialog {
        id: saveDialog
        title: "保存 2FAS 备份文件"
        nameFilters: ["2FAS 备份 (*.2fas)", "所有文件 (*)"]
        fileMode: FileDialog.SaveFile
        currentFile: new Date().toISOString().slice(0,10).replace(/-/g,"") + "_authendesk.2fas"
        onAccepted: {
            exportPath = selectedFile.toString().replace("file://", "")
        }
    }
}
