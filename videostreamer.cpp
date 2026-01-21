#include "videostreamer.h"
#include <QDateTime>
#include <QCoreApplication>
#include <QDir>
#include <QElapsedTimer>
#include <QFileInfo>
#include <QStandardPaths>
#include "opencv2/imgproc.hpp"
#include "qdebug.h"
#include <chrono>
#include <cmath>
//#include <filesystem>
#include <QDateTime>
#include <QDir>
#include <QElapsedTimer>
#include <QFileInfo>
#include <QStandardPaths>
#include "opencv2/imgproc.hpp"
#include "qdebug.h"
#include "videostreamer.h"
#include <chrono>
#include <cmath>
#include <iostream>
#include <opencv2/core.hpp> // Basic OpenCV structures (cv::Mat)
#include <opencv2/gapi.hpp>
#include <opencv2/gapi/gstreaming.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp> // Video write
//#include <filesystem>#include <iostream>
#include <opencv2/core.hpp> // Basic OpenCV structures (cv::Mat)
#include <opencv2/gapi.hpp>
#include <opencv2/gapi/gstreaming.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp> // Video write
#include <opencv2/videoio/videoio.hpp>
//#include <videoenhancement/include/videoenhancement.h>
//#include "colorcorrection/include/colorcorrection.h"
#include <QKeyEvent>
#include <QCoreApplication>
#include <QProcess>
//#include "audiorecorder.h"
static VideoWriter received_video;
//AudioRecorder recorder;
static VideoWriter video, writing_video, sending_video, video2;
static Mat image_out,resized_overlay_frame2;
static bool recording_status = false,recording_status2 = false;
static bool exit_status=false,exit_status2=false;
static double writing_fps = 0.0,writing_fps2 = 0.0;
qint64 elapsedMilliseconds;
QDateTime recordingStartTime;
int hours,minutes,seconds;
static double amount = 1, bright = 1;
static bool original_not = false;
static bool push_back = false;

static Mat frame;
QString dst_file = "received_video.mp4 ";
QString outputPath,outputPath2;
static int writing_delay2=0;
// Global QTextStream pointer for logging
QTextStream* logStream = nullptr;
int elapsedSeconds ;
//ColorCorrection color_correction;

//Filter parameters
static int filter_pixel_size
    = 13; //Kernel Size(only Odd and Positive for Gaussian Filter) or Filtering pixel Size for Bilateral Filter
static int sigma_x = 25; // Spatial Deviation in (X-direction) for Gaussian Filter and Bilateral Filter
static int sigma_y = 25;     // Spatial Deviation in (Y-direction) for Gaussian Filter
static int sigma_color = 80; //Pixel Intensity Factor for Bilateral Filter
QElapsedTimer timer_1;
using namespace cv;
QElapsedTimer timer;
cv::Mat push_frame;

//AHRS
static qfloat16 yaw1 = 0.0;
static qfloat16 pitch1 = 0.0;
static qfloat16 roll1 = 0.0;
static qfloat16 yaw2 = 0;
static qfloat16 pitch2 = 0;
static qfloat16 roll2 = 0;
int battery_percentage=0;
//Sensors
static qfloat16 press = 0;
static qfloat16 temp = 0;
static qfloat16 depth = 0;

