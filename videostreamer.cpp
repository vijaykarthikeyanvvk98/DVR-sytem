#include "videostreamer.h"
#include <QDateTime>
#include <QFileInfo>
#include <QStandardPaths>
#include "opencv2/imgproc.hpp"
#include "qdebug.h"
#include <QDir>
#include "opencv2/imgproc.hpp"
#include "videostreamer.h"
#include <opencv2/core.hpp> // Basic OpenCV structures (cv::Mat)
#include <opencv2/opencv.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp> // Video write
#include <opencv2/videoio/videoio.hpp>
static VideoWriter received_video; /**< TODO: describe */
static VideoWriter video, writing_video; /**< TODO: describe */
static Mat resized_overlay_frame2; /**< TODO: describe */
static bool recording_status = false; /**< TODO: describe */
static bool exit_status=false,exit_status2=false; /**< TODO: describe */

static Mat frame,received_frame; /**< TODO: describe */
QString dst_file = "received_video.mp4 "; /**< TODO: describe */
QString outputPath,outputPath2; /**< TODO: describe */
//Filter parameters
//Kernel Size(only Odd and Positive for Gaussian Filter) or Filtering pixel Size for Bilateral Filter

using namespace cv;
cv::Mat push_frame; /**< TODO: describe */


static int second_frame_width = 0; /**< TODO: describe */
static int second_frame_height=0; /**< TODO: describe */
static cv::VideoCapture cap,cap2; /**< TODO: describe */
//Sensors
QTimer tUpdate; /**< TODO: describe */
QTimer sub_timer; /**< TODO: describe */
QString streaming_path = ""; /**< TODO: describe */
std::string outputPathStdString=""; /**< TODO: describe */
VideoStreamer *worker2; /**< TODO: describe */
VideoStreamer *worker ; /**< TODO: describe */
static int sub_i=1; /**< TODO: describe */
static int sub_i2=2; /**< TODO: describe */
static int sub_heading=1; /**< TODO: describe */
static int frame_width=0; /**< TODO: describe */
static int frame_height=0; /**< TODO: describe */
const QString ASCII_ART = R"( /**< TODO: describe */
        ##.   ##.          ##             .## .###           .#####.          .###
         ###  ###           ##             .##.##             .##  ##.        ####=
          ##  ##            ##             .####              .######        ##. ##
          .####.            ##             .#####             .##.###       .#######
           ####             ##              ## .##.            ##  ##+      ##.   ##
                ###########=##-###########*......#+##########**##:=+##+
         ###                                #####-                               .###
          #.##*                          ###.#### ##.                          ##.##
          .#  .##.                    ###   # #.##  ###                     ###  .#
           ##    .##               .##     #  #. ##    ###               .##    .#
            ##      ###         .##.     .#   #.  ##      ##:         .##.     *#
             ##        ##.    ###        #.   #.   #.       .##     ##.       .#
              ##         .####          ##    #.    #.         #####          #.
               ##          ####        ##     #.     #.       .###           #.
                ##         #-  ###    ##      #.      #     ##   #          #.
                 #.        ##     ##:##       #.      .#.###     #         ##
                  #.       ##       ###       #.      ###        #        ##
                   #.      ##      ##  ###    #.   .##  .#       #       ##
                   .#      ##     .#      ### #..##.     ##      #      ##
                    :#     ##    .#          ###.         ##     #     .#
                     .#    ##   .#         #######.        ##    #    ##
                      .#   ##   #.       ##   #.  .##       ##   #   =#
                       ##  ##  ##     ##:     #.     ###     ##  #   #
                        ## #* #.   ##*        #.        ##.   ## #  #
                         #####. =##           #.          .##  #.#.#
                          ######              #.             ######.
                                ##########################:
                            ####.             #.             ####-
                             #   ###          #.         ###.  ##
                             .#     .###      #.     .###     ##
                              .#        ###.  #.  ###.       ##
                                #           #####.          ##
                                ##            #.           .#
                                 ##           #.           #
                                  ##          #.         .#
                                   ##         #.         #.
                                    ##        #.        #.
                                     ##       #.       #.
                                      #.      #.      #
                                       #.     #.     ##
                                        #.    #.    ##
                                         #    #.   ##
                                         .#   #.  ##
                                          ##  #. .#
                                           *# #.-#
                                            #####
                                             ###
)";
QStringList lines = ASCII_ART.split('\n'); /**< TODO: describe */
int startTime = 0; /**< TODO: describe */
int duration = 1000; // 1 second per line /**< TODO: describe */
static int gh=0; /**< TODO: describe */
GstElement *volume=nullptr; /**< TODO: describe */

