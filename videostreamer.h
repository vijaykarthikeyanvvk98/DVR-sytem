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
/**
 * @brief
 *
 */
class VideoStreamer : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief
     *
     */
    VideoStreamer();
    /**
     * @brief
     *
     */
~VideoStreamer();

public:
    /**
     * @brief
     *
     */
void streamVideo();
    QThread *threadStreamer = new QThread(); /**< TODO: describe */
    /**
     * @brief
     *
     * @param emittedFrame
     */
void catchFrame(cv::Mat emittedFrame);
    /**
     * @brief
     *
     * @param Mat
     * @return Mat
     */
Mat write_frame(Mat);
    //Received stream
    /**
     * @brief
     *
     */
void streamVideo2();
    QThread *threadStreamer2 = new QThread(); /**< TODO: describe */
    /**
     * @brief
     *
     * @param emittedFrame
     */
void catchFrame2(cv::Mat emittedFrame);
    /**
     * @brief
     *
     * @param QString
     */
void createPipeline(const QString);
    /**
     * @brief
     *
     */
void startPipeline();
    /**
     * @brief
     *
     * @param Mat
     */
void pushFrame(Mat);
    /**
     * @brief
     *
     */
void reset_pipeline();
    /**
     * @brief
     *
     * @param bus
     * @param msg
     * @param data
     * @return gboolean
     */
static gboolean bus_call(GstBus *bus, GstMessage *msg, gpointer data);
    /**
     * @brief
     *
     * @param bus
     * @param msg
     * @param data
     * @return gboolean
     */
static gboolean on_message(GstBus *bus, GstMessage *msg, gpointer data);

public slots:
    /**
     * @brief
     *
     */
void openVideoCamera();
    /**
     * @brief
     *
     * @param QString
     */
void openVideoCamera2(QString);
    /**
     * @brief
     *
     */
void start_recording();
    /**
     * @brief
     *
     */
void streamerThreadSlot();
    /**
     * @brief
     *
     */
void streamerThreadSlot2();
    /**
     * @brief
     *
     */
void stop_recording();
    /**
     * @brief
     *
     */
void pause_streaming();
    /**
     * @brief
     *
     */
void create_directory();
    /**
     * @brief
     *
     */
void sendEOS();
    /**
     * @brief
     *
     */
void qImageToCvMat();
    /**
     * @brief
     *
     * @param bool
     */
void pushback(bool );
    /**
     * @brief
     *
     */
void subtitle_streaming();
    /**
     * @brief
     *
     * @param bool
     */
void is_subttitle(bool);

private:
    QTimer tUpdate, tUpdate2,timer_5; /**< TODO: describe */
    int fps, fps_2; /**< TODO: describe */
    qint64 currentTime = 0, elapsedTime; /**< TODO: describe */
    int calculated_difference = 0; /**< TODO: describe */
    int start_time, end_time; /**< TODO: describe */
    QDateTime date; /**< TODO: describe */
    QString formattedTime; /**< TODO: describe */
    QByteArray formattedTimeMsg; /**< TODO: describe */
    QString logFileDir = "",logFileDir2 = "",logFileDir3 = "",logFileDir4=""; /**< TODO: describe */
    QDateTime currentDateTime; /**< TODO: describe */
    // Increment step for smooth rotation
    QFile subtitleFile; /**< TODO: describe */
    QTextStream out; /**< TODO: describe */

public:
    GstElement *pipeline=nullptr; /**< TODO: describe */
    GstElement *appsrc; /**< TODO: describe */
    GstClockTime timestamp=0; /**< TODO: describe */
    GstElement *volumeElement=nullptr; /**< TODO: describe */

signals:
    /**
     * @brief
     *
     * @param
     */
void newImage(QImage &);
    /**
     * @brief
     *
     * @param frameThread
     */
void emitThreadImage(cv::Mat frameThread);
    /**
     * @brief
     *
     * @param
     */
void newImage2(QImage &);
    /**
     * @brief
     *
     * @param frameThread
     */
void emitThreadImage2(cv::Mat frameThread);
    /**
     * @brief
     *
     */
void writing_success();
    /**
     * @brief
     *
     */
void recording_stop();
    /**
     * @brief
     *
     */
void open_finished();
    /**
     * @brief
     *
     */
void write_finished();
    void stop();
};

#endif // VIDEOSTREAMER_H