static int second_frame_width = 0;
static int second_frame_height=0;
static cv::VideoCapture cap, writing_cap, cap2,cap3;
GstElement *volume=nullptr;
//VideoEnhancement video_enhance;
VideoStreamer::VideoStreamer()
{
    //start_script();
    //qDebug()<<recording_status;
    gst_init(nullptr, nullptr);
    // Create the GStreamer elements
    /*GstElement *src = gst_element_factory_make("udpsrc", "src");
    GstElement *rtpopusdepay = gst_element_factory_make("rtpopusdepay", "rtpopusdepay");
    GstElement *opusdec = gst_element_factory_make("opusdec", "opusdec");
    GstElement *audioresample = gst_element_factory_make("audioresample", "audioresample");
    GstElement *audioconvert = gst_element_factory_make("audioconvert", "audioconvert");
    GstElement *equalizer = gst_element_factory_make("equalizer-10bands", "equalizer");
    GstElement *sink = gst_element_factory_make("wasapisink", "sink");
    volume = gst_element_factory_make("volume", "volume");
    GstElement *filter = gst_element_factory_make("audiocheblimit", "audiocheblimit");
    GstElement *amplify = gst_element_factory_make("audioamplify", "amplify");

    g_object_set(volume, "volume", 10.0, NULL);

   //g_object_set(filter, "window", 1, NULL);
    g_object_set(filter, "cutoff", 400.0, NULL, nullptr);
   //g_object_set(filter, "length", 101, NULL, nullptr);
    g_object_set(filter, "mode", 0, NULL, nullptr);
   // Configure the equalizer bands
    g_object_set(amplify, "amplification", 10.0, NULL);

   g_object_set(equalizer,
                "band0", -20.0,  // 31.25 Hz
                "band1", -20.0,  // 62.5 Hz
                "band2", -20.0,   // 125 Hz
                "band3", -20.0,   // 250 Hz
                "band4", -20.0,   // 500 Hz
                "band5", -20.0,   // 1 kHz
                "band6", 0.0,  // 2 kHz
                "band7", 10.0,  // 4 kHz
                "band8", 0.0,   // 8 kHz
                "band9", 10.0,   // 16 kHz
                NULL);



    GstElement *capsfilter = gst_element_factory_make("capsfilter", "capsfilter");

    // Check if all elements were created
    if (!src || !rtpopusdepay || !opusdec || !audioresample || !audioconvert ||  !sink || !capsfilter || !amplify) {
        g_printerr("Failed to create GStreamer elements\n");
        return;
    }

    // Set the port for the udpsrc element
    g_object_set(src, "port", 8889, NULL);

    // Set the caps filter properties
    GstCaps *caps = gst_caps_new_simple(
        "application/x-rtp",
        "media", G_TYPE_STRING, "audio",
        "clock-rate", G_TYPE_INT, 48000,
        "encoding-name", G_TYPE_STRING, "OPUS",
        "payload", G_TYPE_INT, 96,
        NULL
        );
    g_object_set(capsfilter, "caps", caps, NULL);
    gst_caps_unref(caps);

    // Create the empty pipeline
    pipeline2 = gst_pipeline_new("audio-pipeline");

    if (!pipeline2) {
        g_printerr("Failed to create pipeline\n");
        return;
    }*/

    // Build the pipeline
    /* gst_bin_add_many(GST_BIN(pipeline2), src, capsfilter, rtpopusdepay, opusdec, audioresample,  audioconvert,equalizer, filter, amplify, volume,sink, nullptr);
    if (gst_element_link_many(src,capsfilter,  rtpopusdepay, opusdec, audioresample,audioconvert,equalizer, filter,amplify, volume,sink, nullptr) != TRUE) {
        g_printerr("Failed to link elements\n");
        gst_object_unref(pipeline2);
        return;
    }*/
    // Add elements to the pipeline
    /*gst_bin_add_many(GST_BIN(pipeline2), src, audioconvert, audioresample, sink, nullptr);

    // Link the elements together
    if (!gst_element_link_many(src, audioconvert, audioresample, sink, nullptr)) {
        g_printerr("Elements could not be linked.\n");
        gst_object_unref(pipeline2);
    }


    // Start playing the pipeline
    gst_element_set_state(pipeline2, GST_STATE_PLAYING);
    GstBus *bus = gst_element_get_bus(pipeline2);
    gst_bus_add_signal_watch(bus);

    g_signal_connect(bus, "message", G_CALLBACK(on_message), nullptr);*/
    /*pipeline2 = gst_pipeline_new("audio-receive-pipeline");
    GstElement *udpsrc = gst_element_factory_make("udpsrc", "source");
    GstElement *audioconvert = gst_element_factory_make("audioconvert", "convert");
    GstElement *audioresample = gst_element_factory_make("audioresample", "resample");
    volumeElement = gst_element_factory_make("volume", "volume");
    GstElement *autoaudiosink = gst_element_factory_make("autoaudiosink", "sink");

    if (!udpsrc || !audioconvert || !audioresample || !volumeElement || !autoaudiosink) {
        g_printerr("Not all elements could be created.\n");
        return;
    }

    // Set the properties for the udpsrc element
    g_object_set(udpsrc, "port", 8889, nullptr);

    // Set the caps for udpsrc
    GstCaps *caps = gst_caps_new_simple("audio/x-raw",
                                        "format", G_TYPE_STRING, "S16LE",
                                        "channels", G_TYPE_INT, 1,
                                        "rate", G_TYPE_INT, 44100,
                                        nullptr);
    g_object_set(udpsrc, "caps", caps, nullptr);
    gst_caps_unref(caps);
    g_object_set(volumeElement, "volume", 1.0, nullptr);
    // Add elements to the pipeline
    gst_bin_add_many(GST_BIN(pipeline2), udpsrc, audioconvert, audioresample, volumeElement,  autoaudiosink, nullptr);

    // Link the elements together
    if (!gst_element_link_many(udpsrc, volumeElement, audioconvert, audioresample, autoaudiosink, nullptr)) {
        g_printerr("Elements could not be linked.\n");
        gst_object_unref(pipeline2);
    }

    // Start playing the pipeline
    GstStateChangeReturn ret = gst_element_set_state(pipeline2, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE) {
        g_printerr("Pipeline could not be started.\n");
        gst_object_unref(pipeline2);
        return ;
    }*/
    //gst_element_set_state(pipeline, GST_STATE_NULL);
    //gst_object_unref(pipeline);
    // Wait until error or EOS
    //GstBus *bus = gst_element_get_bus(pipeline);
    //GstMessage *msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, (GstMessageType)(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));

    // Parse the message
    /*if (msg != nullptr) {
        GError *err;
        gchar *debug_info;

        switch (GST_MESSAGE_TYPE(msg)) {
        case GST_MESSAGE_ERROR:
            gst_message_parse_error(msg, &err, &debug_info);
            g_printerr("Error received from element %s: %s\n", GST_OBJECT_NAME(msg->src), err->message);
            g_printerr("Debugging information: %s\n", debug_info ? debug_info : "none");
            g_clear_error(&err);
            g_free(debug_info);
            break;
        case GST_MESSAGE_EOS:
            g_print("End-Of-Stream reached.\n");
            break;
        default:
            // Should not reach here
            g_printerr("Unexpected message received.\n");
            break;
        }
        gst_message_unref(msg);
    }*/

    // Free resources
    //gst_object_unref(bus);

    connect(&tUpdate, &QTimer::timeout, this, &VideoStreamer::streamVideo);
    connect(&tUpdate2, &QTimer::timeout, this, &VideoStreamer::streamVideo2);
    connect(&check, &QTimer::timeout, this, &VideoStreamer::stream_check);
    connect(this, &VideoStreamer::stop, this, &VideoStreamer::sendEOS);
    //connect(&tUpdate3, &QTimer::timeout, this, &VideoStreamer::pushFrame);

    //check.start(1000);


    connect(&timer_5,&QTimer::timeout,this,&VideoStreamer::write_size);

    //qImageToCvMat();
    //"qrc:/resources/images/vikra_2.jpeg"
    // Convert QImage to cv::Mat
    //cv::Mat resized_overlay_frame2 = QImageToCvMat(qOverlay);
    //resized_overlay_frame2 = cv::imread("../resources/images/vikra_2.jpeg", cv::IMREAD_COLOR);
    /*AudioRecorder = new QAudioRecorder(this);
    AudioProbe    = new QAudioProbe(this);

    AudioProbe->setSource(AudioRecorder);

    QAudioEncoderSettings Settings;
    Settings.setCodec("audio/pcm");
    Settings.setBitRate(0);
    Settings.setSampleRate(0);
    Settings.setChannelCount(-1);
    Settings.setQuality(QMultimedia::EncodingQuality::VeryHighQuality);
    Settings.setEncodingMode(QMultimedia::ConstantQualityEncoding);
    QString Container="audio/x-raw";
    AudioRecorder->setOutputLocation(QUrl::fromLocalFile(QCoreApplication::applicationDirPath()+"test.wav"));
    AudioRecorder->setEncodingSettings(Settings,QVideoEncoderSettings(),Container);*/

    //QTimer::singleShot(10000, this, &VideoStreamer::sendEOS);

}

VideoStreamer::~VideoStreamer()
{
    exit_status=true;
    //exit_status2=true;
    tUpdate.stop();
    tUpdate2.stop();

    threadStreamer->requestInterruption();
    threadStreamer2->requestInterruption();

    if (threadStreamer->isRunning()) {
        threadStreamer->requestInterruption();
        threadStreamer->quit();
        threadStreamer->wait(100);
    }
    if (threadStreamer2->isRunning()) {
        threadStreamer2->requestInterruption();
        threadStreamer2->quit();
        threadStreamer2->wait(100);
    }


    if (video.isOpened())
        video.release();
    if (sending_video.isOpened())
        sending_video.release();
    recording_status=false;
    /*if (received_video.isOpened())
        received_video.release();*/
    if (cap.isOpened())
        cap.release();
    if (cap2.isOpened())
        cap2.release();
    sendEOS();
}

void VideoStreamer::streamVideo()
{
    //qDebug() << "Outside" << recording_status;
    if (!frame.empty()) {
        QImage img = QImage(frame.data, frame.cols, frame.rows, QImage::Format_RGB888).rgbSwapped();
        emit newImage(img);

    } else {
        qDebug() << "Frame empty";
    }
}

void VideoStreamer::catchFrame(cv::Mat emittedFrame)
{
    frame = emittedFrame;
}

cv::Mat VideoStreamer::write_frame(cv::Mat dummy_frame)
{
    newFrame = dummy_frame.clone();
    cx = (newFrame.cols - 70) / 2;
    cx2 = (newFrame.cols - 150) / 2;
    /*FPS = "FPS: " + QString::number(writing_fps, 'f', 1);

    //AHRS

    YAW = "H:" + QString::number(yaw2, 'f', 1);
    PITCH = "Pitch:" + QString::number(pitch2, 'f', 2);
    ROLL = "Roll:" + QString::number(roll2, 'f', 2);
    PRESSURE = "P:" + QString::number(press, 'f', 2)+("mBar");
    DEPTH = "D:" + QString::number(depth, 'f', 2)+("m");
    TEMP = "T:" + QString::number(temp, 'f', 2);
    BATTERY = "B:" + QString::number(battery_percentage)+"%";
    cv::putText(dummy_frame,
                FPS.toStdString(),
                Point(10, 20),
                cv::QT_FONT_NORMAL,
                0.5,
                cv::Scalar(0, 255, 0), // BGR for green
                1,cv::LINE_AA);
    cv::putText(dummy_frame,
                YAW.toStdString(),
                Point(100, 20),
                cv::QT_FONT_NORMAL,
                0.5,
                cv::Scalar(0, 255, 0), // BGR for green
                1,cv::LINE_AA);
    cv::putText(dummy_frame,
                PRESSURE.toStdString(),
                Point(175, 20),
                cv::QT_FONT_NORMAL,
                0.5,
                cv::Scalar(0, 255, 0), // BGR for green
                1,cv::LINE_AA);
    cv::putText(dummy_frame,
                DEPTH.toStdString(),
                Point(310, 20),
                cv::QT_FONT_NORMAL,
                0.5,
                cv::Scalar(0, 255, 0), // BGR for green
                1,cv::LINE_AA);
    cv::putText(dummy_frame,
                TEMP.toStdString(),
                Point(400, 20),
                cv::QT_FONT_NORMAL,
                0.5,
                cv::Scalar(0, 255, 0), // BGR for green
                1,cv::LINE_AA);*/
    /*cv::putText(dummy_frame,
                BATTERY.toStdString(),
                Point(470, 20),
                cv::QT_FONT_NORMAL,
                0.5,
                cv::Scalar(0, 255, 0), // BGR for green
                1,cv::LINE_AA);*/
    if(!frame.empty())
    {
        cv::resize(frame, resized_overlay_frame, cv::Size(150, 100)); // Adjust the size as needed
        cv::resize(resized_overlay_frame2, resized_overlay_frame3, cv::Size(50, 50)); // Adjust the size as needed

        // Overlay the resized frame onto the main frame at the desired position
        // For example, overlay at the top-left corner (adjust position as needed)
        cv::Rect roi(cv::Point(second_frame_width-180, second_frame_height-110), resized_overlay_frame.size());
        cv::Rect roi2(cv::Point(0,second_frame_height-50), resized_overlay_frame3.size());
        // Now check the number of channels in src and dst
        //int srcChannels = resized_overlay_frame3.channels();
        //int dstChannels = dummy_frame.channels();

        // Print the number of channels
        //std::cout << "Source image channels: " << srcChannels << std::endl;
        //std::cout << "Destination image channels: " << dstChannels << std::endl;
        resized_overlay_frame3.copyTo(dummy_frame(roi2));
        resized_overlay_frame.copyTo(dummy_frame(roi));
    }

    return dummy_frame;
}

