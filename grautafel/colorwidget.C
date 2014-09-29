#include "colorwidget.h"
#include <QSlider>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QLabel>
#include "gtimage.h"

namespace GT {

  ColorWidget::ColorWidget(QWidget *parent) :
    QWidget(parent)
  {
    invertBox_ = new QCheckBox(trUtf8("Invert colors"));

    minRSlider_ = new QSlider(Qt::Horizontal);
    maxRSlider_ = new QSlider(Qt::Horizontal);

    minRSlider_->setMinimum(0);
    minRSlider_->setMaximum(255);
    minRSlider_->setValue(0);

    maxRSlider_->setMinimum(0);
    maxRSlider_->setMaximum(255);
    maxRSlider_->setValue(255);

    minGSlider_ = new QSlider(Qt::Horizontal);
    maxGSlider_ = new QSlider(Qt::Horizontal);

    minGSlider_->setMinimum(0);
    minGSlider_->setMaximum(255);
    minGSlider_->setValue(0);

    maxGSlider_->setMinimum(0);
    maxGSlider_->setMaximum(255);
    maxGSlider_->setValue(255);

    minBSlider_ = new QSlider(Qt::Horizontal);
    maxBSlider_ = new QSlider(Qt::Horizontal);

    minBSlider_->setMinimum(0);
    minBSlider_->setMaximum(255);
    minBSlider_->setValue(0);

    maxBSlider_->setMinimum(0);
    maxBSlider_->setMaximum(255);
    maxBSlider_->setValue(255);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(new QLabel(trUtf8("Contrast adjustment:")));
    layout->addWidget(new QLabel(trUtf8("Red")));
    layout->addWidget(minRSlider_);
    layout->addWidget(maxRSlider_);
    layout->addWidget(new QLabel(trUtf8("Green")));
    layout->addWidget(minGSlider_);
    layout->addWidget(maxGSlider_);
    layout->addWidget(new QLabel(trUtf8("Blue")));
    layout->addWidget(minBSlider_);
    layout->addWidget(maxBSlider_);
    layout->addWidget(invertBox_);
    setLayout(layout);


    QObject::connect(minRSlider_, SIGNAL(valueChanged(int)),
                     this, SLOT(checkMaxR(int)));
    QObject::connect(maxRSlider_, SIGNAL(valueChanged(int)),
                     this, SLOT(checkMinR(int)));
    QObject::connect(minGSlider_, SIGNAL(valueChanged(int)),
                     this, SLOT(checkMaxG(int)));
    QObject::connect(maxGSlider_, SIGNAL(valueChanged(int)),
                     this, SLOT(checkMinG(int)));
    QObject::connect(minBSlider_, SIGNAL(valueChanged(int)),
                     this, SLOT(checkMaxB(int)));
    QObject::connect(maxBSlider_, SIGNAL(valueChanged(int)),
                     this, SLOT(checkMinB(int)));
    QObject::connect(minRSlider_, SIGNAL(valueChanged(int)),
                     this, SLOT(someMinChanged(void)));
    QObject::connect(maxRSlider_, SIGNAL(valueChanged(int)),
                     this, SLOT(someMaxChanged(void)));
    QObject::connect(minGSlider_, SIGNAL(valueChanged(int)),
                     this, SLOT(someMinChanged(void)));
    QObject::connect(maxGSlider_, SIGNAL(valueChanged(int)),
                     this, SLOT(someMaxChanged(void)));
    QObject::connect(minBSlider_, SIGNAL(valueChanged(int)),
                     this, SLOT(someMinChanged(void)));
    QObject::connect(maxBSlider_, SIGNAL(valueChanged(int)),
                     this, SLOT(someMaxChanged(void)));
    QObject::connect(invertBox_, SIGNAL(toggled(bool)),
                     this, SIGNAL(invertChanged(bool)));
  }

  void ColorWidget::checkMaxR(int newmin) {
    if (newmin > maxRSlider_->value())
      maxRSlider_->setValue(newmin);
  }

  void ColorWidget::checkMinR(int newmax) {
    if (minRSlider_->value() > newmax)
      minRSlider_->setValue(newmax);
  }
  void ColorWidget::checkMaxG(int newmin) {
    if (newmin > maxGSlider_->value())
      maxGSlider_->setValue(newmin);
  }

  void ColorWidget::checkMinG(int newmax) {
    if (minGSlider_->value() > newmax)
      minGSlider_->setValue(newmax);
  }
  void ColorWidget::checkMaxB(int newmin) {
    if (newmin > maxBSlider_->value())
      maxBSlider_->setValue(newmin);
  }

  void ColorWidget::checkMinB(int newmax) {
    if (minBSlider_->value() > newmax)
      minBSlider_->setValue(newmax);
  }

  void ColorWidget::reloadVals(Image *img) {
    if (img) {
        QRgb min = img->minColor();
        QRgb max = img->maxColor();
        minRSlider_->setValue(qRed(min));
        maxRSlider_->setValue(qRed(max));
        minGSlider_->setValue(qGreen(min));
        maxGSlider_->setValue(qGreen(max));
        minBSlider_->setValue(qBlue(min));
        maxBSlider_->setValue(qBlue(max));
        invertBox_->setChecked(img->colorsInverted());
      }
  }

  void ColorWidget::someMinChanged() {
    QRgb m = qRgb(minRSlider_->value(), minGSlider_->value(), minBSlider_->value());
    emit minChanged(m);
  }

  void ColorWidget::someMaxChanged() {
    QRgb m = qRgb(maxRSlider_->value(), maxGSlider_->value(), maxBSlider_->value());
    emit maxChanged(m);
  }
} // namespace GT
