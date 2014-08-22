#include "gtmainwidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QCheckBox>
#include <QScrollArea>
#include <gtimage.h>

GTMainWidget::GTMainWidget(QWidget *parent) :
  QWidget(parent)
{

  // Todo číst texty tlačítek z akcí
  QPushButton *upButton = new QPushButton(trUtf8("Move up"));
  QPushButton *downButton = new QPushButton(trUtf8("Move down"));
  QPushButton *openButton = new QPushButton(trUtf8("Open"));
  QPushButton *deleteButton = new QPushButton(trUtf8("Delete"));
  QCheckBox *toggleTransformBox = new QCheckBox(trUtf8("Preview"));

  QVBoxLayout *bwl = new QVBoxLayout;
  bwl->addWidget(openButton);
  bwl->addWidget(upButton);
  bwl->addWidget(downButton);
  bwl->addWidget(deleteButton);
  bwl->addWidget(toggleTransformBox);
  QWidget *buttonWidget = new QWidget;
  buttonWidget->setLayout(bwl);

  listWidget = new GTImageListWidget;
  view = new GTImageView;
  QObject::connect(listWidget, SIGNAL(emptied()),
                   view, SLOT(clear()));
  QObject::connect(listWidget, SIGNAL(selectedImage(GTImage*)),
                   view, SLOT(setImage(GTImage*)));
  QObject::connect(openButton, SIGNAL(clicked()),
                   listWidget->openAction, SLOT(trigger()));
  QObject::connect(upButton, SIGNAL(clicked()),
                   listWidget->moveUpAction, SLOT(trigger()));
  QObject::connect(downButton, SIGNAL(clicked()),
                   listWidget->moveDownAction, SLOT(trigger()));
  QObject::connect(deleteButton, SIGNAL(clicked()),
                   listWidget->deleteAction, SLOT(trigger()));
  QObject::connect(toggleTransformBox, SIGNAL(toggled(bool)),
                   view, SLOT(preview(bool)));
  // TODO naconnectit ostatní tlačítka

  QHBoxLayout *layout = new QHBoxLayout;
  layout->addWidget(view);
  layout->addWidget(buttonWidget);

  QScrollArea *lwarea = new QScrollArea;
  lwarea->setWidget(listWidget);
  lwarea->setWidgetResizable(true);
  // FIXME sem skutečnou šířku scrollbaru místo konstanty 30
  lwarea->setFixedWidth(GTImage::ThumbnailWidth + 30);
  layout->addWidget(lwarea);
  setLayout(layout);
}
