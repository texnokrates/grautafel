#include "gtmainwidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QCheckBox>
#include <QScrollArea>
#include "gtimage.h"
#include <viewzoomwidget.h>

using namespace GT;

MainWidget::MainWidget(QWidget *parent) :
  QWidget(parent) {

  // Todo číst texty tlačítek z akcí
  view = new ImageView;
  QPushButton *upButton = new QPushButton(trUtf8("Move up"));
  QPushButton *downButton = new QPushButton(trUtf8("Move down"));
  QPushButton *openButton = new QPushButton(trUtf8("Open"));
  QPushButton *deleteButton = new QPushButton(trUtf8("Delete"));
  toggleTransformBox = new QCheckBox(trUtf8("Preview"));
  ZoomWidget *zoomWidget = new ZoomWidget(view);
  QPushButton *pageSetupButton = new QPushButton(trUtf8("Page Setup..."));
  QPushButton *writePdfButton = new QPushButton(trUtf8("Write PDF..."));


  QVBoxLayout *bwl = new QVBoxLayout;
  bwl->addWidget(openButton);
  bwl->addWidget(upButton);
  bwl->addWidget(downButton);
  bwl->addWidget(deleteButton);
  bwl->addWidget(toggleTransformBox);
  bwl->addWidget(zoomWidget);
  bwl->addWidget(pageSetupButton);
  bwl->addWidget(writePdfButton);
  QWidget *buttonWidget = new QWidget;
  buttonWidget->setLayout(bwl);

  listWidget = new ImageListWidget;
  QObject::connect(listWidget, SIGNAL(emptied()),
                   view, SLOT(clear()));
  QObject::connect(listWidget, SIGNAL(selectedImage(Image*)),
                   view, SLOT(setImage(Image*)));
  QObject::connect(openButton, SIGNAL(clicked()),
                   listWidget->openAction, SLOT(trigger()));
  QObject::connect(upButton, SIGNAL(clicked()),
                   listWidget->moveUpAction, SLOT(trigger()));
  QObject::connect(downButton, SIGNAL(clicked()),
                   listWidget->moveDownAction, SLOT(trigger()));
  QObject::connect(deleteButton, SIGNAL(clicked()),
                   listWidget->deleteAction, SLOT(trigger()));
  QObject::connect(toggleTransformBox, SIGNAL(stateChanged(int)),
                   view, SLOT(setPreview(int)));
  QObject::connect(toggleTransformBox, SIGNAL(stateChanged(int)),
                   this, SLOT(ensurePreviewButtonNotTristate(int)));
  QObject::connect(view, SIGNAL(newPreviewState(int)),
                   this, SLOT(setPreviewButton(int)));
  QObject::connect(pageSetupButton, SIGNAL(clicked()),
                   listWidget->pageSetupAction, SLOT(trigger()));
  QObject::connect(writePdfButton, SIGNAL(clicked()),
                   listWidget->writePdfAction, SLOT(trigger()));

  // TODO naconnectit ostatní tlačítka

  QHBoxLayout *layout = new QHBoxLayout;
  layout->addWidget(view);
  layout->addWidget(buttonWidget);

  QScrollArea *lwarea = new QScrollArea;
  lwarea->setWidget(listWidget);
  lwarea->setWidgetResizable(true);
  // FIXME sem skutečnou šířku scrollbaru místo konstanty 30
  lwarea->setFixedWidth(Image::ThumbnailWidth + 30);
  layout->addWidget(lwarea);
  setLayout(layout);
}

void MainWidget::ensurePreviewButtonNotTristate(int state) {
  if ((int)Qt::PartiallyChecked != state)
    toggleTransformBox->setTristate(false);
}

void MainWidget::setPreviewButton(int qtCheckStatus) {
  toggleTransformBox->setCheckState((Qt::CheckState) qtCheckStatus);
  if (qtCheckStatus != (int)Qt::PartiallyChecked)
    toggleTransformBox->setTristate(false);
}
