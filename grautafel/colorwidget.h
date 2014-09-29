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
    void minChanged(QRgb);
    void maxChanged(QRgb);
    void invertChanged(bool);

  public slots:
    void reloadVals(Image *);
    void someMinChanged(void);
    void someMaxChanged(void);

    // Kontrola min < max
    void checkMinR(int newmax);
    void checkMaxR(int newmin);
    void checkMinG(int newmax);
    void checkMaxG(int newmin);
    void checkMinB(int newmax);
    void checkMaxB(int newmax);

  private:
    QSlider *minRSlider_, *maxRSlider_, *minGSlider_, *maxGSlider_, *minBSlider_, *maxBSlider_;
    QCheckBox *invertBox_;
  };

} // namespace GT

#endif // GT_COLORWIDGET_H
