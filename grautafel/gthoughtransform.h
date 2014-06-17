#ifndef GTHOUGHTRANSFORM_H
#define GTHOUGHTRANSFORM_H

#include <QObject>
class QImage;

class GTHoughTransform : public QObject
{
  Q_OBJECT
  int angleRes;
  int ysiz;
  int *data;
  bool validity;

public:
  explicit GTHoughTransform(QObject *parent = 0);
  GTHoughTransform(QImage *src, int angleResolution, QObject *parent = 0);
  ~GTHoughTransform();
  bool valid();
  int angleResolution();
  int height();


signals:

public slots:

};

#endif // GTHOUGHTRANSFORM_H
