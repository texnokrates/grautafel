#include "gtimageview.h"
#include "gtimage.h"
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QLineF>
#include <QPointF>
#include <QPen>
#include <cmath>


GTImageView::GTImageView(QWidget *parent) :
  QGraphicsView(parent)
{
  img_ = 0;
  sc_ = new QGraphicsScene(this);
  for (int i = 0; i < 4; i++) {
      cornerItems_[i] = new GTCornerItem;
      sc_->addItem(cornerItems_[i]);
      cornerItems_[i]->setFlag(QGraphicsItem::ItemIsMovable);
      cornerItems_[i]->setZValue(3);
    }
  // Nějaké výchozí počáteční postavení
  cornerItems_[0]->setPos(0,0);
  cornerItems_[1]->setPos(640, 0);
  cornerItems_[2]->setPos(640,480);
  cornerItems_[3]->setPos(0,480);

  pixmapItem_ = sc_->addPixmap(QPixmap()); //TODO dát sem nějakou tapetu

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
      connect(cornerItems_[i], SIGNAL(requestBoundingRectUpdate()),
              this, SLOT(updateSceneRect(void)));
    }

  setScene(sc_);
}

bool GTImageView::setCorners(const QVector<QPointF> &corners){
  for(int i = 0; i < 4; i++){
      cornerItems_[i]->setX(corners[i].x());
      cornerItems_[i]->setY(corners[i].y());
    }
  return true;
}

void GTImageView::setImage(GTImage *newimg) {
  if (img_ == newimg) return;
  saveChanges();

  img_ = newimg;
  setCorners(newimg->corners());

  pixmapItem_->setPixmap(QPixmap::fromImage(img_->srcImage()));
  emit cornersChanged();
  QRectF cbrect = cornersBoundingRect();
  updateSceneRect();
  setZoom(qMin(1.,width()/cbrect.width()));
  ensureVisible(transform().mapRect(cornersBoundingRect()),15,15);
}

void GTImageView::setZoom(qreal factor){
  setTransform(QTransform::fromScale(factor, factor));
  for(int i = 0; i < 4; i++) {
      cornerItems_[i]->setScale(1./factor);
    }
}

void GTImageView::updateSceneRect(){
  setSceneRect(cornersBoundingRect().adjusted(-15,-15,30,30));
}

qreal GTImageView::zoom(void) const {
  return std::sqrt(transform().det());
}

QVector<QPointF> GTImageView::corners(void) const {
  QVector<QPointF> c(4);
  for(int i = 0; i < 4; i++){
      c[i] = cornerItems_[i]->pos();
    }
  return c;
}

void GTImageView::saveChanges() {
  if(img_) {
      img_->setCorners(corners());
    }
}

QRectF GTImageView::cornersBoundingRect() const {
  if (!img_) return QRectF(0,0,640,480);
  qreal minX = 0, maxX = img_->srcImage().width();
  qreal minY = 0, maxY = img_->srcImage().height();
  for(int i = 0; i < 4; i++){
      minX = qMin(minX, cornerItems_[i]->x());
      maxX = qMax(maxX, cornerItems_[i]->x());
      minY = qMin(minY, cornerItems_[i]->y());
      maxY = qMax(maxY, cornerItems_[i]->y());
    }
  return QRectF(minX, minY, maxX-minX, maxY-minY);
}

void GTImageView::updateLines(void) {
  for (int i = 0; i < 4; i++) {
      borderItems_[i]->setLine(QLineF(cornerItems_[i]->pos(), cornerItems_[(i+1)%4]->pos()));
    }
}

void GTCornerItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
  QPen pen;
  pen.setColor(Qt::red);
  painter->setPen(pen);

  painter->drawLine(-radius_, -radius_, radius_, radius_);
  painter->drawLine(radius_, -radius_, -radius_, radius_);
  painter->drawRect(boundingRect());

}

QRectF GTCornerItem::boundingRect(void) const {
  return QRectF(-1*radius_, -1*radius_, 2*radius_, 2*radius_);
}

void GTCornerItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event){
  QGraphicsObject::mouseReleaseEvent(event);
  emit requestBoundingRectUpdate();
}

void GTImageView::clear(void) {
  pixmapItem_->setPixmap(QPixmap());
  img_ = 0;
}
