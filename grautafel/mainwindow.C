#include "mainwindow.h"
//#include "ui_mainwindow.h"
#include "gtmainwidget.h"
#define TESTIMG "/home/mmn/repo/grautafel/testy/Einstein_blackboard.jpg"
#define TEST2 "/home/mmn/repo/grautafel/testy/my_blackboard.jpg"
#define TEST3 "/home/mmn/repo/grautafel/testy/whiteboard_2.jpg"

// ================= test ===================
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

#include <QDir>
#include <QDebug>
#include <QTime>

#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>

using namespace GT;

class GraphicsScene : public QGraphicsScene {
protected:
  virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) {
    qDebug() << "coords:" << event->scenePos();
    QList<QGraphicsItem *> its = items();
    for(QList<QGraphicsItem *>::Iterator i = its.begin(); i != its.end(); i++) {

      if ((*i)->type() == QGraphicsLineItem::Type) {
        QGraphicsLineItem *it = qgraphicsitem_cast<QGraphicsLineItem *>(*i);
        it->setLine(QLineF(it->line().p1(),QPointF(event->scenePos())));
      }
    }
  }
};


// Umístí přímku do obdélníku
#include <math.h>
#include <QLineF>


// Maximálně zpraseno, opravit
QLineF intersectLineRect(QLineF start, QRectF rect) {
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

QLineF houghLine(double r, double alpha, int rectWidth, int rectHeight) {
  int xo = rectWidth / 2;
  int yo = rectHeight / 2;
  double y = r * sin(alpha);
  double x = r * cos(alpha);
  QLineF start(xo+x, yo+y, xo+x-y, yo+y+x);
  QRectF rect(0,0,rectWidth,rectHeight);
  return intersectLineRect(start, rect);
}

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
    std::vector<HoughTransform::coords> cns = ht->roughCorners();
    for (int i = 0; i < 4; i++)
      qDebug() << "Corner " << i << ": (" << cns[i].alpha << "," << cns[i].r << ")" << endl;
  }
};

#include <QTransform>
void testfun(QMainWindow *where) {
#if 0
  GTImage img(TESTIMG);
  QList<QColor> cols =
    img.getColorQuantiles(QRect(img.srcWidth()/3,img.srcHeight()/3,img.srcWidth()/3,img.srcHeight()/3),
                          QList<qreal>() << 0.2 << 0.5 << 0.8);
  QWidget *window = new QWidget;
  QVBoxLayout *layout = new QVBoxLayout;
  for(QList<QColor>::const_iterator i = cols.begin(); i != cols.end(); ++i) {
    QLabel *lab = new QLabel;
    QPalette pal = lab->palette();
    lab->setAutoFillBackground(true);
    pal.setColor(QPalette::Normal,QPalette::Background, *i);
    lab->setPalette(pal);
    lab->setText("Barva:" + (*i).name());

    layout->addWidget(lab);

  }
  window->setLayout(layout);
  window->show();

  // Another test
  QImage srcimg(TESTIMG);
  QLabel *win2 = new QLabel;
  QPixmap *pmap = new QPixmap;
  pmap->convertFromImage(srcimg);
  win2->setPixmap(*pmap);
  win2->show();
  QImage *edgeimg = edgePreview(&srcimg, QRect(3,3,srcimg.width(),srcimg.height()));
  QLabel *win3 = new QLabel;
  QPixmap *pmap2 = new QPixmap;
  pmap2->convertFromImage(*edgeimg);
  win3->setPixmap(*pmap2);
  win3->show();

  QTime t;
  t.start();
  GTHoughTransform *ht = new GTHoughTransform(&srcimg, 500);
  qDebug("Time elapsed: %d ms", t.elapsed());
  QPixmap *vizmap = new QPixmap;
  QImage vizimg = ht->visualise();
  vizimg.save("/tmp/test.png");
  vizmap->convertFromImage(vizimg);
  QLabel *win4 = new QLabel;
  win4->setPixmap(*vizmap);
  win4->show();

  qDebug() << QDir::currentPath();

  LineGraphicsScene *scene = new LineGraphicsScene;
  QPixmap obr(TESTIMG);
  scene->addPixmap(obr);

  HoughGraphicsScene *hs = new HoughGraphicsScene(QImage(TESTIMG), 300);
  QObject::connect(hs, SIGNAL(lineClicked(QLineF&)), scene, SLOT(setLine(QLineF&)));

  QGraphicsView *hview = new QGraphicsView(hs);
  hview->setTransform(QTransform::fromScale(2,2));
  hview->show();

  QGraphicsView *view = new QGraphicsView(scene);
  view->show();
#endif

  ImageView * iv = new ImageView();
  iv->show();
  ImageListWidget *iw = new ImageListWidget;
  iw->show();
  iw->addItems(QList<QString>() << TESTIMG << TEST2 << TEST3);
  QObject::connect(iw, SIGNAL(selectedImage(Image*)),
                   iv, SLOT(setImage(Image*)));
}
// ================= test ===================

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent)//,
//    ui(new Ui::MainWindow)
{
//    testfun(this);
//    ui->setupUi(this);
  setCentralWidget(new MainWidget());

}

MainWindow::~MainWindow() {
//    delete ui;
}

#include "mainwindow.moc"
