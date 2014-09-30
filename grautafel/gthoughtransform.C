#include "gthoughtransform.h"
#include "gtconvolutionkernel.h"
#include <QImage>
#include <QtGlobal>

using namespace GT;

static const double pi = 3.14159265358979323846;

#include <math.h>
static inline int sq(int x) {
  return x*x;
}

void HoughTransform::set(int r, int alpha, double val) {
  data_[r * angleRes_ + alpha] = val;
}

void HoughTransform::add(int r, int alpha, double val) {
  data_[r * angleRes_ + alpha] += val;
}

double HoughTransform::get(int r, int alpha) const {
  return data_[r * angleRes_ + alpha];
}

double HoughTransform::get(const Coords c) const {
  return data_[c.r * angleRes_ +c.alpha];
}

HoughTransform::HoughTransform(const QImage *src, int angleResolution, int margin) {
  leftMargin_ = rightMargin_ = topMargin_ = bottomMargin_ = margin;
  originalHeight_ = src->height();
  originalWidth_ = src->width();
  radius_ = (int) sqrt(sq(src->height()) + sq(src->width())) / 2;
  angleRes_ = angleResolution;

  data_ = new double[radius_ * angleResolution]();
  // Center coordinates
  int cX = src->width()/2;
  int cY = src->height()/2;

  for (int x = edgeDetectorX->xorig + leftMargin_; x < src->width() - (edgeDetectorX->xsiz - edgeDetectorX->xorig) - rightMargin_; x++)
    for (int y = edgeDetectorX->yorig + topMargin_; y < src->height() - (edgeDetectorX->ysiz - edgeDetectorX->yorig)- bottomMargin_; y++) {
      QPoint pos(x,y);
      double val = edgeDetectorX->dConvolveSquared(src, pos);
      val += edgeDetectorY->dConvolveSquared(src, pos);
      val = sqrt(val);
//            int R = (int) round(sqrt(sq(x-cX)+sq(y-cY))) % radius; //FIXME je modulo nutné
      int Alpha = ((int) round(atan2(y-cY, x-cX) * angleRes_ / (2 * pi))) % angleRes_;
      for (int alphaDif = -angleRes_ / 4; alphaDif < angleRes_ / 4 ; alphaDif++) {
        int alpha = (Alpha + alphaDif + angleRes_) % angleRes_;
        double a = alpha * 2 * pi / angleRes_;
        int r = (int) round((x-cX)*cos(a) + (y-cY)*sin(a));
        if (r >= 0)
          add(r, alpha, val);
      }
    }
  coords_by_value_init();
}

#include <algorithm>

void HoughTransform::coords_by_value_init(void) {
  coordsByValue_.resize(radius_ * angleRes_);
  for(int r = 0; r < radius_; r++)
    for(int alpha = 0; alpha < angleRes_; alpha++) {
      coordsByValue_[r * angleRes_ + alpha].r = r;
      coordsByValue_[r * angleRes_ + alpha].alpha = alpha;
    }
  cmpstruct s(this);
  std::sort(coordsByValue_.begin(), coordsByValue_.end(), s);
}

/*!
 * \brief Guesses the board's corners' position
 *
 * Goes through the Hought transform from the brightest point.
 * Searches for the first four brightest points, while ignoring
 * anything in the +- 20 degrees around the points already found.
 *
 * \return
 */
std::vector<HoughTransform::Coords> HoughTransform::roughCorners(double limitAngle) {
  int limitAlpha = limitAngle * angleRes_ / (2 * pi);
  std::vector<Coords> corners(4);
  int c = 0;
  for(std::vector<Coords>::const_iterator i = coordsByValue_.begin(); i != coordsByValue_.end(); i++) {
    for(int cc = 0; cc < c; cc++)
      if (angleRes_/2 - abs(angleRes_ / 2 - abs(i->alpha - corners[cc].alpha))<= limitAlpha)
        goto next;
    corners[c] = *i;
    c++;
    if (c >= 4) return corners;
next:
    ;
  }
  abort(); // Sem jsme se neměli dostat. Příliš velký limitAngle?
}

