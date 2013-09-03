#include "gtimage.h"

GTImage::GTImage(QObject *parent) :
    QObject(parent)
{
}

static QColor GTImage::getMedianColor(const QImage &img, const QRect &area) {
    // "QImage::pixel() is expensive when used for massive pixel manipulations."
    int len = area.width()*area.height();
    QVector<int> r(len), g(len), b(len);
    for(int i = 0; i < area.height(); i++) {
        for(int j = 0; j < area.width(); j++){
            QRgb px = img.pixel(area.bottomLeft() + QPoint(j,i));
            r[i*area.width+j] = qRed(px);
            g[i*area.width+j] = qGreen(px);
            b[i*area.width+j] = qBlue(px);
        }
    }
    qSort(r);
    qSort(g);
    qSort(b);
    return QColor(r[len/2], g[len/2], b[len/2]);
}
