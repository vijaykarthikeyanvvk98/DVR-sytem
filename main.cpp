#include <QQmlContext>
#include <QWindow>
#include <QApplication>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <QQmlApplicationEngine>
#include "opencvimageprovider.h"
#include "videostreamer.h"
using namespace cv;
int main(int argc, char *argv[])
{
    //qDebug()<<cv::getBuildInformation();
    QApplication a(argc, argv);
    qRegisterMetaType<cv::Mat>("cv::Mat");
    //std::cout << cv::getBuildInformation() << std::endl;
    VideoStreamer videoStreamer;

    OpencvImageProvider *liveImageProvider(new OpencvImageProvider);
    QQmlApplicationEngine engine;

    engine.rootContext()->setContextProperty("VideoStreamer", &videoStreamer);

    engine.rootContext()->setContextProperty("liveImageProvider", liveImageProvider);

    engine.addImageProvider("live", liveImageProvider);

    const QUrl url(QStringLiteral("qrc:/rtsp2.qml"));

    QObject::connect(&videoStreamer,
                     &VideoStreamer::newImage,
                     liveImageProvider,
                     &OpencvImageProvider::updateImage);
    QObject::connect(&videoStreamer,
                     &VideoStreamer::newImage2,
                     liveImageProvider,
                     &OpencvImageProvider::updateImage2);
    engine.loadFromModule("dvr_system", "Main");

    if (engine.rootObjects().isEmpty())
        return -1;
    QObject *rootObject = engine.rootObjects().first();
    QWindow *window = qobject_cast<QWindow *>(rootObject);

    if (window) {
        window->setTitle("DVR SYSTEM");

        window->showMaximized();
        window->setMinimumSize(QSize(670, 470));
    }

    return a.exec();
}
