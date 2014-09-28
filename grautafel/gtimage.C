#include "gtimage.h"
#include <QtDebug>
#include <QImageReader>
#include <QSize>
#include <QPagedPaintDevice>

using namespace GT;

Image::Image(const QString &fn, QObject *parent, const struct PageSettings &settings) {
  isOk_ = true;
  setSrcFilename(fn);
  QImageReader reader(fn);
  if (!reader.canRead()) {
    isOk_ = false;
    return;
  }
  size_ = reader.size();
  reader.setScaledSize(size_.scaled(ThumbnailWidth, ThumbnailHeight, Qt::KeepAspectRatio));
  if(!thumbnail_.convertFromImage(reader.read())) {
    isOk_ = false;
    return;
  }
  corners_ = QVector<QPointF>(4);
  corners_[0] = QPointF(0,0);
  corners_[1] = QPointF(size_.width(), 0);
  corners_[2] = QPointF(size_.width(), size_.height());
  corners_[3] = QPointF(0, size_.height());
  cstat_ = SetToSourceCornersAtLoad;

  settings_ = settings;

  setLastZoom(0);

}

void Image::setPageSettings(PageSettings &s) {
  settings_ = s;
}

int Image::srcWidth() {
  checkSrcLoad();
  return src_.width();
}

int Image::srcHeight() {
  checkSrcLoad();
  return src_.height();
}

#if 0
QColor GTImage::getMedianColor(const QImage &img, const QRect &area) {
  // "QImage::pixel() is expensive when used for massive pixel manipulations."
  int len = area.width()*area.height();
  QVector<int> r(len), g(len), b(len);
  for(int i = 0; i < area.height(); i++) {
    for(int j = 0; j < area.width(); j++) {
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

QList<QColor> Image::getColorQuantiles (const QRect &area, const QList<qreal> &quantiles) {
  checkSrcLoad();
  // "QImage::pixel() is expensive when used for massive pixel manipulations."
  int len = area.width()*area.height();
  QVector<int> r(len), g(len), b(len);
  for(int i = 0; i < area.height(); i++) {
    for(int j = 0; j < area.width(); j++) {
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

#if 0
QPolygon Image::findRectangleSpiral(qreal relativeMedianThreshold) {

}
#endif

//void GTImage::makeThumbnail() {
//  checkSrcLoad();
//  QImage thumbnailImage = src_.scaled(ThumbnailWidth, ThumbnailHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
//  thumbnail_ = QPixmap::fromImage(thumbnailImage);
//}

QPixmap Image::thumbnail() {
  return thumbnail_;
}

bool Image::checkSrcLoadARGB() {
  if(!checkSrcLoad()) return false;
  if(src_.format() != QImage::Format_ARGB32)
    src_ = src_.convertToFormat(QImage::Format_ARGB32);
  return true;
}

bool Image::checkSrcLoad() {
  if(src_.format() == QImage::Format_Invalid)
    src_ = QImage(srcFilename_);
  // TODO testy na typ chyby, vypuštění chybové hlášky (do logu a na stavový řádek)...
  if(src_.isNull()) {
    emit srcLoadFailed(this);
    qWarning() << tr("Could not load image:") << srcFilename_;
    return false;
  }
  return true;
}

QVector<QPointF> Image::corners() {
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

bool Image::isOk(void) const {
  return isOk_;
}

bool Image::setCorners(const QVector<QPointF> &crs) {
  corners_ = crs;
  return true; //TODO kontrola validity argumentu
}

void Image::checkSrcUnload() {
  if(!src_.isNull() && true /* TODO zde podmínka dle politiky */ )
    src_ = QImage();
}

QImage Image::srcImage() {
  QImage src;
  checkSrcLoad();
  src = src_;
  checkSrcUnload();
  return src;
}

QSizeF Image::targetSize() const {
  return settings_.targetRect.size();
}

void Image::setTargetSize(const QSizeF &s) {
  settings_.targetRect.setSize(s);
}

QRectF Image::targetRect() const {
  return settings_.targetRect;
}

/*!
 * \brief Returns the transform mapping the table on the photo to the target rectangle.
 *
 * Beware that this is calculated from the _saved_ corner positions.
 * \return
 */
QTransform Image::transform() {
  QPolygonF photoQuad(corners());
  QPolygonF targetQuad(targetRect());
  targetQuad.pop_back(); // Workaround for a bug in QTransform::squareToQuad().
  qDebug() << photoQuad;
  qDebug() << targetQuad;
  QTransform tr;
  if (false == QTransform::quadToQuad(photoQuad, targetQuad, tr)) {
    qWarning("Failed to find the transform (perhaps the source polygon is non-convex). Setting to identity.");
  }
  qDebug() << "Orig. tf: " << tr;
  qDebug() << "Real  tf: " << QImage::trueMatrix(tr, size_.width(), size_.height());
  return tr;
}

void Image::setLastViewPoint(const QPointF &where) {
  lastViewPoint_ = where;
}

void Image::setLastZoom(qreal zoom) {
  lastZoom_ = zoom;
}

QPointF Image::lastViewPoint(void) const {
  return lastViewPoint_;
}

qreal Image::lastZoom(void) const {
  return lastZoom_;
}

Image::PageSettings Image::PageSettings::defaultSettings() {
  Image::PageSettings s;
  s.pageSize = QPagedPaintDevice::A4;
  s.orientation = QPageLayout::Landscape;
  s.pageSizeMM = QSizeF(297,210);
  s.targetRect = QRectF(13.7, 15, 270, 180);
  return s;
}

QImage Image::targetImage()  {
  if (!checkSrcLoad()) return QImage();
  //FIXME nutno ještě naškálovat (pro správné rozlišení) a ořezat:
  QImage transformed = src_.transformed(transform(), Qt::SmoothTransformation);
  transformed.save("/tmp/debug.png");  // TODO odstranit
  checkSrcUnload();
  return transformed;
}

const Image::PageSettings &Image::pageSettings() {
  return settings_;
}

/*!
 * \brief Calculates the shift between transform() and its QImage::trueMatrix.
 * This is needed as a workaround for clipping the image correctly
 *
 * \return A vector which shifts the top left corner of the board back to origin.
 */
QPointF Image::transformDelta() {
  const QTransform matrix = transform();
  const QRectF rect(0, 0, size_.width(), size_.height());
  const QRect mapped = matrix.mapRect(rect).toAlignedRect();
  const QPoint delta = mapped.topLeft();
  return delta;
}
