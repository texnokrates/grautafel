#ifndef GTHOUGHTRANSFORM_H
#define GTHOUGHTRANSFORM_H

#include <QObject>
class QImage;

class GTHoughTransform : public QObject
{
  Q_OBJECT
  int angleRes;
  int radius;
  double *data;
  bool validity;
  int originalHeight;
  int originalWidth;
  void set(int r, int alpha, double val);
  void add(int r, int alpha, double val);

public:
  explicit GTHoughTransform(QObject *parent = 0);
  GTHoughTransform(const QImage *src, int angleResolution, QObject *parent = 0);
  ~GTHoughTransform();
  bool valid() const {return validity;}
  int angleResolution(void) {return angleRes;}
  int origHeight(void) {return originalHeight;}
  int origWidth(void) {return originalWidth;}
  double get(int r, int alpha) const;
  QImage visualise(void) const;


signals:

public slots:

};

#endif // GTHOUGHTRANSFORM_H
