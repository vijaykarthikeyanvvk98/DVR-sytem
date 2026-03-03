#ifndef VIDEOSTREAMER_H
#define VIDEOSTREAMER_H

#include <QDateTime>
#include <QImage>
#include <QObject>
#include <QThread>
#include <QTimer>
#include <QVariant>
#include "qfloat16.h"
#include "qthread.h"
#include <opencv2/highgui.hpp>
#include <gst/gst.h>
#include <gst/app/gstappsrc.h> // Include this header for appsrc
#include <qdir.h>
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
    void createPipeline(const QString);
    void startPipeline();
    void pushFrame(Mat);
    void reset_pipeline();
    static gboolean bus_call(GstBus *bus, GstMessage *msg, gpointer data);
    static gboolean on_message(GstBus *bus, GstMessage *msg, gpointer data);

public slots:
    void openVideoCamera();
    void openVideoCamera2(QString);
    void start_recording();
    void streamerThreadSlot();
    void streamerThreadSlot2();
    void stop_recording();
    void pause_streaming();
    void create_directory();
    void sendEOS();
    void qImageToCvMat();
    void pushback(bool );
    void subtitle_streaming();
    void is_subttitle(bool);

private:
    QTimer tUpdate, tUpdate2,timer_5;
    int fps, fps_2;
    qint64 currentTime = 0, elapsedTime;
    int calculated_difference = 0;
    int start_time, end_time;
    QDateTime date;
    QString formattedTime;
    QByteArray formattedTimeMsg;
    QString logFileDir = "",logFileDir2 = "",logFileDir3 = "",logFileDir4="";
    QDateTime currentDateTime;
    // Increment step for smooth rotation
    QFile subtitleFile;
    QTextStream out;

public:
    GstElement *pipeline=nullptr;
    GstElement *appsrc;
    GstClockTime timestamp=0;
    GstElement *volumeElement=nullptr;

signals:
    void newImage(QImage &);
    void emitThreadImage(cv::Mat frameThread);
    void newImage2(QImage &);
    void emitThreadImage2(cv::Mat frameThread);
    void writing_success();
    void recording_stop();
    void open_finished();
    void write_finished();
    void stop();
};

#endif // VIDEOSTREAMER_H
