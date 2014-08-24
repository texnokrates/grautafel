#ifndef VIEWZOOMWIDGET_H
#define VIEWZOOMWIDGET_H

#include <QAbstractSpinBox>
#include <QLocale>

class GTImageViewZoomWidget : public QAbstractSpinBox
{
  Q_OBJECT
public:
  explicit GTImageViewZoomWidget(QWidget *parent = 0);
  // Reimplemented QAbstractSpinBox virtual funs:
  QValidator::State validate (QString & input, int & pos) const;
  void stepBy(int steps);
  void fixup(QString & input) const;

signals:

public slots:
private:
  float zoom_;
  QLocale locale_; // for string conversions
};

#endif // VIEWZOOMWIDGET_H