/**
 * @brief
 *
 */
VideoStreamer::VideoStreamer()
{

    gst_init(nullptr, nullptr);

    connect(&tUpdate, &QTimer::timeout, this, &VideoStreamer::streamVideo);
    connect(&tUpdate2, &QTimer::timeout, this, &VideoStreamer::streamVideo2);
    connect(this, &VideoStreamer::stop, this, &VideoStreamer::sendEOS);

    connect(&sub_timer, &QTimer::timeout, this, &VideoStreamer::subtitle_streaming);

}

/**
 * @brief
 *
 */
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

    recording_status=false;

    if (cap.isOpened())
        cap.release();
    if (cap2.isOpened())
        cap2.release();
    sendEOS();

    if(subtitleFile.isOpen())
    {
        sub_timer.stop();
        out.flush();
        subtitleFile.close();
    }
}

/**
 * @brief
 *
 */
void VideoStreamer::streamVideo()
{
    if (!frame.empty()) {
        QImage img = QImage(frame.data, frame.cols, frame.rows, QImage::Format_RGB888).rgbSwapped();
        emit newImage(img);

    } else {
        qDebug() << "Frame empty";
    }
}

/**
 * @brief
 *
 * @param emittedFrame
 */
void VideoStreamer::catchFrame(cv::Mat emittedFrame)
{
    frame = emittedFrame;
}

/**
 * @brief
 *
 */
void VideoStreamer::streamVideo2()
{

    if (!received_frame.empty()) {
        QImage img = QImage(received_frame.data,
                            received_frame.cols,
                            received_frame.rows,
                            QImage::Format_RGB888)
                         .rgbSwapped();
        emit newImage2(img);
    } else {
        //qDebug() << "Frame 2 empty";
    }
}

/**
 * @brief
 *
 * @param emittedFrame
 */
void VideoStreamer::catchFrame2(Mat emittedFrame)
{
    received_frame = emittedFrame;
    //qDebug()<<recording_status;
    if(recording_status)
    {
        pushFrame(received_frame);
    }

}

/**
 * @brief
 *
 * @param path
 */
