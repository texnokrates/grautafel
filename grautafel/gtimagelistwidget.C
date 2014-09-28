#include "gtimagelistwidget.h"
#include <QDebug>
#include <QFileDialog>
#include "gtimage.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QAction>
#include "pagedialog.h"
#include <QPdfWriter>
#include <QPainter>
#include <QErrorMessage>

using namespace GT;

ImageItem::ImageItem(const QString &srcname, QWidget *parent, const Image::PageSettings & opt) :
  QFrame(parent) {
  const int ls = 3;
  img_ = new Image(srcname, this, opt);
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
  setFixedSize(2*frameWidth()+Image::ThumbnailWidth, 2*frameWidth()+Image::ThumbnailHeight);
  setFocusPolicy(Qt::StrongFocus);
}

void ImageItem::drawSelected() {
  setFrameStyle(QFrame::Panel | QFrame::Sunken);
}

void ImageItem::focusInEvent(QFocusEvent *event) {
  drawSelected();
  qDebug() << "Focus on GTImageItem at " <<this;
  emit requestSelection(this);
}

void ImageItem::drawUnselected() {
  setFrameStyle(QFrame::Panel | QFrame::Raised);
}

void ImageItem::unselect() {
  drawUnselected();
}

ImageListWidget::ImageListWidget(QWidget *parent) :
  QWidget(parent) {
  defaultSettings = {QRectF(13.5, 15, 270,180),
                     QPagedPaintDevice::A4,
                     QSizeF(297,210),
                     QPageLayout::Landscape};
  layout = new QVBoxLayout;
  setLayout(layout);
  selected = 0;
  openAction = new QAction(trUtf8("&Open"), this);
  moveUpAction = new QAction(trUtf8("Move &up"), this);
  moveDownAction = new QAction(trUtf8("Move &down"), this);
  deleteAction = new QAction(trUtf8("D&elete"), this);
  writePdfAction = new QAction(trUtf8("&Write PDF"), this);
  pageSetupAction = new QAction(trUtf8("&Page Settings"), this);

  connect(openAction, SIGNAL(triggered()),
          this, SLOT(startOpenDialog()));
  connect(moveUpAction, SIGNAL(triggered()),
          this, SLOT(moveSelectedUp()));
  connect(moveDownAction, SIGNAL(triggered()),
          this, SLOT(moveSelectedDown()));
  connect(deleteAction, SIGNAL(triggered()),
          this, SLOT(deleteSelected()));
  connect(writePdfAction, SIGNAL(triggered()),
          this, SLOT(writePdf()));
  connect(pageSetupAction, SIGNAL(triggered()),
          this, SLOT(pageSetup()));
  setMinimumWidth(Image::ThumbnailWidth+6);
}

bool ImageListWidget::addItem(const QString & filename) {
  ImageItem *item = new ImageItem(filename, 0, defaultSettings);
  if(!item->image()->isOk()) {
    delete item;
    return false;
  }

  items.append(item);
  layout->addWidget(item);

  QObject::connect(item, SIGNAL(requestSelection(ImageItem*)), this, SLOT(selectImage(ImageItem*)));
  return true;
}

void ImageListWidget::moveSelectedUp(void) {
  if(!selected) return;
  int i = items.indexOf(selected);
  Q_ASSERT(i >= 0);
  if(i == 0) return; //Výše už to nepůjde
  QLayoutItem *taken = layout->takeAt(i-1);
  layout->insertItem(i, taken);
  ImageItem *taken2 = items.at(i-1);
  items.removeAt(i-1);
  items.insert(i, taken2);
}

void ImageListWidget::deleteSelected(void) {
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

void ImageListWidget::moveSelectedDown(void) {
  if(!selected) return;
  int i = items.indexOf(selected);
  Q_ASSERT(i >= 0);
  if(i == items.count() - 1) return; //Níže už to nepůjde
  QLayoutItem *taken = layout->takeAt(i+1);
  layout->insertItem(i, taken);
  ImageItem *taken2 = items.at(i+1);
  items.removeAt(i+1);
  items.insert(i, taken2);
}

bool ImageListWidget::addItems(const QStringList &filenames) {
  bool ok = true;
  for(QList<QString>::ConstIterator i = filenames.constBegin(); i != filenames.constEnd(); i++)
    if(!addItem(*i)) ok = false;
  return ok;
}

void ImageListWidget::startOpenDialog() {
  QStringList filesToOpen = QFileDialog::getOpenFileNames(this, trUtf8("Open images"));
  if (filesToOpen.isEmpty()) return;
  addItems(filesToOpen);
}

void ImageListWidget::selectImage(ImageItem *item) {
  // TODO ověřit, že je obsažen
  if(selected == item) return;
  if(selected) selected->unselect();
  selected = item;
  emit selectedImage(selected->image());
}

void ImageListWidget::pageSetup(void) {
  Image::PageSettings opt = defaultSettings;
  PageDialog dialog(opt, this);
  // FIXME / TODO rozlišovat rozsah aplikace (nové/současná/vše)
  // Momentálně vše.
  if (QDialog::Accepted == dialog.exec()) {
      defaultSettings = opt;
      // Aplikace na vše
      for (QList<ImageItem *>::Iterator i = items.begin(); i != items.end(); i++) {
          (*i)->image()->setPageSettings(opt);
        }
    }
}

bool ImageListWidget::writePdf(QString &target) {
  QPdfWriter w(target);
  /* První stránku nutno nastavit ještě před konstrukcí QPainter */
  w.setPageSize(items[0]->image()->pageSettings().pageSize);
  w.setPageSizeMM(items[0]->image()->pageSettings().pageSizeMM);
  w.setCreator(trUtf8("Grautafel"));
  QPainter painter(&w);
  for (QList<ImageItem *>::ConstIterator i = items.constBegin(); i != items.constEnd(); i++) {
      Image *gtimg = (*i)->image();

      w.setPageSize(gtimg->pageSettings().pageSize);
      w.setPageSizeMM(gtimg->pageSettings().pageSizeMM);
      if (i != items.constBegin()) if(!w.newPage()) {
          qCritical() << trUtf8("Failed creating new page.");
          return false;
        }

      QImage transformed = gtimg->targetImage();
      if(transformed.isNull()) {
          qCritical() << trUtf8("Transformed image is null.");
          return false;
        }

      painter.drawImage(gtimg->targetRect(), transformed);
    }
  return true;
}

void ImageListWidget::writePdf(void) {
  QString filename = QFileDialog::getSaveFileName(this, trUtf8("Save PDF"), QString(), "PDF documents (*.pdf)");
  if(!writePdf(filename))
    qCritical() << trUtf8("Failed to write PDF."); //FIXME překlady
}
