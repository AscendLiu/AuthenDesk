import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
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
                text: Strings.back
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
                text: Strings.importExportTitle
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
                text: Strings.importTab
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
                text: Strings.exportTab
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

            ColumnLayout {
                Item { Layout.fillHeight: true }

                Text {
                    text: Strings.importFrom2fas
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
                        text: filePath ? filePath.split("/").pop() : Strings.select2fasFile
                        color: filePath ? "#0F172A" : "#94A3B8"
                        font: parent.font
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        elide: Text.ElideMiddle
                        width: parent.width - 24
                    }
                    onClicked: {
                        var result = nativeFileDialog.getOpenFileName(
                            Strings.select2fasBackup,
                            "",
                            Strings.twofasBackup + ";;" + Strings.allFiles
                        )
                        if (result) {
                            filePath = result
                        }
                    }
                }

                TextField {
                    id: passwordField
                    Layout.fillWidth: true
                    placeholderText: Strings.passwordIfEncrypted
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
                    text: Strings.startImport
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
                            statusText.text = Strings.importSuccess
                            statusText.color = "#16A34A"
                        } else {
                            statusText.text = Strings.importFailed
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

            ColumnLayout {
                Item { Layout.fillHeight: true }

                Text {
                    text: Strings.exportTo2fas
                    color: "#64748B"
                    font.pixelSize: 15
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                    wrapMode: Text.WordWrap
                }

                Text {
                    text: Strings.currentTokenCount.arg(tokenModel.count)
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
                        text: exportPath ? exportPath.split("/").pop() : Strings.selectSaveLocation
                        color: exportPath ? "#0F172A" : "#94A3B8"
                        font: parent.font
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        elide: Text.ElideMiddle
                        width: parent.width - 24
                    }
                    onClicked: {
                        var result = nativeFileDialog.getSaveFileName(
                            Strings.save2fasBackup,
                            "",
                            new Date().toISOString().slice(0,10).replace(/-/g,"") + "_authendesk.2fas",
                            Strings.twofasBackup + ";;" + Strings.allFiles
                        )
                        if (result) {
                            exportPath = result
                        }
                    }
                }

                Button {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 45
                    text: Strings.exportButton
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
                            exportStatus.text = Strings.exportSuccess
                            exportStatus.color = "#16A34A"
                        } else {
                            exportStatus.text = Strings.exportFailed
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
}
