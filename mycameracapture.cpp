#include "mycameracapture.h"
#include <QPainter>
#include <QDebug>

MyCameraCapture::MyCameraCapture(QLabel *widget, QObject *parent)
    : QAbstractVideoSurface(parent)
    , m_image()
    , m_label(widget)
{
}

QImage MyCameraCapture::getImage() const
{
    QMutexLocker l(&m_locker);
    return m_image;
}

QList<QVideoFrame::PixelFormat> MyCameraCapture::supportedPixelFormats(QAbstractVideoBuffer::HandleType type) const
{
    Q_UNUSED(type);

    return QList<QVideoFrame::PixelFormat>()
           << QVideoFrame::Format_ARGB32 << QVideoFrame::Format_ARGB32_Premultiplied << QVideoFrame::Format_RGB32
           << QVideoFrame::Format_RGB24 << QVideoFrame::Format_RGB565 << QVideoFrame::Format_RGB555
           << QVideoFrame::Format_ARGB8565_Premultiplied << QVideoFrame::Format_BGRA32
           << QVideoFrame::Format_BGRA32_Premultiplied << QVideoFrame::Format_BGR32 << QVideoFrame::Format_BGR24
           << QVideoFrame::Format_BGR565 << QVideoFrame::Format_BGR555 << QVideoFrame::Format_BGRA5658_Premultiplied
           << QVideoFrame::Format_AYUV444 << QVideoFrame::Format_AYUV444_Premultiplied << QVideoFrame::Format_YUV444
           << QVideoFrame::Format_YUV420P << QVideoFrame::Format_YV12 << QVideoFrame::Format_UYVY
           << QVideoFrame::Format_YUYV << QVideoFrame::Format_NV12 << QVideoFrame::Format_NV21
           << QVideoFrame::Format_IMC1 << QVideoFrame::Format_IMC2 << QVideoFrame::Format_IMC3
           << QVideoFrame::Format_IMC4 << QVideoFrame::Format_Y8 << QVideoFrame::Format_Y16 << QVideoFrame::Format_Jpeg
           << QVideoFrame::Format_CameraRaw << QVideoFrame::Format_AdobeDng;
}

bool MyCameraCapture::present(const QVideoFrame &frame)
{
    if (frame.isValid()) {
        QVideoFrame cloneFrame(frame);
        cloneFrame.map(QAbstractVideoBuffer::ReadOnly);
        {
            QMutexLocker l(&m_locker);

            QImage::Format imageFormat = QVideoFrame::imageFormatFromPixelFormat(frame.pixelFormat());
            if (imageFormat != QImage::Format_Invalid) {
                m_image = QImage(cloneFrame.bits(), cloneFrame.width(), cloneFrame.height(), imageFormat);
            } else {
                int nbytes = frame.mappedBytes();
                m_image = QImage::fromData(frame.bits(), nbytes);
            }
        }
        cloneFrame.unmap();

        m_label->setPixmap(QPixmap::fromImage(m_image).scaled(m_label->size(), Qt::KeepAspectRatio));

        return true;
    }

    return false;
}
