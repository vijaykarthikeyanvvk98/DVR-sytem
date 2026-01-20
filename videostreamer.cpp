#include "videostreamer.h"
#include <QDateTime>
#include <QDir>
#include <QElapsedTimer>
#include <QFileInfo>
#include <QStandardPaths>
#include "opencv2/imgproc.hpp"
#include "qdebug.h"
#include <chrono>
#include <cmath>
//#include <filesystem>
#include <iostream>
#include <opencv2/core.hpp> // Basic OpenCV structures (cv::Mat)
#include <opencv2/gapi.hpp >
#include <opencv2/gapi/gstreaming.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp> // Video write
#include "opencv2/imgproc.hpp"
#include "opencv2/photo/photo.hpp"
using namespace cv;

static VideoWriter video, writing_video;
static bool recording_status = false;
static double amount = 1, bright = 10;
static bool original_not = false;

//Filter parameters
static int filter_pixel_size
    = 13; //Kernel Size(only Odd and Positive for Gaussian Filter) or Filtering pixel Size for Bilateral Filter
static int sigma_x = 25; // Spatial Deviation in (X-direction) for Gaussian Filter and Bilateral Filter
static int sigma_y = 25;     // Spatial Deviation in (Y-direction) for Gaussian Filter
static int sigma_color = 80; //Pixel Intensity Factor for Bilateral Filter
QElapsedTimer timer_1;

VideoStreamer::VideoStreamer()
{
    connect(&tUpdate, &QTimer::timeout, this, &VideoStreamer::streamVideo);
    //connect(&tUpdate, &QTimer::timeout, this, &VideoStreamer::streamVideo2);
}

VideoStreamer::~VideoStreamer()
{
    //cap.release();
    tUpdate.stop();
    threadStreamer->requestInterruption();
}

void VideoStreamer::streamVideo()
{
    if (!original_not) {
        if (!frame.empty()) {
            QImage img = QImage(frame.data, frame.cols, frame.rows, QImage::Format_RGB888)
            .rgbSwapped();
            emit newImage(img);

        } else {
            qDebug() << "Frame empty";
        }
    } else {
        if (!normal_frame.empty()) {
            QImage img2 = QImage(normal_frame.data,
                                 normal_frame.cols,
                                 normal_frame.rows,
                                 QImage::Format_RGB888)
                              .rgbSwapped();
            emit newImage(img2);
        } else {
            qDebug() << "Frame empty";
        }
    }
}

void VideoStreamer::streamVideo2()
{
    if (!original_not) {
        if (!frame2.empty()) {
            QImage img2 = QImage(frame2.data, frame2.cols, frame2.rows, QImage::Format_RGB888)
            .rgbSwapped();
            emit newImage2(img2);
        } else {
            qDebug() << "Frame empty";
        }
    } else {
        if (!normal_frame.empty()) {
            QImage img2 = QImage(normal_frame.data,
                                 normal_frame.cols,
                                 normal_frame.rows,
                                 QImage::Format_RGB888)
                              .rgbSwapped();
            emit newImage2(img2);
        } else {
            qDebug() << "Frame empty";
        }
    }
}

void VideoStreamer::catchFrame(cv::Mat emittedFrame)
{
    frame = emittedFrame;
}

void VideoStreamer::catchFrame2(cv::Mat emittedFrame2)
{
    normal_frame = emittedFrame2;
}

void VideoStreamer::openVideoCamera(QString path)
{
    qDebug() << path;
    /*if (cap.isOpened())
        cap.release();*/
    if (path.length() == 1)
        cap.open(0);
    else {
        cap.open(path.toStdString());
    }

    if (!cap.isOpened()) {
        qDebug() << "Error opening video stream or file";
        return;
    }

    int frame_width = cap.get(cv::CAP_PROP_FRAME_WIDTH);
    int frame_height = cap.get(cv::CAP_PROP_FRAME_HEIGHT);

    video.open("C:/Users/hp/Videos/Captures/output.avi",
               cv::VideoWriter::fourcc('M', 'J', 'P', 'G'),
               cap.get(cv::CAP_PROP_FPS),
               Size(frame_width, frame_height),
               true);
    VideoStreamer *worker = new VideoStreamer();
    worker->moveToThread(threadStreamer);
    QObject::connect(threadStreamer, SIGNAL(started()), worker, SLOT(streamerThreadSlot()));
    QObject::connect(worker, &VideoStreamer::emitThreadImage, this, &VideoStreamer::catchFrame);
    QObject::connect(worker, &VideoStreamer::emitThreadImage2, this, &VideoStreamer::catchFrame2);

    threadStreamer->start();
    if (1000 / cap.get(cv::CAP_PROP_FPS) < 0)
        tUpdate.start(1000 / 25);
    else
        tUpdate.start(1000 / cap.get(cv::CAP_PROP_FPS));
}

