#ifndef VIDEOSTREAMER_H
#define VIDEOSTREAMER_H

#include <QDateTime>
#include <QImage>
#include <QObject>
#include <QThread>
#include <QTimer>
#include <QVariant>
#include "qthread.h"
#include <iostream>
#include <opencv2/highgui.hpp>

using namespace cv;
static cv::VideoCapture cap, writing_cap;

class VideoStreamer : public QObject
{
    Q_OBJECT

public:
    VideoStreamer();
    ~VideoStreamer();

public:
    void streamVideo();
    void streamVideo2();
    QThread *threadStreamer = new QThread();
    void catchFrame(cv::Mat emittedFrame);
    void catchFrame2(cv::Mat emittedFrame);

public slots:
    void openVideoCamera(QString path);
    void open_image(QString Path);
    void start_recording();
    void streamerThreadSlot();
    void histogram_equalization(cv::Mat);
    cv::Mat gamma_correction(Mat, double);
    void stop_streaming();
    void edited_parameters(double brightness, double contrast);
    void filter_selection(int);
    void export_video(QString Path);
    void export_image(QString Path);
    void normal_processed(bool);
    void set_filter_parameters(int, int);

private:
    Mat simg;
    Mat sharpened;
    Mat blurred;
    cv::Mat frame, frame2, imageContrastHigh4, drawing, lowContrastMask, final_image, normal_frame;
    QTimer tUpdate, timer;
    int FPS_count = 0;
    int fps, fps_2;
    qint64 lastTimeStamp;
    qint64 currentTime = 0, elapsedTime;
    QString FPS;
    int thickness = 4;
    int frame_width = 0;
    int frame_height = 0;

    // Rotation angle in degrees
    double angle = 15.0;

    QString m_canny1 = "80";

    QString m_canny2 = "240";

    int enhance_value = 1;
    int delay_2 = 40;
    int calculated_difference = 0;
    int start_time, end_time;
    QDateTime date;
    QString formattedTime;
    QByteArray formattedTimeMsg;
    //parameters

    int selected_filter = 0;
signals:
    void newImage(QImage &);
    void newImage2(QImage &);
    void emitThreadImage(cv::Mat frameThread);
    void emitThreadImage2(cv::Mat frameThread2);
    void writing_success();
};

#endif // VIDEOSTREAMER_H
