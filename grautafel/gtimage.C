#include "gtimage.h"
#include <QtDebug>
#include <QImageReader>
#include <QSize>
#include <QPagedPaintDevice>

using namespace GT;

QColor rgbInvert(QColor orig) {
  int r, g, b;
  orig.getRgb(&r, &g, &b);
  return QColor(255-r, 255-g, 255-b);
}

Image::Image(const QString &fn, QObject *parent,
             const struct PageSettings &settings,
             bool guessColors, bool guessShape) {
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

  settings_ = settings;

  if (guessColors || guessShape)
    checkSrcLoad();

  if (false /*quessShape*/) {
      ;//TODO
  }
  else {
    corners_ = QVector<QPointF>(4);
    corners_[0] = QPointF(0,0);
    corners_[1] = QPointF(size_.width(), 0);
    corners_[2] = QPointF(size_.width(), size_.height());
    corners_[3] = QPointF(0, size_.height());
    cstat_ = SetToSourceCornersAtLoad;
  }

  if (guessColors) {
      QList<qreal> probs;
      probs.append(lowerQuantileGuess);
      probs.append(higherQuantileGuess);
      probs.append(0.5);
      probs.append(1. - higherQuantileGuess);
      probs.append(1. - lowerQuantileGuess);
      QList<QColor> quantiles = getColorQuantiles(
            QRect(size_.width() / 4, size_.height() / 4,
                  size_.width() / 2, size_.height() / 2),
            probs);
      qDebug() << "median lightness: " << quantiles[2].lightness();
      if (quantiles[2].lightness() < 128) { // Tabule je tmavá, odhad vycucaný z prstu
         setColorsInverted(true);
         qDebug() << "colors inverted";
         setMinColor(rgbInvert(quantiles[4]).rgb());
         setMaxColor(rgbInvert(quantiles[3]).rgb());
        }
      else {
          setColorsInverted(false);
          setMinColor(quantiles[0].rgb());
          setMaxColor(quantiles[1].rgb());
        }
    }
  else {
    maxColor_ = qRgb(255, 255, 255);
    minColor_ = qRgb(0, 0, 0);
    invertColors_ = false;
    }

  if (guessColors || guessShape)
    checkSrcUnload();
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

QList<QColor> Image::getColorQuantiles (const QRect &area, const QList<qreal> &probs) {
  // FIXME zbytečně pomalé, lepší by byl trojitý bucketsort.
  checkSrcLoad();
  qDebug() << area;
  // "QImage::pixel() is expensive when used for massive pixel manipulations."
  QVector<int> hr(256), hg(256), hb(256); // histogram
  for(int i = 0; i < area.height(); i++) {
      for (int j = 0; j < area.width(); j++) {
          QRgb px = src_.pixel(area.topLeft() + QPoint(j, i));
          hr[qRed(px)] += 1;
          hg[qGreen(px)] += 1;
          hb[qBlue(px)] += 1;
        }
    }
  qreal rs = 0, gs = 0, bs = 0; // Celkové součty (vlastně zbytečné, mělo by dát velikost)
  for (int i = 0; i < 256; i++) {
      rs += hr[i];
      gs += hg[i];
      bs += hb[i];
    }

  QVector<qreal> rcdf(256), gcdf(256), bcdf(256); // Kumulativní součty
  int rss = 0, gss = 0, bss = 0;
  for (int i = 0; i < 256; i++) {
      rss += hr[i];
      gss += hg[i];
      bss += hb[i];
      rcdf[i] = rss / rs;
      gcdf[i] = gss / gs;
      bcdf[i] = bss / bs;
    }
//  qDebug() << rcdf;
//  qDebug() << gcdf;
//  qDebug() << bcdf;

  QList<QColor> quantiles;
  for (QList<qreal>::const_iterator p = probs.constBegin(); p != probs.constEnd(); p++) {
      int r, g, b;
      if (*p < rcdf[0]) {
          r = 0;
          goto endR;
        }
      for (int i = 1; i < 256; i++) {
          qreal avg = (rcdf[i-1] + rcdf[i]) / 2.;
          if (*p >= rcdf[i-1] && *p <= rcdf[i]) {
            if (*p < avg) r = i-1;
            else r = i;
            goto endR;
            }
        }
      r = 255;
      endR:
        if (*p < gcdf[0]) {
          g = 0;
          goto endG;
        }
      for (int i = 1; i < 256; i++) {
          qreal avg = (gcdf[i-1] + gcdf[i]) / 2.;
          if (*p >= gcdf[i-1] && *p <= gcdf[i]) {
            if (*p < avg) g = i-1;
            else g = i;
            goto endG;
            }
        }
      g = 255;
      endG:
      if (*p < bcdf[0]) {
          b = 0;
          goto endB;
        }
      for (int i = 1; i < 256; i++) {
          qreal avg = (bcdf[i-1] + bcdf[i]) / 2.;
          if (*p >= bcdf[i-1] && *p <= bcdf[i]) {
            if (*p < avg) b = i-1;
            else b = i;
            goto endB;
            }
        }
      b = 255;
      endB:
      quantiles.append(qRgb(r,g,b));
    }
  qDebug() << quantiles;
  return quantiles;
}

#if 0
QList<int> Image::getLightnessQuantiles(const QImage &img, const QList<qreal> &probs, const QRect &area) {
  QVector<int> hist = getLightnessHistogram(img, area);
  qreal sum = 0;
  for (int i = 0; i < 256; i++) sum += hist[i];
  qreal cdf[256];
  {
    int ss;
    for (int i = 0; i < 256; i++) {
      ss += hist[i];
      cdf[i] = ss / sum;
    }
  }
  QList<int> quantiles;
  for (QList<qreal>::const_iterator p = probs.constBegin(); p != probs.constEnd(); p++) {
    if (*p < cdf[0]) {
      quantiles.append(0);
      goto nextp;
    }
    for (int i = 1; i < 256; i++)
      if (*p < cdf[i]) {
        qreal avg = (cdf[i-1] + cdf[i]) / 2.;
        if (*p < avg) quantiles.append(i-1);
        else quantiles.append(i);
        goto nextp;
      }
    quantiles.append(255);
nextp:
    ;
  }
  return quantiles;
}
QVector<int> Image::getLightnessHistogram(const QImage &img, const QRect &area) {
  QVector<int> hist(256);
  for(int i = 0; i < area.height(); i++) {
    for (int j = 0; j < area.width(); j++) {
      QRgb px = img.pixel(area.bottomLeft() + QPoint(j, i));
      int l = QColor(px).lightness(); // tahle metoda je v Qt zbytečně nabobtnalá, možná to bude pomalé...
      hist[l] = hist[l] + 1;
    }
  }
  return hist;
}

QVector<int> Image::getLightnessHistogram(const QRect &area) {
  checkSrcLoad();
  QVector<int> hist = getLightnessHistogram(src_, area);
  checkSrcUnload();
  return hist;
}

QList<int> Image::getLightnessQuantiles(const QList<qreal> &probs, const QRect &area) {
  checkSrcLoad();
  QList<int> q = getLightnessQuantiles(src_, probs, area);
  checkSrcUnload();
  return q;
}

#endif


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

QTransform Image::transform(qreal unitScaling) {
  QTransform t = transform();
  //qDebug() << "orig.  tf" << t << "scaling: " << unitScaling;
  t = t * QTransform::fromScale(unitScaling, unitScaling);
  //qDebug() << "scaled tf" << t;
  return t;
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
  s.targetRect = QRectF(13.5, 15, 270, 180);
  return s;
}

QImage Image::targetImage()  {
  if (!checkSrcLoad()) return QImage();

  // Výpočet "přirozeného" rozlišení jako délky nejdelší strany na původním obrázku
  qreal whratio = targetSize().width() / targetSize().height();
  qreal max = 0;
  for (int i = 0; i < 4; i++) {
    qreal l = QLineF(corners_[i], corners_[(i+1) % 4]).length();
    if (1 == i % 2) l *= whratio;
    if (l > max) max = l;
  }
  qreal trfactor = max / targetSize().width(); // Tímto přenásobíme transformace, abychom dosáhli rozumného rozlišení

  qDebug() << "transform scale factor:" << trfactor;

  QRect cutFrame(trfactor * (targetRect().x() - transformDelta().x()),
                 trfactor * (targetRect().y() - transformDelta().y()),
                 trfactor * targetRect().width(),
                 trfactor * targetRect().height());
  //FIXME nutno ještě naškálovat (pro správné rozlišení) a ořezat:
  QImage transformed = src_.transformed(transform(trfactor), Qt::SmoothTransformation);
  transformed = transformed.copy(cutFrame);
  transformed.save("/tmp/debug.png");  // TODO odstranit
  checkSrcUnload();
  transformed = trimColors(transformed, minColor(), maxColor(), colorsInverted());
  return transformed;
}

const Image::PageSettings &Image::pageSettings() {
  return settings_;
}

/*!
 * \brief Calculates the shift between transform() and its QImage::trueMatrix().
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

QPointF Image::transformDelta(qreal unitScaling) {
  return transformDelta() * unitScaling;
}

bool Image::colorsInverted() const {
  return invertColors_;
}
void Image::setColorsInverted(bool i) {
  invertColors_ = i;
}

QRgb Image::minColor() const {
  return minColor_;
}
void Image::setMinColor(QRgb v) {
  minColor_ = v;
}
QRgb Image::maxColor() const {
  return maxColor_;
}
void Image::setMaxColor(QRgb v) {
  maxColor_ = v;
}

#if 0
static inline QRgb trimPxLightness (QRgb orig, qreal minL, qreal maxL) {
//  if (minL == 0 && maxL == 1) return orig;
  QColor c = orig;
  qreal h, s, l;
  c.getHslF(&h, &s, &l);

  if (l <= minL)
    return QColor(Qt::black).rgb();
  if (l >= maxL)
    return QColor(Qt::white).rgb();

  c.setHslF(h, s, (l-minL) / (maxL - minL));
  return c.rgb();
}
#endif

static inline QRgb trimPxColors(QRgb orig, QRgb minL, QRgb maxL) {
  int min, max, l, r, g, b;

  min = qRed(minL), max = qRed(maxL), l = qRed(orig);
  if (l <= min) r = 0;
  else if (l >= max) r = 255;
  else r = ((l - min) * 255) / (max - min + !(max-min));

  min = qGreen(minL), max = qGreen(maxL), l = qGreen(orig);
  if (l <= min) g = 0;
  else if (l >= max) g = 255;
  else g = ((l-min) * 255) / (max - min + !(max-min));

  min = qBlue(minL), max = qBlue(maxL), l = qBlue(orig);
  if (l <= min) b = 0;
  else if (l >= max) b = 255;
  else b = ((l-min) * 255) / (max - min + !(max - min));

  return qRgb(r, g, b);
}

QImage Image::trimColors(QImage img, QRgb minL, QRgb maxL, bool invertColors){
  // TODO není potřeba převod na ARGB?
  if(invertColors)
    img.invertPixels(QImage::InvertRgb);
  if (qRgb(0,0,0) == minL && qRgb(255,255,255) == maxL)
    return img;
  int h = img.height(), w = img.width();
  for(int j = 0; j < h; j++)
    for (int i = 0; i < w; i++){
      QRgb px = img.pixel(i,j);
      px = trimPxColors(px, minL, maxL);
      img.setPixel(i, j, px);
    }
  return img;
}

QImage Image::trimColors(QRgb minL, QRgb maxL, bool invertColors) {
  checkSrcLoad();
  QImage img = trimColors(src_, minL, maxL, invertColors);
  checkSrcUnload();
  return img;
}

QImage Image::trimColors() {
  return trimColors(minColor(), maxColor(), colorsInverted());
}