void VideoStreamer::createPipeline(const QString path)
{
    //qDebug()<<path;
    pipeline = gst_pipeline_new("pipeline");

    /* ================= VIDEO SOURCE ================= */

    appsrc = gst_element_factory_make("appsrc", "video_source");
    if (!appsrc) {
        qCritical() << "Failed to create appsrc";
        return;
    }

    g_object_set(appsrc,
                 "format", GST_FORMAT_TIME,
                 NULL, nullptr);


    /* ================= AUDIO SOURCE ================= */

    GstElement *audioSource = gst_element_factory_make("autoaudiosrc", "audio_source");

    /* ================= AUDIO PROCESS ================= */

    GstElement *audioConvert = gst_element_factory_make("audioconvert", "audio_convert");
    GstElement *audioResample = gst_element_factory_make("audioresample", "audio_resample");
    GstElement *audioEnc = gst_element_factory_make("avenc_aac", "audio_enc");
    volumeElement = gst_element_factory_make("volume", "volume");
    if (!volumeElement) {
        qCritical() << "Failed to create volume element";
        return;
    }
    GstElement *audioQueue3 = gst_element_factory_make("queue", "audio_queue3");
    GstElement *audioQueue4 = gst_element_factory_make("queue", "audio_queue4");
    g_object_set(audioSource, "sync", TRUE, NULL, nullptr);
    g_object_set(volumeElement, "volume", 0.0, NULL, nullptr);
    /* ================= VIDEO PROCESS ================= */

    GstElement *videoConvert = gst_element_factory_make("videoconvert", "video_convert");
    GstElement *i420CapsFilter = gst_element_factory_make("capsfilter", "i420_capsfilter");
    GstElement *videoQueue = gst_element_factory_make("queue", "video_queue");
    GstElement *videoEnc = gst_element_factory_make("x264enc", "video_enc");

    /* ================= MUX & SINK ================= */

    GstElement *muxer = gst_element_factory_make("mp4mux", "muxer");
    GstElement *sink = gst_element_factory_make("filesink", "file_sink");


    GstElement *amplify = gst_element_factory_make("audioamplify", "amplify");
    GstElement *limit = gst_element_factory_make("audiocheblimit", "audiocheblimit");
    GstElement *dynamic = gst_element_factory_make("audiodynamic", "dynamic");
    GstElement *level = gst_element_factory_make("level", "level");
    GstElement *resample2 = gst_element_factory_make("audioresample", "resample2");
    GstElement *convert2 = gst_element_factory_make("audioconvert", "convert2");
    GstElement *volume = gst_element_factory_make("volume", "volume");
    GstElement *filter = gst_element_factory_make("audiowsinclimit", "audiowsinclimit");
    GstElement *equalizer = gst_element_factory_make("equalizer-10bands", "equalizer");

    g_object_set(sink, "location", path.toStdString().c_str(), NULL, nullptr);
    g_object_set(muxer, "faststart", TRUE, NULL, nullptr);

    g_object_set(equalizer,
                 "band0", -20.0,  // 31.25 Hz
                 "band1", -20.0,  // 62.5 Hz
                 "band2", -20.0,   // 125 Hz
                 "band3", -20.0,   // 250 Hz
                 "band4", -20.0,   // 500 Hz
                 "band5", -20.0,   // 1 kHz
                 "band6", 0.0,  // 2 kHz
                 "band7", 5.0,  // 4 kHz
                 "band8", 0.0,   // 8 kHz
                 "band9", 5.0,   // 16 kHz
                 NULL, nullptr);


    //g_object_set(audioEnc, "bitrate", 44100, NULL, nullptr);
    g_object_set(amplify, "amplification", 7.0, NULL, nullptr);
    /* ================= CAPS ================= */

    GstCaps *videoCaps = gst_caps_new_simple(
        "video/x-raw",
        "format", G_TYPE_STRING, "BGR",
        "width", G_TYPE_INT, 1080,
        "height", G_TYPE_INT, 720,
        "framerate", GST_TYPE_FRACTION, 15, 1,
        NULL, nullptr
        );
    g_object_set(G_OBJECT(appsrc), "caps", videoCaps, NULL, nullptr);
    gst_caps_unref(videoCaps);

    GstCaps *i420Caps = gst_caps_new_simple(
        "video/x-raw",
        "format", G_TYPE_STRING, "I420",
        NULL, nullptr
        );
    g_object_set(G_OBJECT(i420CapsFilter), "caps", i420Caps, NULL, nullptr);

    gst_caps_unref(i420Caps);

    /* ================= ENCODER SETTINGS ================= */

    g_object_set(videoEnc,
                 "bitrate", 4500,
                 "key-int-max", 25,
                 "tune", 4,   // zerolatency
                 NULL);

    g_object_set(audioEnc, "bitrate", 128000, NULL, nullptr);

    g_object_set(audioQueue3, "max-size-time", 5000000000, NULL, nullptr);

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
                     volumeElement,      // 👈 ADD HERE
                     audioEnc,
                     audioQueue4,
                     muxer,
                     sink,
                     NULL, nullptr);

    /* ================= LINK VIDEO ================= */

    if (!gst_element_link_many(appsrc,
                               videoConvert,
                               i420CapsFilter,
                               videoQueue,
                               videoEnc,
                               muxer,
                               NULL, nullptr)) {
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
                               volumeElement,   // 👈 INSERT HERE
                               audioEnc,
                               audioQueue4,
                               muxer,
                               NULL, nullptr)) {
        qCritical() << "Failed to link audio pipeline";
        gst_object_unref(pipeline);
        pipeline = nullptr;
        return;
    }
    if (!gst_element_link(muxer, sink)) {
        qCritical() << "Failed to link muxer and sink";
        gst_object_unref(pipeline);
        pipeline = nullptr;
        return;
    }
    /* ================= BUS ================= */

    GstBus *bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline));
    gst_bus_add_watch(bus, bus_call, this);
    gst_object_unref(bus);
}

