#include "gtimageview.h"
#include "gtimage.h"
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QLineF>
#include <QPointF>
#include <QPen>



GTImageView::GTImageView(QWidget *parent) :
  QGraphicsView(parent)
{
  sc_ = new QGraphicsScene(this);
  for (int i = 0; i < 4; i++) {
      cornerItems_[i] = new CornerItem;
      sc_->addItem(cornerItems_[i]);
      cornerItems_[i]->setFlag(QGraphicsItem::ItemIsMovable);
    }
  // Nějaké výchozí počáteční postavení
  cornerItems_[0]->setPos(0,0);
  cornerItems_[1]->setPos(640, 0);
  cornerItems_[2]->setPos(640,480);
  cornerItems_[3]->setPos(0,480);

  QPen pen;
  pen.setColor(Qt::cyan);
  pen.setStyle(Qt::DashDotLine);
  for (int i = 0; i < 4; i++) {
      borderItems_[i] = sc_->addLine(QLineF(cornerItems_[i]->pos(),cornerItems_[(i+1)%4]->pos()));
      borderItems_[i]->setPen(pen);


    }
  // Zde je potřeba naconnectit signály
  for (int i = 0; i < 4; i++) {
      connect(cornerItems_[i], SIGNAL(xChanged()), this, SLOT(updateLines()));
      connect(cornerItems_[i], SIGNAL(yChanged()), this, SLOT(updateLines()));
    }

  setScene(sc_);
}

void GTImageView::updateLines(void) {
  for (int i = 0; i < 4; i++) {
      borderItems_[i]->setLine(QLineF(cornerItems_[i]->pos(), cornerItems_[(i+1)%4]->pos()));
    }
}

void GTImageView::CornerItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
  QPen pen;
  pen.setColor(Qt::red);
  painter->setPen(pen);

  painter->drawLine(-radius_, -radius_, radius_, radius_);
  painter->drawLine(radius_, -radius_, -radius_, radius_);
  painter->drawRect(boundingRect());

}

QRectF GTImageView::CornerItem::boundingRect(void) const {
  return QRectF(-1*radius_, -1*radius_, 2*radius_, 2*radius_);
}
