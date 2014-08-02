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
      cornerItems_[i] = new CornerItem;
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
  setZoom(width()/cbrect.width());
  ensureVisible(transform().mapRect(cornersBoundingRect()),0,0);
}

void GTImageView::setZoom(qreal factor){
  setTransform(QTransform::fromScale(factor, factor));
  for(int i = 0; i < 4; i++) {
      cornerItems_[i]->setScale(1./factor);
    }
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
  qreal minX = cornerItems_[0]->x(), maxX = cornerItems_[0]->x();
  qreal minY = cornerItems_[0]->y(), maxY =  cornerItems_[0]->y();
  for(int i = 1; i < 4; i++){
      minX = qMin(minX, cornerItems_[i]->x());
      maxX = qMax(maxX, cornerItems_[i]->x());
      minY = qMin(minY, cornerItems_[i]->y());
      maxY = qMax(maxX, cornerItems_[i]->y());
    }
  return QRectF(minX, minY, maxX-minX, maxY-minY);
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