/**
 * @brief
 *
 */
void VideoStreamer::startPipeline()
{
    if(pipeline)
    {
        //qDebug()<<"Pipeline created";
    }
    else
        qDebug()<<"Pipeline is not created";

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
    //is_subttitle(true);

}

/**
 * @brief
 *
 */
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
            gst_element_set_state(pipeline, GST_STATE_PAUSED);
            gst_element_set_state(pipeline, GST_STATE_READY);
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

}

/**
 * @brief
 *
 */
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

}

/**
 * @brief
 *
 * @param push_frame
 */
void VideoStreamer::pushFrame(Mat push_frame)
{
    // Convert Mat to GstBuffer
    if (!pipeline || !appsrc) {
        qCritical() << "Pipeline or appsrc is null, cannot push frame";
        return;
    }
    GstBuffer *buffer;
    GstFlowReturn ret;
    int size = push_frame.total() * push_frame.elemSize();
    //qDebug()<<size;
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

    g_signal_emit_by_name(appsrc, "push-buffer", buffer, &ret);
    gst_buffer_unref(buffer);

    if (ret != GST_FLOW_OK) {
        qCritical() << "Error pushing buffer";
    }

}

/**
 * @brief
 *
 */
void VideoStreamer::reset_pipeline()
{
    //qDebug()<<"recording";
    currentDateTime = QDateTime::currentDateTime();
    formattedTime = currentDateTime.toString("dd.MM.yyyy-hh.mm.ss");
    outputPath = logFileDir+"/"+ QString(formattedTime) + "-"+dst_file;
    outputPath2 = outputPath;

    createPipeline(outputPath);
    startPipeline();

}

/**
 * @brief
 *
 * @param bus
 * @param msg
 * @param data
 * @return gboolean
 */
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

/**
 * @brief
 *
 * @param bus
 * @param msg
 * @param data
 * @return gboolean
 */
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

/**
 * @brief
 *
 * @param value
 */
void VideoStreamer::pushback(bool value)
{
    volumeElement = gst_bin_get_by_name(GST_BIN(pipeline), "volume");
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
        return;
    }
}

