#ifndef GTHOUGHTRANSFORM_H
#define GTHOUGHTRANSFORM_H
// FIXME QObject je to proč?

//#include <QObject>
#include <vector>
#include <list>
class QImage;

class GTHoughTransform
    //: public QObject
{
//  Q_OBJECT
public:
  struct coords{
    int r;
    int alpha;
  };

private:
  std::vector<coords> coords_by_value;
  struct cmpstruct {
    GTHoughTransform *m;
    cmpstruct(GTHoughTransform *p){m = p;}
    bool operator()(const coords &a, const coords &b) {
      return m->get(a) < m->get(b);
    }
  };

  int topMargin, bottomMargin, leftMargin, rightMargin;
  int angleRes;
  int radius;
  double *data;
  bool validity; // FIXME smazat
  int originalHeight;
  int originalWidth;
  void set(int r, int alpha, double val);
  void add(int r, int alpha, double val);
  void coords_by_value_init(void);

public:
  coords coordsByValue(int n) { return coords_by_value[n]; }
  GTHoughTransform(const QImage *src, int angleResolution, int margin = 2);//, QObject *parent = 0);
  ~GTHoughTransform();
  bool valid() const {return validity;}
  int angleResolution(void) const {return angleRes;}
  int origHeight(void) const {return originalHeight;}
  int origWidth(void) const {return originalWidth;}
  double get(int r, int alpha) const;
  double get(const coords c) const;
  double *operator[](int r) const;
  QImage visualise(void) const;
  std::list<coords> roughCorners(void);


//signals:

//public slots:

};

#endif // GTHOUGHTRANSFORM_H
