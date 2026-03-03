#include "opencvimageprovider.h"

/**
 * @brief Constructor
 * Initializes the provider as an 'Image' type provider.
 * Sets a default 200x200 black image so the UI doesn't show a broken link on startup.
 */
OpencvImageProvider::OpencvImageProvider(QObject *parent)
    : QQuickImageProvider(QQuickImageProvider::Image)
{
    // Initialize with a placeholder black square
    image = QImage(200, 200, QImage::Format_RGB32);
    image.fill(QColor("black"));

    image2 = QImage(200, 200, QImage::Format_RGB32);
    image2.fill(QColor("black"));
}

/**
 * @brief Core method called by QML when an Image source changes.
 * * @param id The ID provided in the QML source string (e.g., "image://provider/id").
 * @param size Pointer used to return the original image size to the QML engine.
 * @param requestedSize The size dictated by the QML Image element's width/height.
 * @return The current QImage, scaled if necessary.
 */
QImage OpencvImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    //Q_UNUSED(id); // ID is not used here as we serve a single stream

    switch (id.toInt()) {
    case 0:
        resultant_image = this->image;
        break;
    case 1:
        resultant_image = this->image2;
        break;
    default:
        resultant_image = this->image;
        break;
    }
    if (size) {
        *size = resultant_image.size();
    }

    // If QML requests a specific size, scale the image while maintaining aspect ratio
    if (requestedSize.width() > 0 && requestedSize.height() > 0) {
        return resultant_image.scaled(requestedSize.width(), requestedSize.height(), Qt::KeepAspectRatio);
    }

    return resultant_image;
}

/**
 * @brief Updates the internal frame and triggers a UI refresh.
 * * This is usually connected to a signal from a Camera or Worker thread.
 * @param image The new frame to be displayed.
 */
void OpencvImageProvider::updateImage(const QImage &image)
{
    // Check if image is valid and actually different to save processing power
    if (!image.isNull() && this->image != image) {
        this->image = image;
        emit imageChanged(); // Signal QML that the source needs to be re-fetched
    }
}

/**
 * @brief Secondary update method for an alternative image stream.
 * * Useful if you are displaying the station side camera version (e.g.,Webcamera)
 * alongside the original stream.
 */
void OpencvImageProvider::updateImage2(const QImage &image)
{
    if (!image.isNull() && this->image != image) {
        this->image2 = image;
        emit imageChanged2();
    }
}
