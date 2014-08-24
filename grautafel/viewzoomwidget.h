#ifndef VIEWZOOMWIDGET_H
#define VIEWZOOMWIDGET_H

#include <QAbstractSpinBox>
#include <QLocale>
class QPushButton;
class QLabel;
class GTImageView;

class GTImageViewZoomWidget : public QWidget
{
  Q_OBJECT
public:
  explicit GTImageViewZoomWidget(QWidget *parent = 0);
  void setImageView(GTImageView *v);
signals:

public slots:
private:
  QPushButton *plusButton_;
  QPushButton *minusButton_;
  QLabel *sizeLabel_;
  GTImageView *view_;
  float zoom_;
  QLocale locale_; // for string conversions
};

#endif // VIEWZOOMWIDGET_H
