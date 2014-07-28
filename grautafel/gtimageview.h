#ifndef GTIMAGEVIEW_H
#define GTIMAGEVIEW_H

class GTImage;
class QGraphicsScene;
#include <QGraphicsView>
#include <QGraphicsObject>

class GTImageView : public QGraphicsView
{
  Q_OBJECT
  class CornerItem : public QGraphicsObject{
  //  Q_OBJECT // Je to třeba?
    static const int radius_ = 5;
  public:
    explicit CornerItem(QGraphicsItem *parent = 0) : QGraphicsObject(parent){}
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
    QRectF boundingRect(void) const;
  };

  CornerItem *cornerItems_[4];
  QGraphicsLineItem *borderItems_[4];
  GTImage *img_;
  QGraphicsScene *sc_;

public:
  explicit GTImageView(QWidget *parent = 0);

signals:
  // Todo dodělat signály pro stavový řádek
  void cornersChanged(void);

public slots:
  void updateLines(void); // Přepočte polohy hranic
#if 0 // Doimplementovat
  bool setImage(GTImage *); // uloží změny a načte nový obrázek
  void reset(void); // zruší změny na současném obrázku (načte hodnoty znova z něj)

  void setTopLeftCorner(QPointF);
  void setTopRightCorner(QPointF);
  void setBottomLeftCorner(QPointF);
  void setBottomRightCorner(QPointF);
#endif
};

#endif // GTIMAGEVIEW_H
