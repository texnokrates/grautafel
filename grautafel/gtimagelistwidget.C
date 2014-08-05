#include "gtimagelistwidget.h"
#include <QDebug>
#include <QFileDialog>
#include <gtimage.h>
#include <QHBoxLayout>
#include <QLabel>
#include <QAction>

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
  openAction = new QAction(trUtf8("&Open"), this);
  moveUpAction = new QAction(trUtf8("Move &up"), this);
  moveDownAction = new QAction(trUtf8("Move &down"), this);
  deleteAction = new QAction(trUtf8("D&elete"), this);

  connect(openAction, SIGNAL(triggered()),
          this, SLOT(startOpenDialog()));
  connect(moveUpAction, SIGNAL(triggered()),
          this, SLOT(moveSelectedUp()));
  connect(moveDownAction, SIGNAL(triggered()),
          this, SLOT(moveSelectedDown()));
  connect(deleteAction, SIGNAL(triggered()),
          this, SLOT(deleteSelected()));
  setMinimumWidth(GTImage::ThumbnailWidth+6);
}

bool GTImageListWidget::addItem(const QString & filename){
  GTImageItem *item = new GTImageItem(filename);
  if(!item->image()->isOk()){
      delete item;
      return false;
    }

  items.append(item);
  layout->addWidget(item);

  QObject::connect(item, SIGNAL(requestSelection(GTImageItem*)), this, SLOT(selectImage(GTImageItem*)));
  return true;
}

void GTImageListWidget::moveSelectedUp(void){
  if(!selected) return;
  int i = items.indexOf(selected);
  Q_ASSERT(i >= 0);
  if(i == 0) return; //Výše už to nepůjde
  QLayoutItem *taken = layout->takeAt(i-1);
  layout->insertItem(i, taken);
  GTImageItem *taken2 = items.at(i-1);
  items.removeAt(i-1);
  items.insert(i, taken2);
}

void GTImageListWidget::deleteSelected(void){
  if(!selected) return;
  int i = items.indexOf(selected);
  Q_ASSERT(i >= 0);
  items.removeAt(i);
  layout->removeWidget(selected);
  delete selected;
  selected = 0;
  if (0 == items.count()) {
      emit emptied();
      return;
    }
  if (items.count() == i) items[i-1]->setFocus(Qt::OtherFocusReason);
  else emit items[i]->setFocus(Qt::OtherFocusReason);
}

void GTImageListWidget::moveSelectedDown(void){
  if(!selected) return;
  int i = items.indexOf(selected);
  Q_ASSERT(i >= 0);
  if(i == items.count() - 1) return; //Níže už to nepůjde
  QLayoutItem *taken = layout->takeAt(i+1);
  layout->insertItem(i, taken);
  GTImageItem *taken2 = items.at(i+1);
  items.removeAt(i+1);
  items.insert(i, taken2);
}

bool GTImageListWidget::addItems(const QStringList &filenames){
  bool ok = true;
  for(QList<QString>::ConstIterator i = filenames.constBegin(); i != filenames.constEnd(); i++)
    if(!addItem(*i)) ok = false;
  return ok;
}

void GTImageListWidget::startOpenDialog() {
  QStringList filesToOpen = QFileDialog::getOpenFileNames(this, trUtf8("Open images"));
  if (filesToOpen.isEmpty()) return;
  addItems(filesToOpen);
}

void GTImageListWidget::selectImage(GTImageItem *item) {
  // TODO ověřit, že je obsažen
  if(selected == item) return;
  if(selected) selected->unselect();
  selected = item;
  emit selectedImage(selected->image());
}