/**
 * @brief
 *
 */
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

    //cap2.open("rtsp://admin:Vikra@123@192.168.56.50:554/video/live?channel=1&subtype=0", cv::CAP_FFMPEG);
    cap2.open("rtsp://admin:Vikra@123@192.168.2.3:554/video/live?channel=1&subtype=0", cv::CAP_FFMPEG);

    //"rtsp://admin:vikra@123@192.168.56.51:554/cam/realmonitor?channel=1&subtype=0"
    //cap2.open("rtspsrc  location=rtsp://192.168.56.50:554/cam/realmonitor?channel=1&subtype=0 user-id=admin user-pw=vikra@123 latency=0 ! decodebin !  videoconvert ! appsink udpsrc port=5202 buffer-size=524288 ! application/x-rtp,media=audio,clock-rate=48000,encoding-name=OPUS,payload=96 ! rtpopusdepay ! opusdec ! audioconvert ! autoaudiosink ",cv::CAP_GSTREAMER);

    if (!cap2.isOpened()) {
        qDebug() << "Error opening received video stream or file";
    }

    second_frame_width = cap2.get(CAP_PROP_FRAME_WIDTH);
    second_frame_height= cap2.get(CAP_PROP_FRAME_HEIGHT);


    VideoStreamer *worker = new VideoStreamer();
    worker->moveToThread(threadStreamer);
    QObject::connect(threadStreamer, SIGNAL(started()), worker, SLOT(streamerThreadSlot()));
    QObject::connect(worker, &VideoStreamer::emitThreadImage, this, &VideoStreamer::catchFrame);
    QObject::connect(worker, &VideoStreamer::emitThreadImage2, this, &VideoStreamer::catchFrame2);
    //qDebug() << cap.get(cv::CAP_PROP_FPS);
    threadStreamer->start();
    //tUpdate.start();
    if (1000 / cap.get(cv::CAP_PROP_FPS) > sizeof(double))
        tUpdate.start(1000.0 / 30.0);
    else
        tUpdate.start(1000.0 / cap.get(cv::CAP_PROP_FPS));

    if (cap2.get(cv::CAP_PROP_FPS) == 0)
    {
        tUpdate2.start(1000 / 40);
        fps = 25;

    }
    else
    {
        tUpdate2.start(1000 / cap2.get(cv::CAP_PROP_FPS));
        fps = cap2.get(cv::CAP_PROP_FPS);
    }
}

/**
 * @brief
 *
 * @param path
 */
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
    cap2.open("rtsp://admin:Vikra@123@192.168.56.50:554/video/live?channel=1&subtype=0", cv::CAP_FFMPEG);
    //cap2.open("rtsp://admin:Vikra@123@192.168.2.3:554/video/live?channel=1&subtype=0", cv::CAP_FFMPEG);

    //"rtsp://admin:vikra@123@192.168.56.51:554/cam/realmonitor?channel=1&subtype=0"
    //cap2.open("rtspsrc  location=rtsp://192.168.56.50:554/cam/realmonitor?channel=1&subtype=0 user-id=admin user-pw=vikra@123 latency=0 ! decodebin !  videoconvert ! appsink udpsrc port=5202 buffer-size=524288 ! application/x-rtp,media=audio,clock-rate=48000,encoding-name=OPUS,payload=96 ! rtpopusdepay ! opusdec ! audioconvert ! autoaudiosink ",cv::CAP_GSTREAMER);

    if (!cap2.isOpened()) {
        qDebug() << "Error opening received video stream or file";
    }

    second_frame_width = cap2.get(CAP_PROP_FRAME_WIDTH);
    second_frame_height= cap2.get(CAP_PROP_FRAME_HEIGHT);

    worker2->moveToThread(threadStreamer2);
    QObject::connect(threadStreamer2, SIGNAL(started()), worker2, SLOT(streamerThreadSlot2()));
    QObject::connect(worker2, &VideoStreamer::emitThreadImage2, this, &VideoStreamer::catchFrame2);

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

}

/**
 * @brief
 *
 */
void VideoStreamer::start_recording()
{
    reset_pipeline();
}

/**
 * @brief
 *
 */
void VideoStreamer::streamerThreadSlot()
{
    cv::Mat tempFrame,tempFrame2;

    while (!threadStreamer->isInterruptionRequested()) {
        if(exit_status)
        {
            return;
        }

        cap >> tempFrame;
        cap2 >> tempFrame2;

        if (tempFrame.data || tempFrame2.data) {
            emit emitThreadImage(tempFrame);
            emit emitThreadImage2(tempFrame2);

        }

        if (threadStreamer->isInterruptionRequested()) {
            return;
        }

    }
}

/**
 * @brief
 *
 */
