#include "gtmainwidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QCheckBox>

GTMainWidget::GTMainWidget(QWidget *parent) :
  QWidget(parent)
{
  QPushButton *upButton = new QPushButton(trUtf8("Move up"));
  QPushButton *downButton = new QPushButton(trUtf8("Move down"));
  QCheckBox *toggleTransformBox = new QCheckBox(trUtf8("Preview"));

  QVBoxLayout *bwl = new QVBoxLayout;
  bwl->addWidget(upButton);
  bwl->addWidget(downButton);
  bwl->addWidget(toggleTransformBox);
  QWidget *buttonWidget = new QWidget;
  buttonWidget->setLayout(bwl);

  listWidget = new GTImageListWidget;
  view = new GTImageView;
  QObject::connect(listWidget, SIGNAL(selectedImage(GTImage*)),
                   view, SLOT(setImage(GTImage*)));
  // TODO naconnectit tlačítka

  QHBoxLayout *layout = new QHBoxLayout;
  layout->addWidget(view);
  layout->addWidget(buttonWidget);
  layout->addWidget(listWidget);
  setLayout(layout);

}
