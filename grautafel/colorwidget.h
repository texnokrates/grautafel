#ifndef GT_COLORWIDGET_H
#define GT_COLORWIDGET_H

#include <QWidget>
class QSlider;
class QCheckBox;

namespace GT {
  class Image;

  class ColorWidget : public QWidget
  {
    Q_OBJECT
  public:
    explicit ColorWidget(QWidget *parent = 0);

  signals:
    void minChanged(int);
    void maxChanged(int);
    void invertChanged(bool);

  public slots:
    void reloadVals(Image *);

    // Kontrola min < max
    void checkMin(int newmax);
    void checkMax(int newmin);

  private:
    QSlider *minSlider_, *maxSlider_;
    QCheckBox *invertBox_;
  };

} // namespace GT

#endif // GT_COLORWIDGET_H
