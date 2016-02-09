import QtQuick 2.5
import TarotClub 1.0
import "wrapper.js" as Wrapper

Canvas {
    id:canvas
    width: 320
    height: 480
    // Additional properties for browser faking
    property real offsetTop: 0
    property real offsetLeft: 0
    property var offsetParent: null

    Timer {
        id: refresh
        interval: 60
        repeat: true
        running: false
        onTriggered: requestPaint();
    }

    Timer {
        id: startup
        interval: 2000
        repeat: false
        running: false
        onTriggered: {

            Wrapper.initialize();
            refresh.start();
        }

    }

    Component.onCompleted: startup.start();

    onPaint: {
      Wrapper.paint();
    }

  Document {
    id: document

    function getElementById()
    {
        return canvas;
    }

    function addEventListener(name, callback, useCapture)
    {
        Wrapper.addEventListener(name, callback);
    }
  }

  WindowElement {
      id: window
      property int innerWidth: canvas.width;
      property int innerHeight: canvas.height;


      function requestAnimationFrame(callback)
      {
          canvas.requestAnimationFrame(callback);
          canvas.requestPaint();
      }

  }
/*
  Image {
      source: "qrc:/cards/default/14-H.svg"
  }
*/

  MouseArea {
        id: mouseArea
        hoverEnabled: true
        anchors.fill: parent
        onClicked: Wrapper.mouseEvent(mouse);
        onPositionChanged: Wrapper.mouseMoveEvent(mouse);
    }

}
