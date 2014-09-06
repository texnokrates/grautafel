#include "viewzoomwidget.h"
#include <QLocale>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <gtimageview.h>

// Chci to předělat na QToolBar

GTImageViewZoomWidget::GTImageViewZoomWidget(GTImageView *view, QWidget *parent)
  : QToolBar(parent)
{
  Q_ASSERT(0 != view);
  view_ = view;
  locale_ = locale();
//  plusButton_ = new QPushButton(QString("^+"));
//  minusButton_ = new QPushButton(QString("^-"));
  sizeLabel_ = new QLabel();

//  QHBoxLayout * l = new QHBoxLayout;
//  l->addWidget(minusButton_);
  addAction(view->zoomOutAction);
//  l->addWidget(sizeLabel_);
  addWidget(sizeLabel_);
//  l->addWidget(plusButton_);
  addAction(view->zoomInAction);
//  setLayout(l);
  QObject::connect(view_, SIGNAL(zoomChanged(qreal)), this, SLOT(displayZoom(qreal)));
}

/*
void GTImageViewZoomWidget::setImageView(GTImageView *v){
  // TODO
  QObject::connect(v, SIGNAL(zoomChanged(qreal)), this, SLOT(displayZoom(qreal)));
}
*/

void GTImageViewZoomWidget::displayZoom(qreal factor) {
  sizeLabel_->setText(locale_.toString(factor, 'f', 2));
}
