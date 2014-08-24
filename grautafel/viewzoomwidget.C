#include "viewzoomwidget.h"
#include <QLocale>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>

// Chci to předělat na QToolBar

GTImageViewZoomWidget::GTImageViewZoomWidget(QWidget *parent) :
  QWidget(parent)
{
  locale_ = locale();
  plusButton_ = new QPushButton(QString("^+"));
  minusButton_ = new QPushButton(QString("^-"));
  sizeLabel_ = new QLabel();

  QHBoxLayout * l = new QHBoxLayout;
  l->addWidget(minusButton_);
  l->addWidget(sizeLabel_);
  l->addWidget(plusButton_);
}

void GTImageViewZoomWidget::setImageView(GTImageView *v){
  // TODO
}