void VideoStreamer::streamerThreadSlot2()
{

    cv::Mat tempFrame;
    if (!cap2.isOpened()) {
        //cap2.release();
        return;
    }

    while (!threadStreamer2->isInterruptionRequested()) {

        if (QThread::currentThread()->isInterruptionRequested()) {
            cap2.release();

            return;
        }
        cap2 >> tempFrame;
        if (tempFrame.data) {
            emit emitThreadImage2(tempFrame);
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
    }

}

/**
 * @brief
 *
 */
void VideoStreamer::stop_recording()
{
    recording_status = false;
    emit stop();
}

/**
 * @brief
 *
 */
void VideoStreamer::pause_streaming()
{
    recording_status = !recording_status;
}

/**
 * @brief
 *
 */
void VideoStreamer::create_directory()
{
    // Get the target directory path for saving files.
    QString targetDirectory = QStandardPaths::writableLocation(QStandardPaths::MoviesLocation);
    // Modify the target directory to add a subdirectory for your files.
    logFileDir = targetDirectory + "/Leo Recordings";

    // Create the directory if it doesn't exist.
    QDir().mkpath(logFileDir);

    // Get the target directory path for saving files.
    QString targetDirectory2 = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
    // Modify the target directory to add a subdirectory for your files.
    logFileDir2 = targetDirectory2 + "/Leo Images";

    // Create the directory if it doesn't exist.
    QDir().mkpath(logFileDir2);

    // Get the target directory path for saving files.
    QString targetDirectory3 = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    // Modify the target directory to add a subdirectory for your files.
    logFileDir3 = targetDirectory3 + "/Leo Data";

    // Create the directory if it doesn't exist.
    //QDir().mkpath(logFileDir3);

    // Get the target directory path for saving files.
    QString targetDirectory4 = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    // Modify the target directory to add a subdirectory for your files.
    logFileDir4 = targetDirectory4 + "/UWC test";

    // Create the directory if it doesn't exist.
    //QDir().mkpath(logFileDir4);
}


/**
 * @brief
 *
 */
void VideoStreamer::subtitle_streaming()
{

    if (subtitleFile.isOpen())
    {

        // Write subtitle to the .ass file for all four corners
        out << "Dialogue: 0,00:00:" + QString::number(sub_i) + ".00,00:00:" + QString::number(sub_i2) + ".00,Default,,0,0,0,,"
            << "{\\pos(50,20)}" // Top Left
            << "{\\fs" + QString::number(10) + "}"
            << /*live_time*/ + "\n";
        gh=0;

        sub_i++;
        sub_i2++;
    }
}


void VideoStreamer::is_subttitle(bool value)
{
    //qDebug()<<value;
    if(value)
    {
        if(subtitleFile.isOpen())
        {
            sub_timer.stop();
            out.flush();
            subtitleFile.close();
            sub_i=0;
            sub_i2=0;
        }
        QDateTime currentDateTime = QDateTime::currentDateTime();
        formattedTime = currentDateTime.toString("dd.MM.yyyy-hh.mm.ss");

        QString outputPath2 = logFileDir + "/Recording-" + formattedTime + ".ass";
        subtitleFile.setFileName(outputPath2);

        outputPathStdString= outputPath.toStdString();
        if (subtitleFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            out.setDevice(&subtitleFile);
            // Write the ASS file header and styles
            out << "[Script Info]\n";
            out << "Title: Example ASS File\n";
            out << "Original Script: OpenAI\n";
            out << "ScriptType: v4.00+\n";
            out << "Collisions: Normal\n";
            out << "PlayDepth: 0\n\n";

            out << "[V4+ Styles]\n";
            out << "Format: Name, Fontname, Fontsize, PrimaryColour, SecondaryColour, BackColour, Bold, Italic, BorderStyle, Outline, Shadow, Alignment, MarginL, MarginR, MarginV, Encoding\n";
            out << "Style: Default,Arial,20,&H00FFFFFF,&H00000000,&H00000000,-1,0,1,1.0,0.0,2,10,10,10,1\n\n";

            out << "[Events]\n";
            out << "Format: Marked, Start, End, Style, Name, MarginL, MarginR, MarginV, Effect, Text\n";
        }
        sub_timer.start();
    }
    else
        ;
}

