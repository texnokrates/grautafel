#ifndef GTHOUGHTRANSFORM_H
#define GTHOUGHTRANSFORM_H
// FIXME QObject je to proč?

#include <vector>
#include <QVector>
#include <QPointF>
#include <QLineF>
#include <QRectF>
#include <QSize>
#include <list>
class QImage;

namespace GT {
  /*!
   * \brief Class holding the Hough transform of given image.
   */
  class HoughTransform
  {
  public:
    struct Coords {
      int r;
      int alpha;
    };

    Coords coordsByValue(int n) {
      return coordsByValue_[n];
    }
    HoughTransform(const QImage *src, int angleResolution, int margin = 2);//, QObject *parent = 0);
    ~HoughTransform();
    int angleResolution(void) const {
      return angleRes_;
    }
    int origHeight(void) const {
      return originalHeight_;
    }
    int origWidth(void) const {
      return originalWidth_;
    }
    double get(int r, int alpha) const;
    double get(const Coords c) const;
    int radius(void) const {
      return radius_;
    }
    QImage visualise(void) const;
    std::vector<Coords> roughCorners(double limitAngle = 0.55);

    static QVector<QPointF> guessCorners(const QImage img,
                                         const QSize &maxSize = QSize(480, 320),
                                         int angleRes = 360);


  private:
    std::vector<Coords> coordsByValue_;
    struct cmpstruct { // Řadíme sestupně
      HoughTransform *m;
      cmpstruct(HoughTransform *p) {
        m = p;
      }
      bool operator()(const Coords &a, const Coords &b) {
        return m->get(a) > m->get(b);
      }
    };

    int topMargin_, bottomMargin_, leftMargin_, rightMargin_;
    int angleRes_;
    int radius_;
    double *data_;
    int originalHeight_;
    int originalWidth_;
    void set(int r, int alpha, double val);
    void add(int r, int alpha, double val);
    void coords_by_value_init(void);
    double *operator[](int r) const;

  };

  QLineF houghLine(double r, double alpha, int rectWidth, int rectHeight);
  QLineF houghLine(HoughTransform::Coords &coords, const QSize &size);
  QLineF intersectLineRect(const QLineF &start, const QRectF &rect);
}
#endif // GTHOUGHTRANSFORM_H
