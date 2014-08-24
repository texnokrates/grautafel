#include "viewzoomwidget.h"
#include <QLocale>

GTImageViewZoomWidget::GTImageViewZoomWidget(QWidget *parent) :
  QAbstractSpinBox(parent)
{
  setButtonSymbols(QAbstractSpinBox::PlusMinus);
  locale_ = locale();
}

QValidator::State GTImageViewZoomWidget::validate (QString & input, int & pos) const {
  // Very basic functionality
  bool ok;
  locale_.toFloat(input, &ok);
  return ok ? QValidator::Acceptable : QValidator::Invalid;
}

void GTImageViewZoomWidget::stepBy(int steps) {
  //TODO
}

void GTImageViewZoomWidget::fixup(QString & input) const {
  //TODO
}
