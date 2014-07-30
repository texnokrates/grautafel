#include "gtimagelistwidget.h"
#include <QDebug>

GTImageItem::GTImageItem(const QString &srcname, QWidget *parent) :
  QFrame(parent)
{
  const int ls = 3;
  img_ = new GTImage(srcname, this);
  thumbnail_ = new QLabel;
  thumbnail_->setPixmap(img_->thumbnail());
  layout_ = new QVBoxLayout();
  layout_->setSpacing(0);
  layout_->setContentsMargins(0,0,0,0);
  layout_->addStretch();
  layout_->addWidget(thumbnail_);
  layout_->addStretch();
  setLayout(layout_);
  setLineWidth(ls);
  drawUnselected();
  setFixedSize(2*frameWidth()+GTImage::ThumbnailWidth, 2*frameWidth()+GTImage::ThumbnailHeight);
  setFocusPolicy(Qt::StrongFocus);
}

void GTImageItem::drawSelected()
{
  setFrameStyle(QFrame::Panel | QFrame::Sunken);
}

void GTImageItem::focusInEvent(QFocusEvent *event) {
  drawSelected();
  qDebug() << "Focus on GTImageItem at " <<this;
  emit requestSelection(this);
}

void GTImageItem::drawUnselected()
{
  setFrameStyle(QFrame::Panel | QFrame::Raised);
}



GTImageListWidget::GTImageListWidget(QWidget *parent) :
  QWidget(parent)
{



}
