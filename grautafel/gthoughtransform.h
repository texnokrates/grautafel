#ifndef GTHOUGHTRANSFORM_H
#define GTHOUGHTRANSFORM_H
// FIXME QObject je to proč?

//#include <QObject>
#include <vector>
#include <list>
class QImage;

namespace GT {
  class HoughTransform
    //: public QObject
  {
//  Q_OBJECT
  public:
    struct coords {
      int r;
      int alpha;
    };

  private:
    std::vector<coords> coordsByValue_;
    struct cmpstruct { // Řadíme sestupně
      HoughTransform *m;
      cmpstruct(HoughTransform *p) {
        m = p;
      }
      bool operator()(const coords &a, const coords &b) {
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

  public:
    coords coordsByValue(int n) {
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
    double get(const coords c) const;
    int radius(void) const {
      return radius_;
    }
    double *operator[](int r) const;
    QImage visualise(void) const;
    std::vector<coords> roughCorners(double limitAngle = 0.55);


//signals:

//public slots:

  };
}
#endif // GTHOUGHTRANSFORM_H
