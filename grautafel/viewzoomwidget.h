#ifndef VIEWZOOMWIDGET_H
#define VIEWZOOMWIDGET_H

#include <QAbstractSpinBox>
#include <QLocale>
#include <QToolBar>
class QPushButton;
class QLabel;

namespace GT {
  class ImageView;

  class ZoomWidget : public QToolBar {
    Q_OBJECT
  public:
    explicit ZoomWidget(ImageView *view, QWidget *parent = 0);
//  void setImageView(GTImageView *v);
  signals:

  public slots:
    void displayZoom(qreal);
  private:
//  QPushButton *plusButton_;
//  QPushButton *minusButton_;
    QLabel *sizeLabel_;
    ImageView *view_;
    float zoom_;
    QLocale locale_; // for string conversions
  };
}

#endif // VIEWZOOMWIDGET_H
