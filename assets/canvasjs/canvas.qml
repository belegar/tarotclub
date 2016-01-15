import QtQuick 2.4
import TarotClub 1.0
import "wrapper.js" as Wrapper

Canvas {
    id:canvas
    width: 320
    height: 480

    Timer {
        id: refresh
        interval: 30
        repeat: true
        running: false
        onTriggered: requestPaint();
      //  onTriggered: Wrapper.start(engine);
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

  MouseArea {
        id: mouseArea
        anchors.fill: parent
        onClicked: Wrapper.mouseEvent(mouse);
    }

}