void VideoStreamer::streamVideo2()
{

    if (!received_frame.empty()) {
        /*
            img2.setTo(cv::Scalar(0, 0, 0,0));
            // Calculate the center point of the line
            cv::Point center((startPoint.x + endPoint.x) / 2, (startPoint.y + endPoint.y) / 2);
            double radians = angle * CV_PI / 180.0;
            int newStartX = center.x + (startPoint.x - center.x) * cos(radians) - (startPoint.y - center.y) * sin(radians);
            int newStartY = center.y + (startPoint.x - center.x) * sin(radians) + (startPoint.y - center.y) * cos(radians);
            int newEndX = center.x + (endPoint.x - center.x) * cos(radians) - (endPoint.y - center.y) * sin(radians);
            int newEndY = center.y + (endPoint.x - center.x) * sin(radians) + (endPoint.y - center.y) * cos(radians);
            // Create a rotation matrix
            cv::Mat rotationMatrix = cv::getRotationMatrix2D(center, angle, 1.0);
            cv::line(img2, cv::Point(newStartX, newStartY), cv::Point(newEndX, newEndY), color, thickness);
            img2.copyTo(frame);
            angle += 1.0;*/
        //drawing_frame = received_frame.clone();

        // Draw the original line
        //cv::line(frame, startPoint, endPoint, color, thickness);
        // Apply the rotation to the line
        //cv::warpAffine(frame, frame, rotationMatrix, frame.size(), cv::INTER_LINEAR, cv::BORDER_CONSTANT, cv::Scalar(0, 0, 0));


        //if (received_video.isOpened())
        //received_video.write(received_frame);

        /*if(recording_status)
            pushFrame(received_frame);*/
        QImage img = QImage(received_frame.data,
                            received_frame.cols,
                            received_frame.rows,
                            QImage::Format_RGB888)
                         .rgbSwapped();
        emit newImage2(img);
        //
        //qDebug()<<"Frame-2";


    } else {
        //qDebug() << "Frame 2 empty";
    }
}

void VideoStreamer::catchFrame2(Mat emittedFrame)
{
    received_frame = emittedFrame;
    //qDebug()<<recording_status;
    if(recording_status)
    {
        pushFrame(received_frame);
    }

}

void VideoStreamer::logMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    if (logStream) {
        switch (type) {
        case QtDebugMsg:
            *logStream << "Debug: " << msg << Qt::endl;
            break;
        case QtInfoMsg:
            *logStream << "Info: " << msg << Qt::endl;
            break;
        case QtWarningMsg:
            *logStream << "Warning: " << msg << Qt::endl;
            break;
        case QtCriticalMsg:
            *logStream << "Critical: " << msg << Qt::endl;
            break;
        case QtFatalMsg:
            *logStream << "Fatal: " << msg << Qt::endl;
            abort();
        }
        logStream->flush();
    }
}

void VideoStreamer::setupLogging(const QString &logFilePath)
{
    static QFile logFile(logFilePath);
    if (logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        logStream = new QTextStream(&logFile);
        qInstallMessageHandler(logMessageHandler);
    } else {
        qWarning() << "Failed to open log file for writing:" << logFilePath;
    }
}

void VideoStreamer::createPipeline(const QString path)
{
    pipeline = gst_pipeline_new("pipeline");

    /* ================= VIDEO SOURCE ================= */

    appsrc = gst_element_factory_make("appsrc", "video_source");
    if (!appsrc) {
        qCritical() << "Failed to create appsrc";
        return;
    }

    g_object_set(G_OBJECT(appsrc),
                 "format", GST_FORMAT_TIME,
                 "is-live", TRUE,
                 "do-timestamp", TRUE,
                 NULL);

    /* ================= AUDIO SOURCE ================= */

    GstElement *audioSource = gst_element_factory_make("autoaudiosrc", "audio_source");

    /* ================= AUDIO PROCESS ================= */

    GstElement *audioConvert = gst_element_factory_make("audioconvert", "audio_convert");
    GstElement *audioResample = gst_element_factory_make("audioresample", "audio_resample");
    GstElement *audioEnc = gst_element_factory_make("avenc_aac", "audio_enc");

    GstElement *audioQueue3 = gst_element_factory_make("queue", "audio_queue3");
    GstElement *audioQueue4 = gst_element_factory_make("queue", "audio_queue4");

    /* ================= VIDEO PROCESS ================= */

    GstElement *videoConvert = gst_element_factory_make("videoconvert", "video_convert");
    GstElement *i420CapsFilter = gst_element_factory_make("capsfilter", "i420_capsfilter");
    GstElement *videoQueue = gst_element_factory_make("queue", "video_queue");
    GstElement *videoEnc = gst_element_factory_make("x264enc", "video_enc");

    /* ================= MUX & SINK ================= */

    GstElement *muxer = gst_element_factory_make("mp4mux", "muxer");
    GstElement *sink = gst_element_factory_make("filesink", "file_sink");

    g_object_set(sink, "location", path.toStdString().c_str(), NULL);
    g_object_set(muxer, "faststart", TRUE, NULL);

    /* ================= CAPS ================= */

    GstCaps *videoCaps = gst_caps_new_simple(
        "video/x-raw",
        "format", G_TYPE_STRING, "BGR",
        "width", G_TYPE_INT, 1080,
        "height", G_TYPE_INT, 720,
        "framerate", GST_TYPE_FRACTION, 25, 1,
        NULL
        );
    g_object_set(G_OBJECT(appsrc), "caps", videoCaps, NULL);
    gst_caps_unref(videoCaps);

    GstCaps *i420Caps = gst_caps_new_simple(
        "video/x-raw",
        "format", G_TYPE_STRING, "I420",
        NULL
        );
    g_object_set(G_OBJECT(i420CapsFilter), "caps", i420Caps, NULL);
    gst_caps_unref(i420Caps);

    /* ================= ENCODER SETTINGS ================= */

    g_object_set(videoEnc,
                 "bitrate", 4500,
                 "key-int-max", 25,
                 "tune", 4,   // zerolatency
                 NULL);

    g_object_set(audioEnc, "bitrate", 128000, NULL);

    g_object_set(audioQueue3, "max-size-time", 5000000000, NULL);

    /* ================= ADD TO PIPELINE ================= */

    gst_bin_add_many(GST_BIN(pipeline),
                     appsrc,
                     videoConvert,
                     i420CapsFilter,
                     videoQueue,
                     videoEnc,

                     audioSource,
                     audioConvert,
                     audioResample,
                     audioQueue3,
                     audioEnc,
                     audioQueue4,
                     muxer,
                     sink,
                     NULL);

    /* ================= LINK VIDEO ================= */

    if (!gst_element_link_many(appsrc,
                               videoConvert,
                               i420CapsFilter,
                               videoQueue,
                               videoEnc,
                               muxer,
                               NULL)) {
        qCritical() << "Failed to link video pipeline";
        gst_object_unref(pipeline);
        pipeline = nullptr;
        return;
    }

    /* ================= LINK AUDIO ================= */

    if (!gst_element_link_many(audioSource,
                               audioConvert,
                               audioResample,
                               audioQueue3,
                               audioEnc,
                               audioQueue4,
                               muxer,
                               NULL)) {
        qCritical() << "Failed to link audio pipeline";
        gst_object_unref(pipeline);
        pipeline = nullptr;
        return;
    }

    /* ================= BUS ================= */

    GstBus *bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline));
    gst_bus_add_watch(bus, bus_call, this);
    gst_object_unref(bus);
}

