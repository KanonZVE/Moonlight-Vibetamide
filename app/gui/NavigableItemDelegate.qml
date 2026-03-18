import QtQuick 2.0
import QtQuick.Controls 2.2

ItemDelegate {
    property GridView grid

    highlighted: grid.activeFocus && grid.currentItem === this

    // Moonlight Vitaminado: Smooth scale and opacity transitions
    scale: highlighted ? 1.05 : 1.0
    opacity: highlighted ? 1.0 : 0.9

    Behavior on scale { NumberAnimation { duration: 200; easing.type: Easing.OutBack } }
    Behavior on opacity { NumberAnimation { duration: 200 } }

    Keys.onLeftPressed: {
        grid.moveCurrentIndexLeft()
    }
    Keys.onRightPressed: {
        grid.moveCurrentIndexRight()
    }
    Keys.onDownPressed: {
        grid.moveCurrentIndexDown()
    }
    Keys.onUpPressed: {
        grid.moveCurrentIndexUp()

        // If we've reached the top of the grid, move focus to the toolbar
        if (grid.currentItem === this) {
            nextItemInFocusChain(false).forceActiveFocus(Qt.TabFocus)
        }
    }
    Keys.onReturnPressed: {
        clicked()
    }
    Keys.onEnterPressed: {
        clicked()
    }
}
