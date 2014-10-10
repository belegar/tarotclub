import QtQuick 2.3

Rectangle {
    id: rectangle
    color: "red"
    width: 200
    height: 200

    Text {
        id: text
        text: "This is QML code.\n(Click to pause)"
        font.pointSize: 14
        anchors.centerIn: parent
        PropertyAnimation {
            id: animation
            target: text
            property: "rotation"
            from: 0; to: 360; duration: 5000
            loops: Animation.Infinite
        }
    }
    MouseArea {
        anchors.fill: parent
        onClicked: animation.paused ? animation.resume() : animation.pause()
    }
	
	Column {
        Image {
            sourceSize.width: 61
            sourceSize.height: 112
            source: "image://minideck/01-T"
        }

	}
	
    Component.onCompleted: animation.start()
}