void VideoStreamer::startPipeline()
{
    //qDebug()<<recording_status;
    /*currentDateTime = QDateTime::currentDateTime();
    formattedTime = currentDateTime.toString("dd.MM.yyyy-hh.mm.ss");
    //qDebug()<<formattedTime;
    outputPath = logFileDir+"/"+ QString(formattedTime) + "-"+dst_file;
    createPipeline(outputPath);*/

    if(pipeline)
    {
        //qDebug()<<"Pipeline created";
    }
    GstStateChangeReturn ret2 = gst_element_set_state(pipeline, GST_STATE_READY);
    if(ret2 == GST_STATE_CHANGE_FAILURE)
        qCritical() << "Failed to ready pipeline";

    GstStateChangeReturn ret = gst_element_set_state(pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE) {
        qCritical() << "Failed to start pipeline";
    }
    else
    {
        //qCritical() << "success to start pipeline";

    }

    //qDebug()<<"This one";
    recording_status = true;
    exit_status2=false;
    recordingStartTime = QDateTime::currentDateTime();

    //timer_5.start(5000);  // 1800000 milliseconds = 30 minutes

}

void VideoStreamer::sendEOS()
{



    if (pipeline) {
        //qDebug() << "EOS started";
        // Add debug statements before and after sending EOS
        //gst_element_send_event(this->pipeline, gst_event_new_eos());
        GstElement *video_src = gst_bin_get_by_name(GST_BIN(pipeline), "video_source");
        g_assert(video_src);
        GstElement *audio_src = gst_bin_get_by_name(GST_BIN(pipeline), "audio_source");
        g_assert(audio_src);
        gst_element_send_event(video_src, gst_event_new_eos());
        gst_element_send_event(audio_src, gst_event_new_eos());

        //qDebug() << "EOS ended";
        //qDebug() << "EOS events sent and sources unreferenced";
        gst_object_unref(video_src);
        gst_object_unref(audio_src);
        video_src=nullptr;
        audio_src=nullptr;
        QTimer::singleShot(1000, [this]() {
            //gst_element_set_state(pipeline, GST_STATE_PAUSED);
            //gst_element_set_state(pipeline, GST_STATE_READY);
            gst_element_set_state(pipeline, GST_STATE_NULL);
            //gst_element_set_state(pipeline2, GST_STATE_NULL);
            gst_object_unref(pipeline);
            pipeline = nullptr;
            emit recording_stop();
            //gst_object_unref(pipeline2);
            //pipeline2 = nullptr;
            //qDebug() << "Pipeline state set to NULL after EOS";
        });
    }
    else
    {
        //qDebug()<<"No EOS";
    }

    timestamp=0;
    //timer_5.stop();

}

void  VideoStreamer::qImageToCvMat()
{
    QString imagePath(":/dvr_system/images/vikra_2.png");
    QImage qImage;
    if (!qImage.load(imagePath)) {
        qDebug() << "Failed to load image from resources";
    }
    QImage::Format format = qImage.format();
    cv::Mat mat(qImage.height(), qImage.width(), CV_8UC4, const_cast<uchar*>(qImage.bits()), qImage.bytesPerLine());
    //cv::Mat mat = imread(imagePath.toStdString(),cv::IMREAD_UNCHANGED);
    cvtColor(mat, mat, COLOR_RGBA2RGB); // Convert RGBA to RGB

    resized_overlay_frame2 = mat.clone();

    /*if(resized_overlay_frame2.data)
    {
        qDebug()<<"Has Data";
        qDebug()<<resized_overlay_frame2.size().height;
    }*/
}

void VideoStreamer::write_size()
{
    outputPath2 = outputPath2.trimmed();

    QFileInfo fileInfo(outputPath2);
    //qDebug()<<outputPath2;
    if (fileInfo.exists() && fileInfo.isFile()) {
        //qDebug()<<"Yes file is there";
        qint64 fileSize = fileInfo.size();
        QString formattedSize = formatFileSize(fileSize);

        QDateTime currentDateTime = QDateTime::currentDateTime();
        QString dateTimeString = currentDateTime.toString("yyyy-MM-dd hh:mm:ss");
        elapsedMilliseconds = recordingStartTime.msecsTo(QDateTime::currentDateTime()); // Replace recordingStartTime with actual start time
        elapsedSeconds = elapsedMilliseconds / 1000;

        hours = elapsedSeconds / 3600;
        minutes = (elapsedSeconds % 3600) / 60;
        seconds = elapsedSeconds % 60;

        QString formattedTime2 = QString("%1:%2:%3").arg(hours, 2, 10, QChar('0'))
                                     .arg(minutes, 2, 10, QChar('0'))
                                     .arg(seconds, 2, 10, QChar('0'));
        QString Path = logFileDir4 +"/"+QString(formattedTime) + "-"+"check.csv";
        QFile logFile(Path);
        if (logFile.open(QIODevice::Append | QIODevice::Text)) {
            QTextStream out(&logFile);
            out << formattedTime2 << "," << formattedSize << "\n";
            out.flush();
            logFile.close();
        } else {
            qDebug() << "Failed to open log file.";
        }
    } else {
        qDebug()<<"no file is  not there";

        qDebug() << "File does not exist or is not a regular file.";
    }
}

QString VideoStreamer::formatFileSize(qint64 size)
{
    double convertedSize = size;
    QString suffix = " bytes";

    if (convertedSize > 1024) {
        convertedSize /= 1024;
        suffix = " KB";
    }
    if (convertedSize > 1024) {
        convertedSize /= 1024;
        suffix = " MB";
    }
    if (convertedSize > 1024) {
        convertedSize /= 1024;
        suffix = " GB";
    }
    //qDebug()<<convertedSize;
    return QString::number(convertedSize, 'f', 2) + suffix;
}

