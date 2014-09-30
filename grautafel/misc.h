#ifndef MISC_H
#define MISC_H

/* Různé vývojové/testovací třídy a další odpad */

#define TESTIMG "/home/mmn/repo/grautafel/testy/Einstein_blackboard.jpg"
#define TEST2 "/home/mmn/repo/grautafel/testy/my_blackboard.jpg"
#define TEST3 "/home/mmn/repo/grautafel/testy/whiteboard_2.jpg"

#include "gtimage.h"
#include "gtconvolutionkernel.h"
#include <QColor>
#include <QLabel>
#include <QImage>
#include <QRect>
#include <QPalette>
#include <QVBoxLayout>
#include "gthoughtransform.h"
#include <QGraphicsScene>
#include "gtimageview.h"
#include "gtimagelistwidget.h"
class QMainWindow;
#include <QDir>
#include <QDebug>
#include <QTime>

#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>

namespace GT {

static const double pi = 3.14159265358979323846;

class LineGraphicsScene : public QGraphicsScene {
  Q_OBJECT
  QGraphicsLineItem *theLineItem;
public:
  LineGraphicsScene(QObject *parent = 0) : QGraphicsScene(parent) {
    QLineF line();
    theLineItem = addLine(QLineF());
    theLineItem->setZValue(5);
  }

public slots:
  void setLine(QLineF &line) {
    qDebug() << "caught a line:" << line;
    theLineItem->setLine(line);
  }
};


class HoughGraphicsScene : public QGraphicsScene {
  Q_OBJECT
  HoughTransform *ht;
public:

signals:
  void lineClicked(QLineF &line);

protected:
  virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    if (event->buttons()) {
      qDebug() << "coords:" << event->scenePos();
      QLineF l = houghLine(event->scenePos().y(), 2*pi*event->scenePos().x()/ht->angleResolution(),ht->origWidth(), ht->origHeight());
      emit lineClicked(l);
    }
  }
public:
  HoughGraphicsScene(const QImage &src, int angleResolution, QObject *parent = 0) {
    ht = new HoughTransform(&src, angleResolution, 2);
    QPixmap pixmap;
    pixmap.convertFromImage(ht->visualise());
    addPixmap(pixmap);
    std::vector<HoughTransform::Coords> cns = ht->roughEdges();
    for (int i = 0; i < 4; i++)
      qDebug() << "Corner " << i << ": (" << cns[i].alpha << "," << cns[i].r << ")" << endl;
  }
};

void testfun(QMainWindow *where);


} // namespace GT
#endif // MISC_H
