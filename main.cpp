#include <QQmlContext>
#include <QWindow>
#include <QApplication>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <QQmlApplicationEngine>
#include "opencvimageprovider.h"
#include "videostreamer.h"
#include <QSplashScreen>

using namespace cv;
int main(int argc, char *argv[])
{
    //std::cout << cv::getBuildInformation() << std::endl;
    QApplication a(argc, argv);
    QSplashScreen *splash = new QSplashScreen;
    splash->setPixmap(QPixmap(":/resources/images/vikra_2.jpeg"));
    splash->show();
    qRegisterMetaType<cv::Mat>("cv::Mat");
    //std::cout << cv::getBuildInformation() << std::endl;
    VideoStreamer videoStreamer;
    videoStreamer.create_directory();

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
    // Tell Qt to look for modules in the local 'qml' folder we just copied
    //engine.addImportPath(QCoreApplication::applicationDirPath() + "/qml");
    if (engine.rootObjects().isEmpty())
        return -1;
    QObject *rootObject = engine.rootObjects().first();
    QWindow *window = qobject_cast<QWindow *>(rootObject);
    const QIcon icon = QIcon(":/dvr_system/images/vikra_2.jpeg");

    if (window) {
        window->setTitle("DVR SYSTEM");
        window->setIcon(icon);

        window->showMaximized();
        window->setMinimumSize(QSize(670, 470));
    }
    QTimer::singleShot(1000,splash,SLOT(close()));

    return a.exec();
}
