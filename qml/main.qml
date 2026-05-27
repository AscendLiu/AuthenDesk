import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window

ApplicationWindow {
    id: window
    width: 420
    height: 680
    visible: true
    title: "AuthenDesk"
    color: "#F8FAFC"

    property string currentPage: "list"

    StackView {
        id: stackView
        anchors.fill: parent
        initialItem: tokenListPage
    }

    Component {
        id: tokenListPage
        TokenListPage {}
    }

    Component {
        id: addTokenPage
        AddTokenPage {}
    }

    Component {
        id: importPage
        ImportPage {}
    }

    Component {
        id: editTokenPage
        EditTokenPage {}
    }

    function navigateTo(page) {
        if (page === "add") {
            stackView.push(addTokenPage)
        } else if (page === "backup") {
            stackView.push(importPage)
        }
    }

    function navigateToEdit(index) {
        stackView.push(editTokenPage, { tokenIndex: index })
    }

    function popPage() {
        stackView.pop()
    }
}
