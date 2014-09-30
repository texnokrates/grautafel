#include "gthoughtransform.h"
#include "gtconvolutionkernel.h"
#include <QImage>
#include <QtGlobal>
#include <QDebug>
//#include <QUuid>

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

/*!
 * \brief Overloaded method.
 */
double HoughTransform::get(int r, int alpha) const {
  return data_[r * angleRes_ + alpha];
}

/*!
 * \brief Access to Hough transform at individual points.
 * \param Hought transform coordinates.
 * \return Value of the Hough transform at a given point.
 */
double HoughTransform::get(const Coords c) const {
  return data_[c.r * angleRes_ +c.alpha];
}

/*!
 * \brief Constructs the Hough transform.
 * \param Source image.
 * \param Resolution of the angle coordinate
 * \param Margin (necessary to exclude false maxima)
 */
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

// Initialises the vector with coordinates sorted by value
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
 * \brief Guesses the board's borders' position
 *
 * Goes through the Hought transform from the brightest point.
 * Searches for the first four brightest points, while ignoring
 * anything in the +- 20 degrees around the points already found.
 *
 * \param Optional another limit for the minimum angle difference (in radians).
 * \return Guessed positions in Hough transform coordinates, sorted by angle starting with
 * what is thought to be the upper edge.
 * (N.B. This order is wrong if the center of the original image lies outside the board.)
 */
std::vector<HoughTransform::Coords> HoughTransform::roughEdges(double limitAngle) {
  int limitAlpha = limitAngle * angleRes_ / (2 * pi);
  std::vector<Coords> borders(4);
  int c = 0;
  for(std::vector<Coords>::const_iterator i = coordsByValue_.begin(); i != coordsByValue_.end(); i++) {
    for(int cc = 0; cc < c; cc++)
      if (angleRes_/2 - abs(angleRes_ / 2 - abs(i->alpha - borders[cc].alpha))<= limitAlpha)
        goto next;
    borders[c] = *i;
    c++;
    if (c >= 4) {
      // Bubblesort podle úhlu
      for (int k = 0; k < 4; k++)
        for (int j = k + 1; j < 4; j++)
          if((borders[k].alpha + (angleRes_ * 5) / 8) % angleRes_ > (borders[j].alpha + ((angleRes_ * 5) / 8)) % angleRes_) {
            Coords tmp = borders[k];
            borders[k] = borders[j];
            borders[j] = tmp;
          }
      return borders;
    }
next:
    ;
  }
  Q_UNREACHABLE(); // Sem jsme se neměli dostat. Příliš velký limitAngle?
}

/*!
 * \brief Visualisation of the Hough transform.
 * \return Image with the Hough transform.
 */
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
  v.save("/tmp/hough.png"); // FIXME odstranit
  return v;
}


double *HoughTransform::operator[](int r) const {
  return data_ + r * angleRes_;
}

/*!
 * \brief Makes a guess about the board corner positions using the Hough transform.
 * \param Source image
 * \param Maximum size of the image to transform. (Source image is scaled to this size.)
 * \param Angle resolution of the transformed image
 * \return Guessed positions of corners.
 *
 * Because the Hough tranform is computationally expensive (required time
 * is proportional to angleRes * (image diameter)**3), the original image is scaled down
 * maxSize. This has of course a consequence of lower accuracy (the border lines might
 * be shifted).
 */
QVector<QPointF> HoughTransform::guessCorners(const QImage src, const QSize &maxSize, int angleRes) {
  QImage imgscaled = src.scaled(maxSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
//  qDebug() << imgscaled.size();
//  imgscaled.save("/tmp/debug/" + QUuid::createUuid().toString() + ".png");
  HoughTransform ht(&imgscaled, angleRes);
//  ht.visualise().save("/tmp/debug/" + QUuid::createUuid().toString() + ".png");
  QVector<Coords> borders = QVector<Coords>::fromStdVector(ht.roughEdges());

  QVector<QPointF> corners(4);
  for (int i = 0; i < 4; i++) {
    QLineF a = houghLine(borders[i].r, borders[i].alpha * 2 * pi / angleRes, imgscaled.width(), imgscaled.height());
    QLineF b = houghLine(borders[(i + 1) % 4].r, borders[(i + 1) % 4].alpha * 2 * pi / angleRes, imgscaled.width(), imgscaled.height());
    QPointF intersection;
    if (QLineF::NoIntersection == a.intersect(b, &intersection)) {
      Q_UNREACHABLE();
    }
    corners[i] = intersection * (src.width() / (qreal)imgscaled.width());
  }
  return corners;
}

/*!
 * \brief Cleans up the Hought transform data.
 */
HoughTransform::~HoughTransform() {
  delete []data_;
}

/*!
 * \brief Transforms Hough transform coordinates to real line.
 * \param Hough transform r coordinate
 * \param Hough transform angle coordinate in radians
 *      (i.e. not in the integer coordinates as in Coords;
 *       that must be multiplied by 2*pi/angleRes)
 * \param width of the original image
 * \param height of the original image
 * \return a line fitted to the original image
 */

QLineF GT::houghLine(double r, double radAlpha, int rectWidth, int rectHeight) {
  int xo = rectWidth / 2;
  int yo = rectHeight / 2;
  double y = r * sin(radAlpha);
  double x = r * cos(radAlpha);
  QLineF start(xo+x, yo+y, xo+x-y, yo+y+x);
  QRectF rect(0,0,rectWidth,rectHeight);
//  qDebug() << "Hough coords" << r << ", " << radAlpha << "transformed to " << intersectLineRect(start, rect);
  return intersectLineRect(start, rect);
}

/*!
 * \brief Places a line into given rectangle
 * \param the line
 * \param the rectangle
 * \return QLineF beginning and ending at the edges of the rectangle.
 */
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

