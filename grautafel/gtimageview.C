#include "gtimageview.h"
#include "gtimage.h"
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QLineF>
#include <QPointF>
#include <QPen>
#include <cmath>
#include <QScrollBar>
#include <QTransform>
#include <QAction>
#include <QIcon>

using namespace GT;

ImageView::ImageView(QWidget *parent) :
  QGraphicsView(parent) {
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
    connect(cornerItems_[i], SIGNAL(requestBoundingRectUpdate()),
            this, SLOT(updateSceneRect(void)));
  }

  zoomInAction = new QAction(QIcon::fromTheme("zoom-in"), trUtf8("Zoom in"), this);
  zoomOutAction = new QAction(QIcon::fromTheme("zoom-out"), trUtf8("Zoom out"), this);
  zoomFitToWidthAction = new QAction(trUtf8("Fit to width"), this);
  connect(zoomInAction, SIGNAL(triggered()),
          this, SLOT(zoomIn()));
  connect(zoomOutAction, SIGNAL(triggered()),
          this, SLOT(zoomOut()));
  connect(zoomFitToWidthAction, SIGNAL(triggered()),
          this, SLOT(zoomToWidth()));

  setDragMode(QGraphicsView::ScrollHandDrag);

  setScene(sc_);
}

bool ImageView::setCorners(const QVector<QPointF> &corners) {
  for(int i = 0; i < 4; i++) {
    cornerItems_[i]->setX(corners[i].x());
    cornerItems_[i]->setY(corners[i].y());
  }
  return true;
}

void ImageView::setImage(Image *newimg) {
  if (img_ == newimg) return;
  saveChanges();

  img_ = newimg;
  setCorners(newimg->corners());
  saveAndEmitPreviewStateChange(NotPreview);

  pixmapItem_->setPixmap(QPixmap::fromImage(img_->srcImage()));
  updateSceneRect();
  if (0 == img_->lastZoom()) {
    setZoom(fitToWidthZoom());
    ensureVisible(transform().mapRect(cornersBoundingRect()),15,15);
  } else {
    setZoom(img_->lastZoom());
    centerOn(img_->lastViewPoint());
  }
  emit imageChanged(newimg);
}

qreal ImageView::fitToWidthZoom(void) const {
  QRectF cbrect = cornersBoundingRect();
  return qMin(1., width()/cbrect.width());
}

void ImageView::setZoom(qreal factor) {
  setTransform(transform() * QTransform::fromScale(factor/zoom(), factor/zoom()));
  for(int i = 0; i < 4; i++) {
    cornerItems_[i]->setScale(1./factor);
  }
  emit zoomChanged(zoom());
}

void ImageView::zoomIn(qreal factor) {
  setZoom(zoom() * factor);
}

void ImageView::zoomOut(qreal divisor) {
  setZoom(zoom() / divisor);
}

void ImageView::zoomToWidth(void) {
  setZoom(fitToWidthZoom());
}

void ImageView::updateSceneRect() {
  setSceneRect(cornersBoundingRect().adjusted(-15,-15,30,30));
}


/*!
 * \brief GTImageView::zoom
 * \return The current zoom factor.
 */
qreal ImageView::zoom(void) const {
  return std::sqrt(transform().det());
}

QVector<QPointF> ImageView::corners(void) const {
  QVector<QPointF> c(4);
  for(int i = 0; i < 4; i++) {
    c[i] = cornerItems_[i]->pos();
  }
  return c;
}

void ImageView::saveChanges() {
  if(img_) {
    img_->setCorners(corners());
    img_->setLastViewPoint(center());
    img_->setLastZoom(zoom());
  }
}

/*!
 * Returns (approximate) center of current visible area.
 *
 */

