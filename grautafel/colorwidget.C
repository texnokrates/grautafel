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
    minSlider_ = new QSlider(Qt::Horizontal);
    maxSlider_ = new QSlider(Qt::Horizontal);
    invertBox_ = new QCheckBox(trUtf8("Invert colors"));

    minSlider_->setMinimum(0);
    minSlider_->setMaximum(255);
    minSlider_->setValue(0);

    maxSlider_->setMinimum(0);
    maxSlider_->setMaximum(255);
    maxSlider_->setValue(255);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(new QLabel(trUtf8("Contrast adjustment:")));
    layout->addWidget(minSlider_);
    layout->addWidget(maxSlider_);
    layout->addWidget(invertBox_);
    setLayout(layout);

    QObject::connect(minSlider_, SIGNAL(valueChanged(int)),
                     this, SLOT(checkMax(int)));
    QObject::connect(maxSlider_, SIGNAL(valueChanged(int)),
                     this, SLOT(checkMin(int)));
    QObject::connect(minSlider_, SIGNAL(valueChanged(int)),
                     this, SIGNAL(minChanged(int)));
    QObject::connect(maxSlider_, SIGNAL(valueChanged(int)),
                     this, SIGNAL(maxChanged(int)));
    QObject::connect(invertBox_, SIGNAL(toggled(bool)),
                     this, SIGNAL(invertChanged(bool)));
  }

  void ColorWidget::checkMax(int newmin) {
    if (newmin > maxSlider_->value())
      maxSlider_->setValue(newmin);
  }

  void ColorWidget::checkMin(int newmax) {
    if (minSlider_->value() > newmax)
      minSlider_->setValue(newmax);
  }

  void ColorWidget::reloadVals(Image *img) {
    if (img) {
        minSlider_->setValue(img->minLightness());
        maxSlider_->setValue(img->maxLightness());
        invertBox_->setChecked(img->colorsInverted());
      }
  }
} // namespace GT

