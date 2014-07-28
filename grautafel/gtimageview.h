#ifndef GTIMAGEVIEW_H
#define GTIMAGEVIEW_H

#include <QWidget>
class GTImage;
class QGraphicsScene;
class QGraphicsView;
#include <QGraphicsObject>
class GTImageViewCornerItem : public QGraphicsObject{
  Q_OBJECT
  explicit GTImageObject(void);
};

class GTImageView : public QWidget
{
  Q_OBJECT
  class CornerItem : public QGraphicsObject{
  //  Q_OBJECT // Je to třeba?
  public:
    explicit CornerItem(QGraphicsItem *parent = 0) : QGraphicsItem(parent){}
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
  };

  CornerItem *cornerItems_[4];
  QGraphicsLineItem *borderItems_[4];
  GTImage *img_;
  QGraphicsScene *sc_;
  QGraphicsView *v_;

public:
  explicit GTImageView(QWidget *parent = 0);

signals:
  // Todo dodělat signály pro stavový řádek
  void cornersChanged(void);

public slots:
  bool setImage(GTImage *); // uloží změny a načte nový obrázek
  void reset(void); // zruší změny na současném obrázku (načte hodnoty znova z něj)
  void updateLines(void); // Přepočte polohy hranic

  void setTopLeftCorner(QPointF);
  void setTopRightCorner(QPointF);
  void setBottomLeftCorner(QPointF);
  void setBottomRightCorner(QPointF);
};

#endif // GTIMAGEVIEW_H