void VideoStreamer::pushFrame(Mat push_frame)
{
    //qDebug()<<fps;
    //push_frame = normal_frame2;
    // Convert Mat to GstBuffer
    if (!pipeline || !appsrc) {
        qCritical() << "Pipeline or appsrc is null, cannot push frame";
        return;
    }
    GstBuffer *buffer;
    GstFlowReturn ret;
    int size = push_frame.total() * push_frame.elemSize();
    buffer = gst_buffer_new_allocate(NULL, size, NULL);
    if (!buffer) {
        qCritical() << "Failed to create GstBuffer";
        return;
    }
    gst_buffer_fill(buffer, 0, push_frame.data, size);

    // Create a new GstCaps to describe the buffer
    GstCaps *caps = gst_caps_new_simple("video/x-raw",
                                        "format", G_TYPE_STRING, "BGR",
                                        "width", G_TYPE_INT, push_frame.cols,
                                        "height", G_TYPE_INT, push_frame.rows,
                                        "framerate", GST_TYPE_FRACTION, fps, 1,
                                        NULL);
    if (!caps) {
        qCritical() << "Failed to create GstCaps";
        gst_buffer_unref(buffer);
        return;
    }
    gst_app_src_set_caps(GST_APP_SRC(appsrc), caps);
    gst_caps_unref(caps);

    // Set the timestamp
    GST_BUFFER_PTS(buffer) = timestamp;
    GST_BUFFER_DTS(buffer) = GST_CLOCK_TIME_NONE;
    timestamp += gst_util_uint64_scale_int(1, GST_SECOND, fps);

    // Push the buffer into the appsrc element
    /*if(recording_status)
        qDebug()<<"Frame";*/
    g_signal_emit_by_name(appsrc, "push-buffer", buffer, &ret);
    gst_buffer_unref(buffer);

    if (ret != GST_FLOW_OK) {
        qCritical() << "Error pushing buffer";
    }

}

void VideoStreamer::reset_pipeline()
{
    //qDebug()<<"recording";
    currentDateTime = QDateTime::currentDateTime();
    formattedTime = currentDateTime.toString("dd.MM.yyyy-hh.mm.ss");
    outputPath = logFileDir+"/"+ QString(formattedTime) + "-"+dst_file;
    outputPath2 = outputPath;
    /*gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
    pipeline = nullptr;
    if(pipeline==nullptr)
        qDebug()<<"Pipeline is closed";

    GstState state;
    GstState pending;
    GstStateChangeReturn ret = gst_element_get_state(pipeline, &state, &pending, GST_CLOCK_TIME_NONE);
    if(state == GST_STATE_NULL)
        qDebug()<<"Not playing";*/

    createPipeline(outputPath);
    startPipeline();
}

gboolean VideoStreamer::bus_call(GstBus *bus, GstMessage *msg, gpointer data)
{
    VideoStreamer *videoStream = static_cast<VideoStreamer*>(data);

    switch (GST_MESSAGE_TYPE(msg)) {
    case GST_MESSAGE_EOS:
        qDebug() << "End of stream";

        //gst_element_set_state(GST_ELEMENT(data), GST_STATE_NULL);
        break;

    case GST_MESSAGE_ERROR:
        GError *err;
        gchar *debug;
        gst_message_parse_error(msg, &err, &debug);
        qCritical() << "Error:" << err->message;
        g_error_free(err);
        g_free(debug);
        gst_element_set_state(videoStream->pipeline, GST_STATE_NULL);
        break;

    default:
        break;
    }
    return TRUE;
}

gboolean VideoStreamer::on_message(GstBus *bus, GstMessage *msg, gpointer data)
{
    switch (GST_MESSAGE_TYPE(msg)) {
    case GST_MESSAGE_ELEMENT: {
        if (gst_message_has_name(msg, "level")) {
            const GstStructure *s = gst_message_get_structure(msg);
            // Parse and use the level data
            double rms_dB;
            if (gst_structure_get_double(s, "rms", &rms_dB)) {
                g_print("RMS level: %.2f dB\n", rms_dB);
            }
        }
        break;
    }
    default:
        break;
    }
    return TRUE;
}

void VideoStreamer::pushback(bool value)
{
    volumeElement = gst_bin_get_by_name(GST_BIN(pipeline2), "volume");
    if (volumeElement != nullptr) {
        if (value) {
            // Mute audio
            g_object_set(volumeElement, "volume", 0.0, NULL, nullptr);
        } else {

            // Unmute audio
            g_object_set(volumeElement, "volume", 1.0, NULL, nullptr);
        }

        g_object_unref(volumeElement);
    } else {
        qDebug() << "Volume element not found!";
    }
}

void VideoStreamer::start_script()
{
    QStringList arguments;
    QString path2 = QCoreApplication::applicationDirPath();
    QProcess *process = new QProcess();
    QString scriptPath =path2 + "/audio.py";

    arguments<<scriptPath;
    process->startDetached("python", arguments, path2);
    process->waitForFinished();
    if (!process->waitForFinished()) {
        qDebug() << "Error: " << process->errorString();
    } else {
        qDebug() << "Output: " << process->readAllStandardOutput();
        qDebug() << "Error Output: " << process->readAllStandardError();

    }
    process->close();
}

void VideoStreamer::stop_script()
{
    QProcess process;

    // Use the full path to taskkill to avoid any PATH issues
    QString program = "C:\\Windows\\System32\\taskkill.exe";
    QStringList arguments;
    arguments << "/F" << "/IM" << "python.exe";

    process.start(program, arguments);

    if (!process.waitForStarted()) {
        qDebug() << "Failed to start taskkill process:" << process.errorString();
        return;
    }

    if (!process.waitForFinished()) {
        qDebug() << "Failed to finish taskkill process:" << process.errorString();
    } else {
        QString output = process.readAllStandardOutput();
        QString error = process.readAllStandardError();

        qDebug() << "taskkill Output:" << output;
        if (!error.isEmpty()) {
            qDebug() << "taskkill Error Output:" << error;
        }
    }
}

void VideoStreamer::push_to_talk(bool status)
{
    gst_element_set_state(pipeline2, GST_STATE_PAUSED);
    if(status)
    {
        g_object_set(volumeElement, "volume", 0.1 , nullptr); // Mute if status is true, unmute otherwise
        start_script();
    }
    else
    {
        g_object_set(volumeElement, "volume",1.0, nullptr); // Mute if status is true, unmute otherwise
        stop_script();
    }
    gst_element_set_state(pipeline2, GST_STATE_PLAYING);
}

Mat VideoStreamer::apply_enhance(Mat enhancing_frame)
{
    //image_out = video_enhance.colorcorrection(enhancing_frame,127.5);
    return image_out;
}

void VideoStreamer::openVideoCamera()
{
    int i=0;
    for(;i<100;i++)
        if(cap.open(i))
            break;
    //cap.open(0);

    if (!cap.isOpened()) {
        qDebug() << "Error opening video stream or file";
        //return;
    }

    if (!sending_video.isOpened()) {
        /*sending_video
            .open("appsrc ! videoconvert ! "
                  "video/x-raw,format=YUY2,width=640,height=480,framerate=30/1 ! jpegenc ! "
                  "rtpjpegpay ! appsink   autoaudiosrc ! audioconvert "
                  "! audioresample ! opusenc ! rtpopuspay ! udpsink host=192.168.56.1 port=5204  ",
                  CAP_GSTREAMER,
                  cv::VideoWriter::fourcc('M', 'J', 'P', 'G'),  // fourcc
                  30.0, // fps
                  Size(640, 480),
                  true);
        /*video.open("appsrc ! videoconvert ! "
                   "video/x-raw,format=YUY2,width=640,height=480,framerate=30/1 ! jpegenc ! "
                   "rtpjpegpay ! appsink  autoaudiosrc ! audioconvert "
                   "! audioresample ! opusenc ! rtpopuspay ! udpsink host=192.168.56.1 port=5204  ",
                   CAP_GSTREAMER,
                   cv::VideoWriter::fourcc('M', 'J', 'P', 'G'),  // fourcc
                   30.0, // fps
                   Size(640, 480),
                   true);*/
    }

    VideoStreamer *worker = new VideoStreamer();
    worker->moveToThread(threadStreamer);
    QObject::connect(threadStreamer, SIGNAL(started()), worker, SLOT(streamerThreadSlot()));
    QObject::connect(worker, &VideoStreamer::emitThreadImage, this, &VideoStreamer::catchFrame);
    //qDebug() << cap.get(cv::CAP_PROP_FPS);
    threadStreamer->start();
    //tUpdate.start();
    if (1000 / cap.get(cv::CAP_PROP_FPS) > sizeof(double))
        tUpdate.start(1000 / 30);
    else
        tUpdate.start(1000 / cap.get(cv::CAP_PROP_FPS));
}