QPointF ImageView::center(void) const {
  QPointF c;
  // Prasárna, leč zdá se, že QGraphicsView nic lepšího neposkytuje
  {
    int xStep = horizontalScrollBar()->pageStep();
    int xMax = horizontalScrollBar()->maximum();
    int xMin = horizontalScrollBar()->minimum();
    int xVal = horizontalScrollBar()->value();
    qreal xr = (xVal - xMin + 0.5 * xStep) / ((qreal)(xMax - xMin + xStep));
    c.setX(sceneRect().x() + sceneRect().width() * xr);
  }
  {
    int yStep = verticalScrollBar()->pageStep();
    int yMax = verticalScrollBar()->maximum();
    int yMin = verticalScrollBar()->minimum();
    int yVal = verticalScrollBar()->value();
    qreal yr = (yVal - yMin + 0.5 * yStep) / ((qreal)(yMax - yMin + yStep));
    c.setY(sceneRect().y() + sceneRect().height() * yr);
  }
  return c;
}

QRectF ImageView::cornersBoundingRect() const {
  if (!img_) return QRectF(0,0,640,480);
  // Následující možno předělat triviálně pomocí metod QPolygon
  qreal minX = 0, maxX = img_->srcImage().width();
  qreal minY = 0, maxY = img_->srcImage().height();
  for(int i = 0; i < 4; i++) {
    minX = qMin(minX, cornerItems_[i]->x());
    maxX = qMax(maxX, cornerItems_[i]->x());
    minY = qMin(minY, cornerItems_[i]->y());
    maxY = qMax(maxY, cornerItems_[i]->y());
  }
  return QRectF(minX, minY, maxX-minX, maxY-minY);
}

void ImageView::updateLines(void) {
  for (int i = 0; i < 4; i++) {
    borderItems_[i]->setLine(QLineF(cornerItems_[i]->pos(), cornerItems_[(i+1)%4]->pos()));
  }
  if (previewState_ != NotPreview) saveAndEmitPreviewStateChange(OldPreview); // FIXME tady je to dost nepřehledně zastrčené
}

void CornerItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
  QPen pen;
  pen.setColor(Qt::red);
  painter->setPen(pen);

  painter->drawLine(-radius_, -radius_, radius_, radius_);
  painter->drawLine(radius_, -radius_, -radius_, radius_);
  painter->drawRect(boundingRect());

}

QRectF CornerItem::boundingRect(void) const {
  return QRectF(-1*radius_, -1*radius_, 2*radius_, 2*radius_);
}

void CornerItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
  QGraphicsObject::mouseReleaseEvent(event);
  emit requestBoundingRectUpdate();
}

void ImageView::clear(void) {
  pixmapItem_->setPixmap(QPixmap());
  img_ = 0;
  saveAndEmitPreviewStateChange(NotPreview);
}

void ImageView::original_(void) {
  setTransform(QTransform::fromScale(zoom(), zoom()));

  if(img_ && img_->lastZoom()) {
    setZoom(img_->lastZoom());
    centerOn(img_->lastViewPoint());
  }
  saveAndEmitPreviewStateChange(NotPreview);
}

void ImageView::saveAndEmitPreviewStateChange(PreviewState nps) {
  if (previewState_ != nps)
    emit newPreviewState((int) (previewState_ = nps));
}

void ImageView::transformed_(void) {
  if(img_) {
    qreal oldZoom = zoom();
    QTransform qtt = quadToTarget();
    qreal qttScale = std::sqrt(qtt.determinant());
    setTransform(qtt * QTransform::fromScale(oldZoom/qttScale, oldZoom/qttScale));
    saveAndEmitPreviewStateChange(NewPreview);
  }
  // TODO nenechat stejný zoom jako při nenáhledu?
}

void ImageView::setPreview(int on) {
  if(on == (int) NotPreview) original_();
  if(on == (int) NewPreview) transformed_();
}

void ImageView::setPreview(bool isPreview) {
  setPreview(isPreview ? (int) NewPreview : (int) NotPreview);
}

QTransform ImageView::quadToTarget() const {
  if (!img_) return QTransform();
  else {
    QPolygonF photoQuad(corners());
    QPolygonF targetQuad(img_->targetRect());
    targetQuad.pop_back(); // Workaround for a bug in QTransform::squareToQuad().
    //qDebug() << photoQuad;
    //qDebug() << targetQuad;
    QTransform tr;
    if (false == QTransform::quadToQuad(photoQuad, targetQuad, tr)) {
      qWarning("Failed to find the transform (perhaps the source polygon is non-convex). Setting to identity.");
    }
    //qDebug() << tr;
    return tr;
  }
}