void VideoStreamer::open_image(QString Path)
{
    cv::Mat mask_frame,frame1,image;
    if (cap.isOpened()) {
        tUpdate.stop();
        threadStreamer->requestInterruption();
    } else
        ;

    if (Path.startsWith("file:///")) {
        Path = Path.remove(0, 8);
    } else
        ;
    frame1 = cv::imread(Path.toStdString(), cv::IMREAD_COLOR);
    QImage img2 = QImage(frame1.data, frame1.cols, frame1.rows, QImage::Format_RGB888).rgbSwapped();
    //emit newImage2(img2);

    if (!frame1.empty()) {
        mask_frame=cv::imread(Path.toStdString(), 0);
        //cv::inpaint(frame1,mask_frame,image,5,cv::INPAINT_NS);
        cv::cvtColor(image, simg, COLOR_BGR2GRAY);
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
                intensity = image.at<Vec3b>(i, j);

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
                intensity = image.at<Vec3b>(i, j);

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

                image.at<Vec3b>(i, j) = intensity;
            }
        }

        for (int i = 0; i < simg.rows; i++) {
            for (int j = 0; j < simg.cols; j++) {
                intensity = image.at<Vec3b>(i, j);
                intensity.val[0] = (intensity.val[0] - vmin_b) * 255 / (vmax_b - vmin_b);
                intensity.val[1] = (intensity.val[1] - vmin_g) * 255 / (vmax_g - vmin_g);
                intensity.val[2] = (intensity.val[2] - vmin_r) * 255 / (vmax_r - vmin_r);
                image.at<Vec3b>(i, j) = intensity;
            }
        }
        // Apply gamma correction (gamma = 0.5 for example)
        //frame_to_process = gamma_correction(frame_to_process, 0.25);
        // sharpen image using "unsharp mask" algorithm
        image.convertTo(image, -1, 1, bright); //decrease the brightness

        double sigma = 5, threshold = 10;

        if (selected_filter == 0) {
            if (filter_pixel_size % 2 == 0)
                filter_pixel_size = filter_pixel_size + 1;
            GaussianBlur(image,
                         blurred,
                         Size(filter_pixel_size, filter_pixel_size),
                         sigma_x,
                         sigma_y,cv::BORDER_DEFAULT);
        } else if (selected_filter == 1)
            bilateralFilter(image, blurred, filter_pixel_size, sigma_color, sigma_x);
        lowContrastMask = abs(image - blurred) < threshold;
        //sharpened = image * (1 + amount) + blurred * (-amount);
        //.copyTo(sharpened, lowContrastMask);
        image.copyTo(final_image, lowContrastMask);
        QImage img = QImage(image.data, image.cols, image.rows, QImage::Format_RGB888)
                         .rgbSwapped();
        if (original_not) {
            emit newImage(img2);
        } else {
            emit newImage(img);
        }
    }
}

void VideoStreamer::start_recording()
{
    recording_status = true;
    int frame_width = cap.get(cv::CAP_PROP_FRAME_WIDTH);
    int frame_height = cap.get(cv::CAP_PROP_FRAME_HEIGHT);

    video.open("C:/Users/hp/Videos/Captures/output.avi",
               cv::VideoWriter::fourcc('M', 'J', 'P', 'G'),
               cap.get(cv::CAP_PROP_FPS),
               Size(frame_width, frame_height),
               true);

    if (!video.isOpened()) {
        std::cerr << "Error: Unable to create the video writer\n";
    }
}

void VideoStreamer::streamerThreadSlot()
{
    cv::Mat tempFrame, normal_frame2;
    int delay = 40;
    int calculated_difference = 1000 / cap.get(cv::CAP_PROP_FPS);

    if (!cap.isOpened()) {
        cap.release();
        return;
    }

    while (1) {
        // Record start time
        auto startTime = std::chrono::high_resolution_clock::now();

        if (threadStreamer->isInterruptionRequested()) {
            // qDebug() << "Interrupt requested";
            cap.release();
            video.release();
            return;
        }
        if (!cap.isOpened())
            return;
        cap >> tempFrame;

        normal_frame2 = tempFrame.clone();
        emit emitThreadImage2(normal_frame2);

        if (tempFrame.data) {
            //histogram_equalization(tempFrame);
            emit emitThreadImage(tempFrame);
        } else {
            video.release();
            return;
            qDebug() << "No data";
        }
        // Record end time
        auto endTime = std::chrono::high_resolution_clock::now();

        // Calculate the actual time taken to process the frame
        int actual_difference
            = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
        qDebug() << "calculated" << calculated_difference;
        qDebug() << "actual" << actual_difference;

        int outcome = std::abs(calculated_difference - actual_difference);
        if (actual_difference < calculated_difference)
            delay = outcome + actual_difference;

        else
            delay = actual_difference - outcome;

        if (delay > 1000 || delay < 0)
            delay = 40;
        qDebug() << "delay" << delay;

        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
        video.write(sharpened);
    }
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
    frame_to_process.convertTo(frame_to_process, -1, 1, bright); //decrease the brightness
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
    emit emitThreadImage(sharpened);
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

void VideoStreamer::stop_streaming()
{
    cap.release();
    tUpdate.stop();
}

void VideoStreamer::edited_parameters(double brightness, double contrast)
{
    amount = contrast;
    bright = brightness;
    //qDebug() << bright;
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
                writing_video.write(writing_frame);
            } else {
                writing_video.release();
                writing_cap.release();
                emit writing_success();
                QFile::remove("C:/Users/hp/Videos/Captures/output.avi");
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

void VideoStreamer::export_image(QString Path)
{
    if (Path.startsWith("file:///")) {
        Path = Path.remove(0, 8);
    } else
        ;
    QString targetDirectory = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
    // Modify the target directory to add a subdirectory for your files.
    QString logFileDir = targetDirectory + "/Processed_Images";

    // Create the directory if it doesn't exist.
    QDir().mkpath(logFileDir);
    QDateTime currentDateTime = QDateTime::currentDateTime();
    formattedTime = currentDateTime.toString("dd.MM.yyyyhh.mm.ss");

    // Construct the file paths relative to the target directory.
    QString outputPath = Path;
    std::string outputPathStdString = Path.toStdString();
    cv::imwrite(outputPathStdString, final_image);
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