void VideoStreamer::openVideoCamera2(QString path)
{
    VideoStreamer *worker2 = new VideoStreamer();

    //qDebug()<<path;
    if(cap2.isOpened())
    {
        tUpdate2.stop();

        cap2.release();
        tUpdate.stop();

        cap2.release();
        worker2->deleteLater();
        threadStreamer2->quit();
        threadStreamer2->wait();
        threadStreamer2->deleteLater();
    }
    //if (!cap2.isOpened()) {
    //qDebug()<<path;

    //cap2.open(path.toStdString());
    //cap2.open(1);
    cap2.open("rtsp://admin:Vikra@123@192.168.56.50:554/video/live?channel=1&subtype=0", cv::CAP_FFMPEG);
    //"rtsp://admin:vikra@123@192.168.56.51:554/cam/realmonitor?channel=1&subtype=0"
    //cap2.open("rtspsrc  location=rtsp://192.168.56.50:554/cam/realmonitor?channel=1&subtype=0 user-id=admin user-pw=vikra@123 latency=0 ! decodebin !  videoconvert ! appsink udpsrc port=5202 buffer-size=524288 ! application/x-rtp,media=audio,clock-rate=48000,encoding-name=OPUS,payload=96 ! rtpopusdepay ! opusdec ! audioconvert ! autoaudiosink ",cv::CAP_GSTREAMER);

    int frame_width = cap2.get(cv::CAP_PROP_FRAME_WIDTH);
    int frame_height = cap2.get(cv::CAP_PROP_FRAME_HEIGHT);
    /*if(received_video.isOpened())
            received_video.release();*/
    /*currentDateTime = QDateTime::currentDateTime();
        formattedTime = currentDateTime.toString("dd.MM.yyyy-hh.mm.ss");
        outputPath = logFileDir+"/"+ QString(formattedTime) + "-"+dst_file;

        createPipeline(outputPath);*/
    //QString logFilePath = logFileDir + "/log_" + formattedTime + ".txt";

    //setupLogging(logFilePath);

    if (writing_fps != 0)
        ;
    else
        writing_fps = 30;
    /*if (!received_video.isOpened())
        {
            if(dst_file == "received_video.avi ")
            {
                //dst_file = "received_video.mkv ";
                //outputPath = logFileDir+"/"+ QString(formattedTime) + "-"+dst_file;
                //outputPath = "C:/Users/Vijay/Videos/dst.mkv";

                /*received_video.open("appsrc ! "
                                "video/x-raw,framerate=30/1,width=" + std::to_string(frame_width) + ",height=" + std::to_string(frame_height) + ",format=BGR ! "
                                                                                                                "videoconvert ! "
                                                                                                                "queue ! jpegenc ! queue ! filesink location=" + outputPath.toStdString(),
                                CAP_GSTREAMER,
                                cv::VideoWriter::fourcc('X','V','I','D'),// fourcc
                                30.0, // fps
                                Size(640, 480),
                                true);*/

    /*received_video.open("appsrc ! "
                                    "video/x-raw,format=BGR,width=" + std::to_string(frame_width) + ",height=" + std::to_string(frame_height) + ",framerate=30/1 ! "
                                                                                                                    "videoconvert ! "
                                                                                                                    "queue ! jpegenc ! "
                                                                                                                    "mux. "
                                                                                                                    "wasapisrc low-latency=true ! "
                                                                                                                    "audio/x-raw,format=S16LE,rate=48000,channels=2 ! "
                                                                                                                    "queue ! audioconvert ! queue ! audioresample ! queue ! "
                                                                                                                    "avenc_aac bitrate=32000 ! queue ! "
                                                                                                                    "mux. "
                                                                                                                    "avimux name=mux ! "
                                                                                                                    "filesink location=" + outputPath.toStdString(),
                                    CAP_GSTREAMER,
                                    cv::VideoWriter::fourcc('X','V','I','D'), // fourcc
                                    30.0, // fps
                                    Size(640, 480),
                                    true);

            }

            else if(dst_file=="received_video.mp4 ")
            {
                received_video.open("appsrc ! "
                                    "video/x-raw,framerate=30/1,width="+ std::to_string(frame_width) + ",height=" + std::to_string(frame_height) + ",format=BGR ! "
                                                                                                                    "videoconvert ! "
                                                                                                                    " x264enc ! h264parse ! queue ! avimux name=mux "
                                                                                                                    " autoaudiosrc ! queue ! audioconvert ! queue ! audioresample ! queue ! avenc_aac bitrate=3200 ! queue ! mux. "
                                                                                                                    "mux. ! filesink o-sync=true location="
                                        + outputPath.toStdString(),
                                    CAP_GSTREAMER,
                                    cv::VideoWriter::fourcc('m','p','4','v'),// fourcc
                                    30.0, // fps
                                    Size(640, 480),
                                    true);
            }
            /*received_video.open("C:/Users/vijay/Videos/rv45.mp4",
                            cv::VideoWriter::fourcc('m','p','4','v'),
                            30.0,
                            Size(640,480),
                            true);*/

    /*}

        if (!received_video.isOpened()) {
            std::cerr << "Error: Unable to create the video writer\n";
        }*/

    //}



    if (!cap2.isOpened()) {
        qDebug() << "Error opening received video stream or file";
    }

    second_frame_width = cap2.get(CAP_PROP_FRAME_WIDTH);
    second_frame_height= cap2.get(CAP_PROP_FRAME_HEIGHT);

    //VideoStreamer *worker2 = new VideoStreamer();
    worker2->moveToThread(threadStreamer2);
    QObject::connect(threadStreamer2, SIGNAL(started()), worker2, SLOT(streamerThreadSlot2()));
    QObject::connect(worker2, &VideoStreamer::emitThreadImage2, this, &VideoStreamer::catchFrame2);
    //QObject::connect(worker2, &VideoStreamer::emitThreadImage2, this, &VideoStreamer::pushFrame);

    //qDebug() << cap.get(cv::CAP_PROP_FPS);
    threadStreamer2->start();
    //tUpdate2.start();
    if (cap2.get(cv::CAP_PROP_FPS) == 0)
    {
        tUpdate2.start(1000 / 30);
        fps = 33;

    }
    else
    {
        tUpdate2.start(1000 / cap2.get(cv::CAP_PROP_FPS));
        fps = cap2.get(cv::CAP_PROP_FPS);
    }

    //tUpdate3.start(1000 / cap2.get(cv::CAP_PROP_FPS));
}

void VideoStreamer::start_recording()
{

    //startPipeline();
    reset_pipeline();

    //recorder.toggleRecord();
    //AudioRecorder->record();
}

