import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3
import QtQuick.Window 2.15
import QtQuick.Controls.Material 2.15

ApplicationWindow {
    id:root
    visible:true
    color:"black"

    signal  map_image_Captured()
    property string vid_path: ""
    property bool edit_not: false
    property color neonblue: "#00FFFF"
    property color neonGreen: "#39FF14"
    property bool connect_pop_status: false
    property bool connect_status:true
    property bool start_or_stop: false
    property bool pause_play: false
    property string record_tool_text: "Start\nVideo Recording"

    property string record_tool_text2: "Pause Recording"

    property bool start_status: false

    property bool stop_status: false
    property bool pause_status: false
    property bool pause_status2: false
    property bool button_Pressed: false  // Track space key state

    //Enhancement parameters

    property bool is_overlay: false
    /*Image {
        id: photoPreview
    }*/

        Component.onCompleted:
        {
            //VideoStreamer.openVideoCamera(0);
             VideoStreamer.openVideoCamera();

            //VideoStreamer.openVideoCamera2(0);
            //VideoStreamer.openVideoCamera2(1);
            //VideoStreamer.openVideoCamera("tcpclientsrc host=192.168.56.1 port=5000 ! gdpdepay ! rtph264depay ! h264parse ! avdec_h264 ! videoconvert ! autovideosink");
            opencvImage.visible=true;
            opencvImage2.visible=true;
        }

        Connections{
            target: liveImageProvider

            function onImageChanged()
            {
                opencvImage2.reload()
                //opencvImage.reload()
            }

            function onImageChanged2()
            {
                opencvImage.reload()
                //opencvImage2.reload()
            }


        }

        Connections
        {
            target: VideoStreamer

            function onRecording_stop()
            {
                clear_status()
                template_text2.text="Alert!!!"
                template_content2.text="Video has been saved successfully!!!"
                message_template2.open()

            }
        }

        Item {
            focus: true
        Keys.onPressed: function(event) {
            if (event.key === Qt.Key_Tab) {
                //button_Pressed =!button_Pressed
                console.log("Button Pressed");
                         if(button_Pressed)
                         {
                            //console.log("True");
                             button_Pressed=false;
                             VideoStreamer.pushback(true)
                         }
                         else if(!button_Pressed)
                         {
                             //console.log("False");
                            button_Pressed=true;
                             VideoStreamer.pushback(false)
                            }
                      }

        }

        Keys.onReleased:  function(event)  {

            if (event.key === Qt.Key_Space) {
                //button_Pressed =!button_Pressed
                //console.log("Button Pressed");
                         if(button_Pressed)
                         {
                            //console.log("True");
                             button_Pressed=false;
                             VideoStreamer.pushback(false)
                         }
                         else if(!button_Pressed)
                         {
                             //console.log("False");
                            button_Pressed=true;
                             VideoStreamer.pushback(true)
                         }
                      }


        }
        }

        Rectangle{
            id: imageRect
            //anchors.horizontalCenter: parent.horizontalCenter
            //anchors.verticalCenter: parent.verticalCenter
            //anchors.verticalCenterOffset: 0.039*parent.width
            anchors.left: parent.left
            anchors.right:parent.right
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            height: 0.95*parent.height
            anchors.margins: 0.005*parent.width
            color: "black"
            border.color: "white"
            border.width: 3
            visible: true
            Image{
                id: opencvImage
                width: 0.99*parent.width
                height:0.99*parent.height
                anchors.centerIn: parent
                fillMode: Image.PreserveAspectFit
                property bool counter: false
                visible: true
                source:"qrc:/dvr_system/images/dummy_template3.jpg"
                asynchronous: false
                cache: false


                function reload()
                {
                    counter = !counter
                    source = "image://live/0?id=" + counter
                }

            }

        }

        Rectangle{
            id: imageRect2
            anchors.bottom: imageRect.bottom
            anchors.bottomMargin: 0.005*parent.width
            anchors.right: imageRect.right
            anchors.rightMargin: 0.005*parent.width
            width: 0.2*parent.width
            height: 0.2*parent.height
            //anchors.margins: 0.005*parent.width
            color: "black"
            border.color: "white"
            border.width: 3
            visible: true
            opacity:1.0
            scale:1

            Image{
                id: opencvImage2
                width: 0.975*parent.width
                height:0.9*parent.height
                anchors.centerIn: parent
                fillMode: Image.PreserveAspectFit
                property bool counter: false
                visible: true
                source:"qrc:/dvr_system/images/dummy_template3.jpg"
                asynchronous: false
                cache: false


                function reload()
                {
                    counter = !counter
                    source = "image://live/1?id=" + counter
                }

            }

        }

        Dialog
            {
                id:message_template2
                //standardButtons: StandardButton.Ok|StandardButton.Cancel
                //title:"Confirmation"
                z:2
                anchors.centerIn: parent

                header: Text
                            {
                                id:template_text2
                                color: "white"
                                text:"Acknowledgement"
                                font.bold: true
                                font.pointSize: 0.01*root.width
                                style: Text.Sunken
                                //anchors.top:parent.top
                                //anchors.topMargin:0.01*parent.width
                                anchors.left:parent.left
                                anchors.leftMargin:0.05*parent.width
                            }

                contentItem : Text
                                {
                                    id:template_content2
                                    color: "white"
                                    verticalAlignment: Text.AlignVCenter
                                    horizontalAlignment: Text.AlignHCenter
                                    font.pointSize: 0.006*root.width
                                    style: Text.Sunken
                                    anchors.top:template_text2.bottom
                                    anchors.topMargin:0.04*parent.width
                                    anchors.left:parent.left
                                    anchors.leftMargin:0.05*parent.width
                                    anchors.right:parent.right
                                    anchors.rightMargin:0.05*parent.width
                                }
                footer:DialogButtonBox {
                    alignment: Qt.AlignCenter
                    buttonLayout : DialogButtonBox.WinLayout
                    background: Rectangle
                                    {
                                        anchors.fill:parent
                                        color:"transparent"

                                    }

                    Button {
                        //text: qsTr("Save")
                        DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
                        background: Rectangle
                        {
                            color:"#1a3154"
                        }

                        contentItem: Text {
                            id:response_button4
                            text:"Close"
                            font.pixelSize:  Math.min(root.width/90,root.height/70)
                            style: Text.Sunken
                            color:"White"
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                    }
                }

                background: Rectangle
                                {
                                    color:"black"
                                    opacity:0.75
                                    radius:0.01*parent.width
                                    border.color:"#21be2b"
                                }

                onAccepted:
                {
                    message_template2.close()
                }

            }


      Dialog
          {
              id:message_template3
              z:2
              anchors.centerIn: parent

              header: Text
                          {
                              id:template_text3
                              color: "white"
                              text:"Alert"
                              font.bold: true
                              font.pointSize: 0.01*root.width
                              style: Text.Sunken
                              //anchors.top:parent.top
                              //anchors.topMargin:0.01*parent.width
                              anchors.left:parent.left
                              anchors.leftMargin:0.05*parent.width
                          }

              contentItem : Text
                              {
                                  id:template_content3
                                  color: "white"
                                  verticalAlignment: Text.AlignVCenter
                                  horizontalAlignment: Text.AlignHCenter
                                  font.pointSize: 0.0075*root.width
                                  style: Text.Sunken
                                  anchors.top:template_text3.bottom
                                  anchors.topMargin:0.04*parent.width
                                  anchors.left:parent.left
                                  anchors.leftMargin:0.05*parent.width
                              }

              background: Rectangle
                              {
                                  color:"black"
                                  opacity:0.75
                                  radius:0.01*parent.width
                                  border.color:"#21be2b"
                              }

              onRejected:
              {
                  message_template.close()
              }
          }

      Timer
      {
          id:timer
          repeat: true
          running:true
          interval: 500

          onTriggered:
          {
              //signalEmitter.checkIP("192.168.56.1");

          }
      }
      function tooltip_reset()
      {
          tooltip_template.x=0
          tooltip_template.y=0
          tooltip_template.text=""
          tooltip_template.visible=false
     }

      function tooltip_set(x,y,text)
      {
          tooltip_template.x= x
          tooltip_template.y= y
          tooltip_template.text=text
          tooltip_template.visible=true
          start_Timer3.start()
      }

      Timer {
          id: start_Timer3
          interval:1500// Timer interval in milliseconds
          running: false // Start the timer when the application starts

          repeat: false

                      onTriggered: {
                          tooltip_reset()
                      }
      }


      ToolTip
      {
          id:tooltip_template
          x:0
          y:0
          z:1
          text:""
          contentItem: Text
                         {
                             color:neonGreen
                             text:tooltip_template.text
                         }
          background: Rectangle
                          {    color:"black"
                              opacity:0.5
                              border.color: "#21be2b"
                          }
      }




      Timer {
          id: start_Timer
          interval: 1000 // Timer interval in milliseconds (1 second in this case)
          running: false // Start the timer when the application starts

          repeat: true

           property int elapsedTime: 0 // Elapsed time in seconds

                      onTriggered: {
                          updateTimer();
                      }
      }
      function updateTimer() {
          // This function will be called every time the timer triggers
          var hours = Math.floor(start_Timer.elapsedTime / 3600);
                      var minutes = Math.floor((start_Timer.elapsedTime % 3600) / 60);
                      var seconds = start_Timer.elapsedTime % 60;

          timerText.text = `${hours.toString().padStart(2, '0')}:${minutes.toString().padStart(2, '0')}:${seconds.toString().padStart(2, '0')}`;



                      start_Timer.elapsedTime++;
      }

      Text {
                        id: timerText
                        color: "White"
                        visible: statusindicator.visible
                        anchors.left: statusindicator.right
                        anchors.leftMargin:0.005*root.width
                        anchors.bottom: statusindicator.bottom
                        //anchors.bottomMargin: 0.005*parent.width
                        font.pixelSize:Math.min(root.width/47,root.height/37)
                        opacity: statusindicator.opacity
                    }

      Rectangle
      {
          id:statusindicator
          anchors
          {
              bottom:imageRect.bottom
              left:imageRect.left
              margins:0.005*root.width
          }
          width:0.015*root.width
          height: width
          radius:100
          color: "red"
          visible: false
      }

      Rectangle
      {
          id:tray_3
          anchors.left: parent.left
          anchors.right: parent.right
          anchors.top:parent.top
          anchors.bottom: imageRect.top
          anchors.margins: 0.005*parent.width
          color: "#5978AC"
          opacity: 0.5
          radius: 5
          z:0
      }

      Image {
          id: vikra
          source: "qrc:/dvr_system/images/vikra_2.jpeg"
          anchors.left: imageRect.left
          anchors.leftMargin:0.05*parent.width
          anchors.top:parent.top
          anchors.topMargin: 0.02*parent.height
          //visible: cme.visible
          width:0.055*parent.width
          height:width
      }

onClosing:
{

    VideoStreamer.stop_recording()
}




function capture_map_image()
               {
                    var saveDirectory;
                        if (Qt.platform.os === "windows") {
                            saveDirectory = StandardPaths.writableLocation(StandardPaths.PicturesLocation);
                        } else {
                           saveDirectory = StandardPaths.writableLocation(StandardPaths.HomeLocation);
                        }
                        var logFileDir = saveDirectory + "/UWC Images";


                       // Capture and save the image
                        var rectangles = [imageRect]
                        var rect,count_ss= 0;
                        for (var i = 0; i < rectangles.length; i++) {
                            (function (rect) {
                                var date = new Date();
                                var formattedDate = date.toLocaleString(Qt.locale("en_IN"), "dd.MM.yyyy-hh.mm.ss");
                                var fileName = "/screenshot_" + formattedDate + "_" + i;
                                rect.grabToImage(function (result) {
                                    var filePath = logFileDir + fileName;
                                    //console.log(filePath)
                                    filePath = filePath.replace("file:///", "");
                                    //console.log(filePath)
                                    if (result.saveToFile(filePath + ".jpeg")) {
                                        //console.log("Success: " + filePath);
                                        count_ss++
                                    } else {
                                        console.log("Failure");
                                    }
                                }, Qt.size(opencvImage.sourceSize.width, opencvImage.sourceSize.height));
                            })(rectangles[i]);
                        }
                           map_image_Captured()

           }

ListView {
    id: fileListView
    //anchors.fill: tray_5
    visible: false
    model: ListModel {
        id: fileModel
        // Add file items here
    }

    delegate: Item {
        width: fileListView.width
        height: 40

        Rectangle {
            width: parent.width
            height: 40
            color: "lightgray"
            border.color: "gray"
            radius: 5

            Text {
                anchors.centerIn: parent
                text: model.fileName
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    // Handle file selection here
                }
            }
        }
    }
}
onMap_image_Captured:
{
    clear_status()
    template_text2.text="Capture Status Message!!!    "
    template_content2.text="Frame has been\n Captured Successfully!!!"
    message_template2.open()
}

Rectangle
{
    id:main_rect
    color: "black"
    opacity:0.5
    anchors.horizontalCenter: parent.horizontalCenter
    radius:0.005*root.width
    anchors.bottom: imageRect.bottom
    anchors.bottomMargin: 0.005*root.width
    width: 0.15*root.width
    height:0.09*root.width

}

Rectangle
{
    id:sub_rect
    color: "black"
    opacity:0.5
    anchors.left: main_rect.right
    anchors.leftMargin: 0.005*root.width
    radius:0.005*root.width
    anchors.bottom: imageRect.bottom
    anchors.bottomMargin: 0.005*root.width
    width: 0.15*root.width
    height:0.09*root.width
    visible: false
}

Button
    {
        id: overlay_button
        width:0.03*root.width
        height:0.03*root.width
        //Layout.alignment: Qt.AlignHCenter
        visible: sub_rect.visible

        property bool talk:false
        focus: true  // Ensure this item can receive keyboard events

        background: Rectangle
        {
            anchors.fill:parent
            color:"grey"//"#011026"
            opacity:1.0
            radius:100
        }
        anchors
        {
            top:sub_rect.top
            topMargin:0.005*root.width
            horizontalCenter:sub_rect.horizontalCenter
        }
        contentItem: Image {
            id:area_15
            source: "qrc:/dvr_system/images/subtitle.svg"
            width:0.9*parent.width
            height:0.9*parent.width
            anchors.centerIn: parent
        }
        MouseArea
        {
            anchors.fill: parent
            hoverEnabled: true

            onClicked:
            {
                if(!is_overlay)
                {
                VideoStreamer.is_subttitle(false);
                clear_status()
                template_text2.text="Overlay Status Message!!!    "
                template_content2.text="Frame Overlay Enabled\n in the\n Current Recording!!!"
                message_template2.open()
                }
                else
                {
                    clear_status()
                    template_text2.text="Overlay Status Alert!!!    "
                    template_content2.text="Frame Overlay already Enabled\n in the\n Current Recording!!!"
                    message_template2.open()
                }
            }
            onEntered:
            {
                tooltip_set(overlay_button.x+mouseX,overlay_button.y-overlay_button.height,"Overlay option")
            }

            onExited:
            {
                tooltip_reset()
            }

            onPressed:
            {
            }

            onPressAndHold:
            {

            }

            onReleased:
            {

            }

            onDoubleClicked:
            {
                /*if(area_15.source=="qrc:/dvr_system/images/mike_on.png")
                {
                    mike_on_off.talk=false
                    area_15.source="qrc:/dvr_system/images/mike_off.png"
                    VideoStreamer.push_to_talk(true)

                }*/


            }


        }
    }
RowLayout
                    {
                    id:main_row
                    width: main_rect.width
                    height:main_rect.height
                    spacing: 0.05*root.width


                    }
                function timer_reset()
                {
                    mission_timer.stop()
                    mission_timer.elapsedTime=0
                }
                    RowLayout
                                        {
                                            id:row
                                            Layout.fillWidth: pause_recording.visible?true:false
                                            /*Label
                                            {
                                                text: qsTr("Brightness")
                                                font.pixelSize: Math.min(root.width/60,root.height/70)
                                                font.bold: true
                                                color:"white"
                                            }*/
                                        }
                                            Button
                                                {
                                                    id:start_stop_recording
                                                    width:0.03*root.width
                                                    height:0.03*root.width
                                                    anchors
                                                    {
                                                        top:main_rect.top
                                                        topMargin:0.005*root.width
                                                        horizontalCenter:main_rect.horizontalCenter
                                                        horizontalCenterOffset:pause_recording.visible==false?0:-0.25*main_rect.width
                                                    }

                                                    Behavior on anchors.horizontalCenterOffset {
                                                        PropertyAnimation { duration: 300; easing.type: Easing.InOutQuad }
                                                    }
                                                    visible: true
                                                    background: Rectangle
                                                    {
                                                        anchors.fill:parent
                                                        color:"grey"//"#011026"
                                                        opacity:0.5
                                                        radius:100
                                                    }

                                                    contentItem: Canvas {
                                                        id:sta_sto
                                                        anchors.fill: parent
                                                        contextType: "2d"
                                                        anchors.centerIn: parent

                                                         onPaint: {
                                                             var ctx = getContext("2d");
                                                             ctx.clearRect(0, 0, width, height); // Clear the canvas

                                                             if (!start_or_stop) {
                                                                 ctx.fillStyle = "#eadeda";
                                                                 ctx.beginPath();
                                                                 ctx.moveTo(0.3 * width, 0.2 * height);
                                                                 ctx.lineTo(0.3 * width, 0.8 * height);
                                                                 ctx.lineTo(0.8 * width, 0.5 * height);
                                                                 ctx.closePath();
                                                                 ctx.fill();
                                                                         } else { // Draw stop icon when recording
                                                                 ctx.fillStyle = "red";

                                                                             ctx.fillRect(0.3 * width, 0.25 * height, 0.45 * width, 0.5 * height);
                                                                         }

                                                         }
                                                     }
                                                    MouseArea
                                                        {
                                                            anchors.fill: parent
                                                            hoverEnabled: true
                                                            onClicked:
                                                            {
                                                                //console.log(start_or_stop)
                                                                start_or_stop = !start_or_stop
                                                                start_stop_recording.contentItem.requestPaint();

                                                                if(start_or_stop)
                                                                {
                                                                    record_tool_text="Stop and\nSave Recording"
                                                                statusindicator.color="darkgreen";
                                                                VideoStreamer.start_recording();
                                                                statusindicator.visible=true
                                                                start_Timer.start()
                                                                    pause_recording.visible=true
                                                                }

                                                                else if(!start_or_stop)
                                                                {
                                                                    record_tool_text="Start\nVideo Recording"
                                                                    statusindicator.color="red"
                                                                    VideoStreamer.stop_recording();
                                                                    //console.log("Recording stopped");
                                                                    start_Timer.elapsedTime=0
                                                                    timerText.text="00:00:00"
                                                                    statusindicator.visible=false
                                                                    pause_recording.visible=false
                                                                    start_Timer.stop()


                                                                }

                                                            }
                                                            onPressed:
                                                                    {
                                                                        start_status = !start_status
                                                                    }

                                                            onReleased:
                                                                    {
                                                                        start_status = !start_status
                                                                    }

                                                            onEntered:
                                                            {

                                                               tooltip_set(start_stop_recording.x+mouseX,start_stop_recording.y-0.05*root.height,record_tool_text)
                                                            }

                                                            onExited:
                                                            {
                                                                tooltip_reset()
                                                            }
                                                        }
                                                }

                                            Button
                                                {
                                                    id:pause_recording
                                                    width:pause_recording.visible?0.03*root.width:0
                                                    height:0.03*root.width
                                                    visible: false
                                                    //Layout.alignment: visible?Qt.AlignHCenter:0
                                                    anchors
                                                    {
                                                        top:main_rect.top
                                                        topMargin:0.005*root.width
                                                        horizontalCenter:main_rect.horizontalCenter
                                                        horizontalCenterOffset:pause_recording.visible==false?0:+0.25*main_rect.width
                                                    }
                                                    background: Rectangle
                                                    {
                                                        anchors.fill:parent
                                                        color:"grey"//"#011026"
                                                        opacity:0.5
                                                        radius:100
                                                    }
                                                    contentItem: Canvas {
                                                        id:pause_rec
                                                        anchors.fill: parent
                                                        contextType: "2d"
                                                         anchors.centerIn: parent

                                                         onPaint: {
                                                             var ctx = getContext("2d");
                                                             ctx.clearRect(0, 0, width, height); // Clear the canvas
                                                             ctx.fillStyle = "#eadeda";

                                                             if (!pause_play) {
                                                                 ctx.fillStyle = "#eadeda";
                                                             ctx.fillRect(0.30 * width, 0.2 * height, 0.15 * width, 0.6 * height);
                                                             ctx.fillRect(0.55 * width, 0.2 * height, 0.15 * width, 0.6 * height);
                                                                         }
                                                             else
                                                             {
                                                                 ctx.fillRect(0.30 * width, 0.2 * height, 0.15 * width, 0.6 * height);

                                                                 ctx.beginPath();
                                                                 ctx.moveTo(0.5 * width, 0.2 * height);
                                                                 ctx.lineTo(0.5 * width, 0.8 * height);
                                                                 ctx.lineTo(0.8 * width, 0.5 * height);
                                                                 ctx.closePath();
                                                                 ctx.fill();
                                                             }

                                                         }
                                                     }
                                                    MouseArea
                                                        {
                                                            anchors.fill: parent
                                                            hoverEnabled: true

                                                            onClicked:
                                                            {
                                                                pause_play = !pause_play
                                                                pause_recording.contentItem.requestPaint();
                                                                if(pause_play)
                                                                {
                                                                    record_tool_text2="Play Recording"
                                                                    start_Timer.stop()
                                                                    VideoStreamer.pause_streaming()
                                                                    //start_Timer2.start()
                                                                    statusindicator.color="red"


                                                                }
                                                                else if(!pause_play)
                                                                {
                                                                    record_tool_text2="Pause Recording"
                                                                    start_Timer.start()
                                                                    VideoStreamer.pause_streaming()
                                                                    //start_Timer2.stop()
                                                                    statusindicator.stopBlinkAnimation()
                                                                    statusindicator.color="green"
                                                                }
                                                            }
                                                            onPressed:
                                                                    {
                                                                        pause_status = !pause_status
                                                                    }

                                                            onReleased:
                                                                    {
                                                                        pause_status = !pause_status
                                                                    }

                                                            onEntered:
                                                            {
                                                                tooltip_set(pause_recording.x+mouseX,pause_recording.y-0.05*root.height,record_tool_text2)
                                                            }

                                                            onExited:
                                                            {
                                                                tooltip_reset()
                                                            }
                                                        }
                                                }

                    Button
                        {
                            id: capture_ss
                            width:0.03*root.width
                            height:0.03*root.width
                            //Layout.alignment: Qt.AlignHCenter
                            visible: true
                            background: Rectangle
                            {
                                anchors.fill:parent
                                color:"grey"//"#011026"
                                opacity:0.5
                                radius:100
                            }
                            anchors
                            {
                                top:start_stop_recording.bottom
                                topMargin:0.01*root.width
                                horizontalCenter:main_rect.horizontalCenter
                                //bottom:box_5.bottom
                                //bottomMargin:0.005*parent.width
                                horizontalCenterOffset:-0.32*main_rect.width
                            }

                            contentItem: Image {
                                source: "qrc:/dvr_system/images/screenshot3.png"
                                anchors.fill: parent
                            }
                            MouseArea
                            {
                                id:area_10
                                anchors.fill: parent
                                hoverEnabled: true

                                onClicked:
                                {
                                    capture_map_image()
                                }
                                onEntered:
                                {
                                    tooltip_set(capture_ss.x+mouseX,capture_ss.y-capture_ss.height,"Capture the\n Frame")
                                }

                                onExited:
                                {
                                    tooltip_reset()
                                }

                            }
                        }

                    Button
                        {
                            id: video_on_off
                            width:0.03*root.width
                            height:0.03*root.width
                            //Layout.alignment: Qt.AlignHCenter
                            visible: true
                            background: Rectangle
                            {
                                anchors.fill:parent
                                color:"grey"//"#011026"
                                opacity:0.5
                                radius:100
                            }
                            anchors
                            {
                                top:start_stop_recording.bottom
                                topMargin:0.01*root.width
                                horizontalCenter:main_rect.horizontalCenter
                                //bottom:box_5.bottom
                                //bottomMargin:0.005*parent.width
                                horizontalCenterOffset:+0.32*main_rect.width
                            }
                            contentItem: Image {
                                id:area_11
                                source: "qrc:/dvr_system/images/video_hide.png"
                                width:0.75*parent.width
                                height:0.75*parent.width
                                anchors.centerIn: parent
                                //clip: true
                                //smooth: true
                            }
                            MouseArea
                            {
                                anchors.fill: parent
                                hoverEnabled: true

                                onClicked:
                                {
                                    //capture_map_image()
                                    if(area_11.source=="qrc:/dvr_system/images/video_show.png")
                                    {
                                        area_11.source="qrc:/dvr_system/images/video_hide.png"
                                        imageRect2.visible=true
                                    }

                                    else
                                    {
                                        area_11.source="qrc:/dvr_system/images/video_show.png"
                                        imageRect2.visible=false
                                    }
                                }
                                onEntered:
                                {
                                    tooltip_set(video_on_off.x+mouseX,video_on_off.y-video_on_off.height,"Turn on\n Web Cam")
                                }

                                onExited:
                                {
                                    tooltip_reset()
                                }

                            }
                        }

                    Button
                        {
                            id: mike_on_off
                            width:0.03*root.width
                            height:0.03*root.width
                            //Layout.alignment: Qt.AlignHCenter
                            visible: true

                            property bool talk:false
                            focus: true  // Ensure this item can receive keyboard events

                            background: Rectangle
                            {
                                anchors.fill:parent
                                color:mike_on_off.talk?"red":"grey"//"#011026"
                                opacity:mike_on_off.talk?1.0:0.5
                                radius:100
                            }
                            anchors
                            {
                                top:start_stop_recording.bottom
                                topMargin:0.01*root.width
                                horizontalCenter:main_rect.horizontalCenter
                            }
                            contentItem: Image {
                                id:area_12
                                source: "qrc:/dvr_system/images/mike_off.png"
                                width:0.9*parent.width
                                height:0.9*parent.width
                                anchors.centerIn: parent
                            }
                            MouseArea
                            {
                                anchors.fill: parent
                                hoverEnabled: true

                                onClicked:
                                {

                                    //capture_map_image()
                                    if(area_12.source=="qrc:/dvr_system/images/mike_off.png")
                                    {
                                        //VideoStreamer.push_to_talk(false)
                                        VideoStreamer.pushback(false)
                                        area_12.source="qrc:/dvr_system/images/mike_on.png"
                                    }

                                    else
                                    {
                                    }
                                }
                                onEntered:
                                {
                                    tooltip_set(mike_on_off.x+mouseX,mike_on_off.y-mike_on_off.height,"Push to Talk")
                                }

                                onExited:
                                {
                                    tooltip_reset()
                                }

                                onDoubleClicked:
                                {
                                    if(area_12.source=="qrc:/dvr_system/images/mike_on.png")
                                    {
                                        mike_on_off.talk=false
                                        area_12.source="qrc:/dvr_system/images/mike_off.png"
                                        VideoStreamer.pushback(true)

                                    }


                                }


                            }
                        }
}


