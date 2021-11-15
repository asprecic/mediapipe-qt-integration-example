import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 1.4
import com.example.renderer 1.0
import com.example.graphs 1.0

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Mediapipe integration example")

    Connections {
        target: MP

        onImageProcessed: {
            imageRenderer.setImage(image)
            imageRenderer.update()
        }
    }


    ColumnLayout {
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 5
        width: parent.width;
        spacing: 5

        RowLayout {
            ComboBox {
                model:[
                    { value: Graphs.SelfieSegmentation, text: styleClass.graphName(Graphs.SelfieSegmentation) },
                    { value: Graphs.HandTracking, text: styleClass.graphName(Graphs.HandTracking) },
                    { value: Graphs.FaceDetection, text: styleClass.graphName(Graphs.FaceDetection) },
                    { value: Graphs.FaceMesh, text: styleClass.graphName(Graphs.FaceMesh) },
                    { value: Graphs.HairSegmentation, text: styleClass.graphName(Graphs.HairSegmentation) },
                    { value: Graphs.HolisticTracking, text: styleClass.graphName(Graphs.HolisticTracking) },
                    { value: Graphs.IrisTracking, text: styleClass.graphName(Graphs.IrisTracking) },
                    { value: Graphs.ObjectDetection, text: styleClass.graphName(Graphs.ObjectDetection) },
                    { value: Graphs.PoseTracking, text: styleClass.graphName(Graphs.PoseTracking) },
                ]
                width: 200
                onActivated: {
                    MP.setGraph(model[currentIndex].value)
                }
            }
            Button {
                text: "Start camera"
                onClicked: MP.startCamera()
            }

            Button {
                text: "Stop camera"
                onClicked: MP.stopCamera()
            }
            Text {
                text: "FPS: " + MP.fps.toFixed(2)
            }
        }
        ImageRenderer {
            id: imageRenderer

            height: 1280;
            width: 720;
        }
    }

    StyleClass {
        id: styleClass
    }
}