void VideoStreamer::streamerThreadSlot()
{
    //qDebug()<<"1 thread";

    cv::Mat tempFrame, normal_frame;
    static int frame_count = 0;
    int delay = 40;
    int calculated_difference = 1000 / cap.get(cv::CAP_PROP_FPS);

    if (calculated_difference >= 1000 || calculated_difference <= 0)
        calculated_difference = 1000 / 30;
    if (!cap.isOpened()) {
        cap.release();
        return;
    }
    timer_1.start();
    while (1) {
        if(exit_status)
        {
            //sending_video.release();
            return;
        }


        //qDebug()<<"1 running";
        auto startTime = std::chrono::high_resolution_clock::now();

        if (QThread::currentThread()->isInterruptionRequested()) {
            cap.release();
            return;
        }
        cap >> tempFrame;
        /*frame_count++;
        if (timer_1.elapsed() >= 1000) { // 1000 milliseconds
            writing_fps = frame_count;
            frame_count = 0;
            timer_1.restart(); // Restart the timer for the next second
        }*/
        if (tempFrame.data) {
            emit emitThreadImage(tempFrame);
            //sending_video.write(tempFrame);
            //video.write(tempFrame);

        }

        if (threadStreamer->isInterruptionRequested()) {
            // qDebug() << "Interrupt requested";
            //cap.release();
            //video.release();
            return;
        }

        //normal_frame = tempFrame.clone();
        //emit emitThreadImage2(normal_frame);

        if (tempFrame.data) {
            //histogram_equalization(tempFrame);
        }
        // Record end time
        auto endTime = std::chrono::high_resolution_clock::now();

        // Calculate the actual time taken to process the frame
        int actual_difference
            = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
        //qDebug() << "calculated" << calculated_difference;
        //qDebug() << "actual" << actual_difference;

        int outcome = std::abs(calculated_difference - actual_difference);
        if (actual_difference < calculated_difference)
            delay = outcome + actual_difference;

        else
            delay = actual_difference - outcome;

        if (delay > 1000 || delay < 0)
            delay = 1000 / 30;
        //qDebug() << "delay" << delay;

        //std::this_thread::sleep_for(std::chrono::milliseconds(delay));
        //video.write(sharpened);
    }
}

void VideoStreamer::streamerThreadSlot2()
{

    cv::Mat tempFrame,enhanced,frame2;
    ;
    static int frame_count = 0;
    int delay = 40;
    int calculated_difference = 1000 / cap2.get(cv::CAP_PROP_FPS);

    if (calculated_difference >= 1000 || calculated_difference <= 0)
        calculated_difference = 1000 / 30;

    if (!cap2.isOpened()) {
        //cap2.release();
        return;
    }
    timer.start();

    while (1) {

        //qDebug()<<"2 running";
        auto startTime = std::chrono::high_resolution_clock::now();

        if (QThread::currentThread()->isInterruptionRequested()) {
            cap2.release();

            return;
        }

        /*if(!cap2.isOpened())
            return;*/

        cap2 >> tempFrame;
        frame_count++;
        if (timer.elapsed() >= 1000) { // 1000 milliseconds
            writing_fps = frame_count;
            frame_count = 0;
            timer.restart(); // Restart the timer for the next second
        }
        if (tempFrame.data) {
            //qDebug() << "Frame";
            //qDebug()<<recording_status;
            //enhanced =  color_correction.GWA_Lab(tempFrame);
            tempFrame.convertTo(enhanced, -1, bright, amount); //decrease the brightness
            enhanced = write_frame(enhanced);
            //histogram_equalization(tempFrame);
            normal_frame2 = frame2.clone();



        }

        else {
            //received_video.release();
            qDebug() << "No frame";
        }

        if (threadStreamer2->isInterruptionRequested()) {
            // qDebug() << "Interrupt requested";
            cap2.release();
            //video.release();
            return;
        }
        // Record end time
        auto endTime = std::chrono::high_resolution_clock::now();

        // Calculate the actual time taken to process the frame
        int actual_difference
            = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
        //qDebug() << "calculated" << calculated_difference;
        //qDebug() << "actual" << actual_difference;

        int outcome = std::abs(calculated_difference - actual_difference);
        if (actual_difference < calculated_difference)
            delay = outcome + actual_difference;

        else
            delay = actual_difference - outcome;

        if (delay > 1000 || delay < 0)
            delay = 30;
        //qDebug() << "delay" << delay;
        writing_delay2=delay;
        //if(recording_status)
        //std::this_thread::sleep_for(std::chrono::milliseconds(delay));
        emit emitThreadImage2(tempFrame);

        //video.write(sharpened);
        /*if (recording_status == true && received_video.isOpened()) {
            //qDebug() << "Recording Started";
            //received_video.write(tempFrame);
        }
        else
        {

        }*/
    }
    /*if(threadStreamer2->isFinished())
        qDebug()<<"Finishes";
    else
        qDebug()<<"not finished";*/

    /*while(1)
    {
        cap2>>tempFrame;
        emit emitThreadImage2(tempFrame);
        if(recording_status)
            received_video.write(tempFrame);
    }*/
}

void VideoStreamer::stop_streaming()
{
    cap2.release();
    cap.release();
    tUpdate.stop();
    tUpdate2.stop();
}

void VideoStreamer::stop_recording()
{
    recording_status = false;

    //tUpdate3.stop();
    /*if (received_video.isOpened()) {
        //cap2.release();
        try {
            received_video.release();
        }
        catch (const cv::Exception& ex) {
            // Handle exception
            std::cerr << "Error: " << ex.what() << std::endl;
        }
    }*/
    //exit_status2=true;
    //received_video.release();
    //recorder.toggleStop();
    //AudioRecorder->stop();
    //QTimer::singleShot(2000, this, &VideoStreamer::sendEOS);
    //
    //qDebug()<<"Before";
    emit stop();
    //sendEOS();
    //qDebug()<<"After";
}

void VideoStreamer::pause_streaming()
{
    recording_status = !recording_status;
}

void VideoStreamer::create_directory()
{
    // Get the target directory path for saving files.
    QString targetDirectory = QStandardPaths::writableLocation(QStandardPaths::MoviesLocation);
    // Modify the target directory to add a subdirectory for your files.
    logFileDir = targetDirectory + "/UWC";

    // Create the directory if it doesn't exist.
    QDir().mkpath(logFileDir);

    // Get the target directory path for saving files.
    QString targetDirectory2 = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
    // Modify the target directory to add a subdirectory for your files.
    logFileDir2 = targetDirectory2 + "/UWC Images";

    // Create the directory if it doesn't exist.
    QDir().mkpath(logFileDir2);

    // Get the target directory path for saving files.
    QString targetDirectory3 = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    // Modify the target directory to add a subdirectory for your files.
    logFileDir3 = targetDirectory3 + "/UWC Data";

    // Create the directory if it doesn't exist.
    QDir().mkpath(logFileDir3);

    // Get the target directory path for saving files.
    QString targetDirectory4 = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    // Modify the target directory to add a subdirectory for your files.
    logFileDir4 = targetDirectory4 + "/UWC test";

    // Create the directory if it doesn't exist.
    QDir().mkpath(logFileDir4);
}

void VideoStreamer::set_ahrs(QVariantList array)
{
    //qDebug()<<array;
    yaw2 = array[0].toFloat();
    //pitch1 = array[1].toFloat();
    //roll1 = array[2].toFloat();
    press = array[1].toFloat();
    depth = array[2].toFloat();
    temp = array[3].toFloat();
    battery_percentage = array[4].toInt();

}

