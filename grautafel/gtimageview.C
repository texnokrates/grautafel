#include "gtimageview.h"
#include "gtimage.h"
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QLineF>
#include <QPointF>



GTImageView::GTImageView(QWidget *parent) :
  QWidget(parent)
{
  sc_ = new QGraphicsScene(this);
  for (int i = 0; i < 4: i++) {
      cornerItems_[i] = new CornerItem;
      sc_->addItem(cornerItems_[i]);
    }
  // Nějaké výchozí počáteční postavení
  cornerItems_[0]->setPos(0,0);
  cornerItems_[1]->setPos(640, 0);
  cornerItems_[2]->setPos(640,480);
  cornerItems_[3]->setPos(0,480);

  for (int i = 0; i < 4; i++) {
      borderItems_[i] = sc_->addLine(QLineF(cornerItems_[i]->pos(),cornerItems_[(i+1)%4]->pos()));

    }
  // Zde je potřeba naconnectit signály
  for (int i = 0; i < 4; i++) {
      connect(cornerItems_[i], SIGNAL(xChanged()), this, SLOT(updateLines()));
      connect(cornerItems_[i], SIGNAL(yChanged()), this, SLOT(updateLines()));
    }
}

void GTImageView::updateLines(void) {
  for (int i = 0; i < 4; i++) {
      borderItems_[i]->setLine(QLineF(cornerItems_[i]->pos()), QLineF(cornerItems_[(i+1)%4]->pos()));
    }
}
