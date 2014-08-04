#include "gtimage.h"
#include <QtDebug>
#include <QImageReader>
#include <QSize>


GTImage::GTImage(const QString &fn, QObject *parent)
{
    isOk_ = true;
    setSrcFilename(fn);
    QImageReader reader(fn);
    if (!reader.canRead()) {
        isOk_ = false;
        return;
      }
    size_ = reader.size();
    reader.setScaledSize(size_.scaled(ThumbnailWidth, ThumbnailHeight, Qt::KeepAspectRatio));
    if(!thumbnail_.convertFromImage(reader.read())){
        isOk_ = false;
        return;
      }
    corners_ = QVector<QPointF>(4);
    corners_[0] = QPointF(0,0);
    corners_[1] = QPointF(size_.width(), 0);
    corners_[2] = QPointF(size_.width(), size_.height());
    corners_[3] = QPointF(0, size_.height());
    cstat_ = SetToSourceCornersAtLoad;

}

int GTImage::srcWidth() {
    checkSrcLoad();
    return src_.width();
}

int GTImage::srcHeight() {
    checkSrcLoad();
    return src_.height();
}

#if 0
QColor GTImage::getMedianColor(const QImage &img, const QRect &area) {
    // "QImage::pixel() is expensive when used for massive pixel manipulations."
    int len = area.width()*area.height();
    QVector<int> r(len), g(len), b(len);
    for(int i = 0; i < area.height(); i++) {
        for(int j = 0; j < area.width(); j++){
            QRgb px = img.pixel(area.bottomLeft() + QPoint(j,i));
            r[i*area.width()+j] = qRed(px);
            g[i*area.width()+j] = qGreen(px);
            b[i*area.width()+j] = qBlue(px);
        }
    }
    qSort(r);
    qSort(g);
    qSort(b);
    return QColor(r[len/2], g[len/2], b[len/2]);
}
#endif

QList<QColor> GTImage::getColorQuantiles (const QRect &area, const QList<qreal> &quantiles){
    checkSrcLoad();
    // "QImage::pixel() is expensive when used for massive pixel manipulations."
    int len = area.width()*area.height();
    QVector<int> r(len), g(len), b(len);
    for(int i = 0; i < area.height(); i++) {
        for(int j = 0; j < area.width(); j++){
            QRgb px = src_.pixel(area.bottomLeft() + QPoint(j,i));
            r[i*area.width()+j] = qRed(px);
            g[i*area.width()+j] = qGreen(px);
            b[i*area.width()+j] = qBlue(px);
        }
    }
    qSort(r);
    qSort(g);
    qSort(b);
    QList<QColor> qcols;

    for (QList<qreal>::const_iterator i = quantiles.begin(); i != quantiles.end(); ++i)
        qcols << QColor(r[(int)(len * (*i))],g[(int)(len * (*i))],b[(int)(len * (*i))]);
    return qcols;
}

#if 0
#include <QPoint>
QPolygon GTImage::findRectangle(int diam, qreal medianThreshold) {
    if(!checkSrcLoadARGB()) return QPolygon();

    /* Takže plán: začneme od prostředního bodu hledáním do šířky a pak... se uvidí*/


}
#endif

QPolygon GTImage::findRectangleSpiral(qreal relativeMedianThreshold){

}

//void GTImage::makeThumbnail() {
//  checkSrcLoad();
//  QImage thumbnailImage = src_.scaled(ThumbnailWidth, ThumbnailHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
//  thumbnail_ = QPixmap::fromImage(thumbnailImage);
//}

QPixmap GTImage::thumbnail(){
  return thumbnail_;
}

bool GTImage::checkSrcLoadARGB(){
    if(!checkSrcLoad()) return false;
    if(src_.format() != QImage::Format_ARGB32)
        src_ = src_.convertToFormat(QImage::Format_ARGB32);
    return true;
}

bool GTImage::checkSrcLoad(){
    if(src_.format() == QImage::Format_Invalid)
        src_ = QImage(srcFilename_);
    // TODO testy na typ chyby, vypuštění chybové hlášky (do logu a na stavový řádek)...
    if(src_.isNull()){
        emit srcLoadFailed(this);
        qWarning() << tr("Could not load image:") << srcFilename_;
        return false;
    }
    return true;
}

QVector<QPointF> GTImage::corners(){
  if(cstat_ != NotSet) return corners_;
  else {
      corners_[0] = QPointF(0,0);
      corners_[1] = QPointF(srcWidth(), 0);
      corners_[2] = QPointF(srcWidth(), srcHeight());
      corners_[3] = QPointF(0, srcHeight());
      cstat_ = SetToSourceCornersAtLoad;
      return corners_;
    }
}

bool GTImage::isOk(void) const {
  return isOk_;
}

bool GTImage::setCorners(const QVector<QPointF> &crs){
  corners_ = crs;
  return true; //TODO kontrola validity argumentu
}

void GTImage::checkSrcUnload(){
    if(!src_.isNull() && true /* TODO zde podmínka dle politiky */ )
        src_ = QImage();
}

QImage GTImage::srcImage(){
  QImage src;
  checkSrcLoad();
  src = src_;
  checkSrcUnload();
  return src;
}