void VideoStreamer::histogram_equalization(Mat frame_to_process)
{
    cv::cvtColor(frame_to_process, simg, COLOR_BGR2GRAY);
    long int N = simg.rows * simg.cols;

    int histo_b[256];
    int histo_g[256];
    int histo_r[256];

    for (int i = 0; i < 256; i++) {
        histo_b[i] = 0;
        histo_g[i] = 0;
        histo_r[i] = 0;
    }
    Vec3b intensity;

    for (int i = 0; i < simg.rows; i++) {
        for (int j = 0; j < simg.cols; j++) {
            intensity = frame_to_process.at<Vec3b>(i, j);

            histo_b[intensity.val[0]] = histo_b[intensity.val[0]] + 1;
            histo_g[intensity.val[1]] = histo_g[intensity.val[1]] + 1;
            histo_r[intensity.val[2]] = histo_r[intensity.val[2]] + 1;
        }
    }

    for (int i = 1; i < 256; i++) {
        histo_b[i] = histo_b[i] + 1 * histo_b[i - 1];
        histo_g[i] = histo_g[i] + 1 * histo_g[i - 1];
        histo_r[i] = histo_r[i] + 1 * histo_r[i - 1];
    }

    int vmin_b = 0;
    int vmin_g = 0;
    int vmin_r = 0;
    int s1 = 3;
    int s2 = 3;

    while (histo_b[vmin_b + 1] <= N * s1 / 100) {
        vmin_b = vmin_b + 1;
    }
    while (histo_g[vmin_g + 1] <= N * s1 / 100) {
        vmin_g = vmin_g + 1;
    }
    while (histo_r[vmin_r + 1] <= N * s1 / 100) {
        vmin_r = vmin_r + 1;
    }

    int vmax_b = 255 - 1;
    int vmax_g = 255 - 1;
    int vmax_r = 255 - 1;

    while (histo_b[vmax_b - 1] > (N - ((N / 100) * s2))) {
        vmax_b = vmax_b - 1;
    }
    if (vmax_b < 255 - 1) {
        vmax_b = vmax_b + 1;
    }
    while (histo_g[vmax_g - 1] > (N - ((N / 100) * s2))) {
        vmax_g = vmax_g - 1;
    }
    if (vmax_g < 255 - 1) {
        vmax_g = vmax_g + 1;
    }
    while (histo_r[vmax_r - 1] > (N - ((N / 100) * s2))) {
        vmax_r = vmax_r - 1;
    }
    if (vmax_r < 255 - 1) {
        vmax_r = vmax_r + 1;
    }

    for (int i = 0; i < simg.rows; i++) {
        for (int j = 0; j < simg.cols; j++) {
            intensity = frame_to_process.at<Vec3b>(i, j);

            if (intensity.val[0] < vmin_b) {
                intensity.val[0] = vmin_b;
            }
            if (intensity.val[0] > vmax_b) {
                intensity.val[0] = vmax_b;
            }

            if (intensity.val[1] < vmin_g) {
                intensity.val[1] = vmin_g;
            }
            if (intensity.val[1] > vmax_g) {
                intensity.val[1] = vmax_g;
            }

            if (intensity.val[2] < vmin_r) {
                intensity.val[2] = vmin_r;
            }
            if (intensity.val[2] > vmax_r) {
                intensity.val[2] = vmax_r;
            }

            frame_to_process.at<Vec3b>(i, j) = intensity;
        }
    }

    for (int i = 0; i < simg.rows; i++) {
        for (int j = 0; j < simg.cols; j++) {
            intensity = frame_to_process.at<Vec3b>(i, j);
            intensity.val[0] = (intensity.val[0] - vmin_b) * 255 / (vmax_b - vmin_b);
            intensity.val[1] = (intensity.val[1] - vmin_g) * 255 / (vmax_g - vmin_g);
            intensity.val[2] = (intensity.val[2] - vmin_r) * 255 / (vmax_r - vmin_r);
            frame_to_process.at<Vec3b>(i, j) = intensity;
        }
    }
    // Apply gamma correction (gamma = 0.5 for example)
    //frame_to_process = gamma_correction(frame_to_process, 0.25);

    //qDebug() << bright;
    // sharpen image using "unsharp mask" algorithm
    double sigma = 1, threshold = 10;
    if (selected_filter == 0)
        GaussianBlur(frame_to_process, blurred, Size(1, 1), sigma, sigma);
    else if (selected_filter == 1)
        bilateralFilter(frame_to_process, blurred, 9, 75, 75);
    lowContrastMask = abs(frame_to_process - blurred) < threshold;
    sharpened = frame_to_process * (1 + amount) + blurred * (-amount);
    frame_to_process.copyTo(sharpened, lowContrastMask);
    frame2 = sharpened.clone();
    //emit emitThreadImage(sharpened);
}

Mat VideoStreamer::gamma_correction(Mat frame_to_correct, double gamma)
{
    // Ensure the frame is not empty
    if (!frame_to_correct.empty()) {
        // Convert the frame to a grayscale image
        cv::cvtColor(frame_to_correct, simg, COLOR_BGR2GRAY);

        // Apply gamma correction to each pixel
        for (int i = 0; i < simg.rows; i++) {
            for (int j = 0; j < simg.cols; j++) {
                // Get the pixel intensity
                int intensity = simg.at<uchar>(i, j);

                // Apply gamma correction formula
                double corrected_intensity = pow(intensity / 255.0, gamma) * 255.0;

                // Set the corrected intensity value
                simg.at<uchar>(i, j) = static_cast<uchar>(corrected_intensity);
            }
        }
        return simg;
    }
}

void VideoStreamer::edited_parameters(double brightness, double contrast)
{
    amount = contrast;
    bright = brightness;
}

void VideoStreamer::filter_selection(int option)
{
    selected_filter = option;
}

void VideoStreamer::export_video(QString Path)
{
    recording_status = true;
    // Get the target directory path for saving files.
    QString targetDirectory = QStandardPaths::writableLocation(QStandardPaths::MoviesLocation);
    // Modify the target directory to add a subdirectory for your files.
    QString logFileDir = targetDirectory + "/Processed_Videos";

    // Create the directory if it doesn't exist.
    QDir().mkpath(logFileDir);
    QDateTime currentDateTime = QDateTime::currentDateTime();
    formattedTime = currentDateTime.toString("dd.MM.yyyyhh.mm.ss");

    // Construct the file paths relative to the target directory.
    QString outputPath = logFileDir + Path + ".avi";
    std::string outputPathStdString = Path.toStdString();

    writing_cap.open("file:///C:/Users/hp/Videos/Captures/output.avi");
    if (!cap.isOpened()) {
        qDebug() << "Not opened";
    } else
        qDebug() << "Opened";
    cv::Mat writing_frame;
    int writing_fps = writing_cap.get(cv::CAP_PROP_FPS);
    int calculated_difference = 1000 / writing_fps;
    int writing_delay = 40;
    int frame_width = cap.get(cv::CAP_PROP_FRAME_WIDTH);
    int frame_height = cap.get(cv::CAP_PROP_FRAME_HEIGHT);
    writing_video.open(outputPathStdString,
                       cv::VideoWriter::fourcc('M', 'J', 'P', 'G'),
                       cap.get(cv::CAP_PROP_FPS),
                       Size(frame_width, frame_height),
                       true);
    if (!writing_video.isOpened()) {
        qDebug() << "Error: Could not open the video writer.";
        recording_status = false;
    }

    else {
        while (true) {
            auto startTime = std::chrono::high_resolution_clock::now();
            writing_cap >> writing_frame;
            if (writing_frame.data) {
                //writing_video.write(writing_frame);
            } else {
                writing_video.release();
                writing_cap.release();
                emit writing_success();
                //QFile::remove("C:/Users/hp/Videos/Captures/output.avi");
                return;
            }
            auto endTime = std::chrono::high_resolution_clock::now();
            // Calculate the actual time taken to process the frame
            int actual_difference
                = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
            int outcome = std::abs(calculated_difference - actual_difference);
            if (actual_difference < calculated_difference)
                writing_delay = outcome + actual_difference;

            else
                writing_delay = actual_difference - outcome;

            if (writing_delay > 1000 || writing_delay < 0)
                writing_delay = 40;

            std::this_thread::sleep_for(std::chrono::milliseconds(writing_delay));
        }
    }
}

void VideoStreamer::normal_processed(bool state)
{
    original_not = state;
}

void VideoStreamer::set_filter_parameters(int parameter, int value)
{
    switch (parameter) {
    case 0:
        filter_pixel_size = value;
        break;
    case 1:
        sigma_x = value;
        break;
    case 2:
        sigma_y = value;
        break;
    case 3:
        sigma_color = value;
        break;
    case 4:
        //sigma_y = value;
        break;
    default:
        break;
    }
}

void VideoStreamer::stream_check()
{
    if(cap2.isOpened())
        ;//qDebug()<<"opened";
}
