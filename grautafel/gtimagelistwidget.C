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

void GTImageItem::unselect(){
  drawUnselected();
}

GTImageListWidget::GTImageListWidget(QWidget *parent) :
  QWidget(parent)
{
  layout = new QVBoxLayout;
  setLayout(layout);
  selected = 0;
}

bool GTImageListWidget::addItem(const QString & filename){
  GTImageItem *item = new GTImageItem(filename);
  // FIXME co když se nahrání nezdaří?

  items.append(item);
  layout->addWidget(item);

  QObject::connect(item, SIGNAL(requestSelection(GTImageItem*)), this, SLOT(selectImage(GTImageItem*)));
  return true;
}

bool GTImageListWidget::addItems(const QList<QString> &filenames){
  bool ok = true;
  for(QList<QString>::ConstIterator i = filenames.constBegin(); i != filenames.constEnd(); i++)
    if(!addItem(*i)) ok = false;
  return ok;
}

void GTImageListWidget::selectImage(GTImageItem *item) {
  // TODO ověřit, že je obsažen
  if(selected == item) return;
  if(selected) selected->unselect();
  selected = item;
  emit selectedImage(selected->image());
}