QImage HoughTransform::visualise(void) const {
  double max = 0;
  for (int r = 0; r < radius_; r++)
    for (int alpha = 0; alpha < angleRes_; alpha++)
      if (max < get(r,alpha)) max = get(r,alpha);

  QImage v(angleRes_, radius_, QImage::Format_Indexed8);
  QVector<QRgb> graytable;
  for(int i = 0; i < 256; i++) graytable.push_back(qRgb(0,i,0));
  v.setColorTable(graytable);

  for (int r = 0; r < radius_; r++)
    for (int alpha = 0; alpha < angleRes_; alpha++)
      v.setPixel(alpha, r, get(r,alpha)*255/max);
  v.save("/tmp/hough.png");
  return v;
}

//   const uchar **lines = new *uchar[kernYsiz];
//   for(int yu = 0; yu <= src->height() - kernYsiz; y++) {
//     for(int l = 0; l < kernYsiz; l++)


//   }
//   validity = true;


//}

double *HoughTransform::operator[](int r) const {
  return data_ + r * angleRes_;
}

/*!
 * \brief Odhad polohy rohů tabule pomocí Houghovy transformace.
 * \param Zdrojový obrázek
 * \param Maximální velikost obrázku, z nějž se transformace počítá. (Zdrojový obrázek je přeškálován na tuto velikost.)
 * \param Úhlové rozlišení transformovaného obrázku
 * \return Odhad polohy rohů.
 */
QVector<QPointF> HoughTransform::guessCorners(const QImage src, const QSize &maxSize, int angleRes) {
  QImage imgscaled = src.scaled(maxSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
  HoughTransform ht(&imgscaled, angleRes);
  QVector<Coords> borders = QVector<Coords>::fromStdVector(ht.roughCorners());



}

HoughTransform::~HoughTransform() {
  delete []data_;
}

QLineF GT::houghLine(double r, double alpha, int rectWidth, int rectHeight) {
  int xo = rectWidth / 2;
  int yo = rectHeight / 2;
  double y = r * sin(alpha);
  double x = r * cos(alpha);
  QLineF start(xo+x, yo+y, xo+x-y, yo+y+x);
  QRectF rect(0,0,rectWidth,rectHeight);
  return intersectLineRect(start, rect);
}

QLineF GT::houghLine(HoughTransform::Coords &coords, const QSize &size) {
  return GT::houghLine(coords.r, coords.alpha, size.width(), size.height());
}

// Umístí přímku do obdélníku
QLineF GT::intersectLineRect(const QLineF &st, const QRectF &rect) {
  QLineF start = st;
  QPointF iss[3];
  int i = 0;
  // Půlnoční velkorysý odhad:
  double longEnough = fabs(rect.bottom()) + fabs(rect.top()) + fabs(rect.left()) + fabs(rect.right()) + fabs(start.x1()) + fabs(start.x2()) + fabs(start.y1()) + fabs(start.y2());
  start.setLength(longEnough);
  if (QLineF::BoundedIntersection == start.intersect(QLineF(rect.bottomLeft(),rect.bottomRight()), iss + i)) i++;
  if (QLineF::BoundedIntersection == start.intersect(QLineF(rect.bottomLeft(),rect.topLeft()), iss + i)) i++;
  if (QLineF::BoundedIntersection == start.intersect(QLineF(rect.topRight(),rect.bottomRight()), iss + i)) i++;
  if (QLineF::BoundedIntersection == start.intersect(QLineF(rect.topRight(),rect.topLeft()), iss + i)) i++;

  start.setLength(-longEnough);
  if (QLineF::BoundedIntersection == start.intersect(QLineF(rect.bottomLeft(),rect.bottomRight()), iss + i)) i++;
  if (QLineF::BoundedIntersection == start.intersect(QLineF(rect.bottomLeft(),rect.topLeft()), iss + i)) i++;
  if (QLineF::BoundedIntersection == start.intersect(QLineF(rect.topRight(),rect.bottomRight()), iss + i)) i++;
  if (QLineF::BoundedIntersection == start.intersect(QLineF(rect.topRight(),rect.topLeft()), iss + i)) i++;

  if (2 == i)
    return QLineF(iss[0],iss[1]);
  else return QLineF();
}
