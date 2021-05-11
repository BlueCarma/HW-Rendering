import QtQuick 2.12
import QtQuick.Window 2.12
import CrossControl 1.0

Window {
    width: 800
    height: 480
    visible: true
    title: qsTr("Hardware accelerated rendering")


    ImagePainter {
        anchors.fill: parent

    }

    Image {
        source: "qrc:/tractor-2.0.png"
        height: 200
        width: 180
        rotation: 90
        anchors.centerIn: parent
    }
}
