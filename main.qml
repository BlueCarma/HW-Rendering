import QtQuick 2.12
import QtQuick.Window 2.12
import CrossControl 1.0

Window {
    width: 800
    height: 480
    visible: true
    title: qsTr("Hardware accelerated rendering")


    ImagePainter {
        anchors.centerIn: parent
        width: 800
        height: 480
    }
}
