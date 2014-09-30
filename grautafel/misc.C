// Různý testovací, vývojový a odpadní kód

#include "misc.h"

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


#include <math.h>
#include <QLineF>



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
//#include "misc.moc"
