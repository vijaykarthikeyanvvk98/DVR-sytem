#ifndef VIDEOSTREAMER_H
#define VIDEOSTREAMER_H

#include <QDateTime>
#include <QImage>
#include <QObject>
#include <QThread>
#include <QTimer>
#include <QVariant>
//#include "qaudiorecorder.h"
#include "qfloat16.h"
#include "qthread.h"
#include <iostream>
#include <opencv2/highgui.hpp>
#include <gst/gst.h>
#include <gst/app/gstappsrc.h> // Include this header for appsrc

//#include <QtMultimedia>
//#include <QtCore>
using namespace cv;

class VideoStreamer : public QObject
{
    Q_OBJECT

public:
    VideoStreamer();
    ~VideoStreamer();

public:
    void streamVideo();
    QThread *threadStreamer = new QThread();
    void catchFrame(cv::Mat emittedFrame);
    Mat write_frame(Mat);
    //Received stream
    void streamVideo2();
    QThread *threadStreamer2 = new QThread();
    void catchFrame2(cv::Mat emittedFrame);
    static void logMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);
    void setupLogging(const QString &logFilePath);
    void createPipeline(const QString);
    void startPipeline();
    void pushFrame(Mat);
    void reset_pipeline();
    static gboolean bus_call(GstBus *bus, GstMessage *msg, gpointer data);
    static gboolean on_message(GstBus *bus, GstMessage *msg, gpointer data);

public slots:
    Mat apply_enhance(cv::Mat);
    void openVideoCamera();
    void openVideoCamera2(QString);
    void start_recording();
    void streamerThreadSlot();
    void streamerThreadSlot2();
    void stop_streaming();
    void stop_recording();
    void pause_streaming();
    void create_directory();
    void set_ahrs(QVariantList);
    void histogram_equalization(cv::Mat);
    cv::Mat gamma_correction(Mat, double);
    void edited_parameters(double brightness, double contrast);
    void filter_selection(int);
    void export_video(QString Path);
    void normal_processed(bool);
    void set_filter_parameters(int, int);
    void stream_check();
    void sendEOS();
    void qImageToCvMat();
    void write_size();
    QString formatFileSize(qint64 size);
    void pushback(bool );
    void start_script();
    void stop_script();
    void push_to_talk(bool);

private:
    Mat simg;
    Mat sharpened;
    Mat blurred;
    cv::Mat frame2, imageContrastHigh4, drawing, lowContrastMask, final_image, normal_frame,
        drawing_frame, received_frame, newFrame, dstROI, resized_overlay_frame,normal_frame2,resized_overlay_frame3;
    QTimer tUpdate, tUpdate2,tUpdate3;
    QTimer timer_5;
    int FPS_count = 0;
    int fps, fps_2;
    qint64 lastTimeStamp;
    qint64 currentTime = 0, elapsedTime;
    QString FPS, YAW, PITCH, ROLL, TEMP, PRESSURE, DEPTH, BATTERY;
    int thickness = 4;
    int frame_width = 0;
    int frame_height = 0;
    int cx = 0,cx2=0;
    // Rotation angle in degrees
    double angle = 15.0;

    int enhance_value = 1;
    int delay_2 = 40;
    int calculated_difference = 0;
    int start_time, end_time;
    QDateTime date;
    QString formattedTime;
    QByteArray formattedTimeMsg;
    //parameters

    int selected_filter = 0;

    QString logFileDir = "",logFileDir2 = "",logFileDir3 = "",logFileDir4="";
    QDateTime currentDateTime;

    QTimer check;

    Rect dstRC;

    //QAudioRecorder *AudioRecorder;
    //QAudioProbe *AudioProbe;
public:
    GstElement *pipeline=nullptr,*pipeline2=nullptr;
    GstElement *appsrc;
    GstClockTime timestamp=0;
    GstElement *volumeElement=nullptr;

signals:
    void newImage(QImage &);
    void emitThreadImage(cv::Mat frameThread);
    void newImage2(QImage &);
    void emitThreadImage2(cv::Mat frameThread);
    void writing_success();
    void stop();
    void recording_stop();

};

#endif // VIDEOSTREAMER_H
