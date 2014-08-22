#ifndef GTIMAGEVIEW_H
#define GTIMAGEVIEW_H

class GTImage;
class QGraphicsScene;
#include <QGraphicsView>
#include <QGraphicsObject>
#include <QVector>
#include <QPointF>

class GTCornerItem : public QGraphicsObject{
    Q_OBJECT // Je to třeba?
    static const int radius_ = 5;
  public:
    explicit GTCornerItem(QGraphicsItem *parent = 0) : QGraphicsObject(parent){}
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
    QRectF boundingRect(void) const;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *);
  signals:
    void requestBoundingRectUpdate();
};

class GTImageView : public QGraphicsView
{
  Q_OBJECT

private:
  GTCornerItem *cornerItems_[4];
  QGraphicsLineItem *borderItems_[4];
  GTImage *img_;
  QGraphicsPixmapItem *pixmapItem_;
  QGraphicsScene *sc_;

  void saveChanges(void); // uloží změny před přepnutím na další obrázek
public:
  explicit GTImageView(QWidget *parent = 0);
  QVector<QPointF> corners(void) const;
  bool setCorners(const QVector<QPointF> &);
  QRectF cornersBoundingRect(void) const;
  void setZoom(qreal factor);
  qreal zoom(void) const;


signals:
  // TODO dodělat signály pro stavový řádek
  void cornersChanged(void);

public slots:
  void original(void);
  void transformed(void);
  void updateSceneRect(void);
  void setImage(GTImage *); // uloží změny a načte nový obrázek
  void updateLines(void); // Přepočte polohy hranic
  void clear(void);
#if 0 // Doimplementovat
 void reset(void); // zruší změny na současném obrázku (načte hodnoty znova z něj)

  void setTopLeftCorner(QPointF);
  void setTopRightCorner(QPointF);
  void setBottomLeftCorner(QPointF);
  void setBottomRightCorner(QPointF);
#endif
};

#endif // GTIMAGEVIEW_H
