#ifndef OPENCVIMAGEPROVIDER_H
#define OPENCVIMAGEPROVIDER_H

#include <QObject>
#include <QImage>
#include <QQuickImageProvider>
#include <QMutex>
/**
 * @class OpencvImageProvider
 * @brief Bridges OpenCV image processing with the QML User Interface.
 * * This class allows QML to display images stored in C++ memory (like camera frames)
 * using the "image://" URL scheme. It acts as a buffer between high-speed
 * image processing and the UI display.
 */
class OpencvImageProvider : public QQuickImageProvider
{
    Q_OBJECT

public:
    /**
     * @brief Constructor for the image provider.
     * @param parent Pointer to the parent QObject.
     */
    OpencvImageProvider(QObject *parent = nullptr);

    /**
     * @brief Called by the QML engine to fetch an image.
     * @param id The unique identifier string from QML (e.g., "image://myprovider/id")
     * @param size Output parameter for the original size of the image.
     * @param requestedSize The size the QML element wants the image to be.
     * @return The current QImage to be displayed.
     */
    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize) override;

public slots:
    /**
     * @brief Updates the internal buffer with a new image.
     * @param image The new frame (usually converted from cv::Mat).
     */
    void updateImage(const QImage &image);

    /**
     * @brief Secondary update slot for handling a second stream or processed version.
     * @param image The second image frame.
     */
    void updateImage2(const QImage &image);

signals:
    /**
     * @brief Emitted when the primary image has been updated.
     * Connect this to a QML property or refresh trigger.
     */
    void imageChanged();

    /**
     * @brief Emitted when the secondary image has been updated.
     */
    void imageChanged2();

    /**
     * @brief Emitted if there is an error or the image stream is lost.
     */
    void no_image();

private:
    QImage image; // The internal storage for the current frame to be rendered. /**< TODO: describe */
    QImage image2; /**< TODO: describe */
    QImage resultant_image; /**< TODO: describe */
    QMutex mutex;
};

#endif // OPENCVIMAGEPROVIDER_H
