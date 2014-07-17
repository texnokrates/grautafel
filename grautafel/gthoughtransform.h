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

public:
  explicit GTHoughTransform(QObject *parent = 0);
  GTHoughTransform(const QImage *src, int angleResolution, QObject *parent = 0);
  ~GTHoughTransform();
  bool valid() {return validity;}
  int angleResolution() {return angleRes;}
  double get(int r, int alpha);


signals:

public slots:

};

#endif // GTHOUGHTRANSFORM_H
