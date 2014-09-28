#include "pagedialog.h"
#include <QComboBox>
#include <QCheckBox>
#include <QPushButton>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QHash>
#include <QGridLayout>
#include "gtimage.h"
#include <QDialogButtonBox>

using namespace GT;

// Pomocná třída pro combobox

PageFormatModel::PageFormatModel(QObject *parent)
  :QAbstractTableModel(parent) {
  formatList_.append(QPageSize::Custom);
  formatList_.append(QPageSize::A2);
  formatList_.append(QPageSize::A3);
  formatList_.append(QPageSize::A4);
  formatList_.append(QPageSize::A5);
  formatList_.append(QPageSize::A6);
}

int PageFormatModel::defaultRowIndex() {
  return formatList_.indexOf(QPageSize::A4);
}

int PageFormatModel::rowCount(const QModelIndex & /*parent*/) const {
  return formatList_.count();
}

int PageFormatModel::columnCount(const QModelIndex & /*parent*/) const {
  return 4;
}

const QList<QPageSize::PageSizeId> *PageFormatModel::listedSizes(void) const {
  return &formatList_;
}

QVariant PageFormatModel::data(const QModelIndex &index, int role) const {
  if (role == Qt::DisplayRole) {
    QPageSize::PageSizeId theId = formatList_[index.row()];
    switch(index.column()) {
    case 0:
      return theId;
    case 1:
      return QPageSize(theId).name();
    case 2:
      return QPageSize(theId).size(QPageSize::Millimeter).width();
    case 3:
      return QPageSize(theId).size(QPageSize::Millimeter).height();
    }
  }
  return QVariant();
}

PageDialog::PageDialog(Image::PageSettings &o, QWidget *parent) :
  QDialog(parent), opt(o) {
  setWindowTitle(trUtf8("Geometry settings"));
  paperFormatBox = new QComboBox();
  paperFormatBox->setEditable(false);
  formatModel_ = new PageFormatModel(this);
  paperFormatBox->setModel(formatModel_);
  paperFormatBox->setModelColumn(1);

  QPageSize ps(static_cast<QPageSize::PageSizeId>(opt.pageSize));
  if (formatModel_->listedSizes()->contains(ps.id())) {
    int i = formatModel_->listedSizes()->indexOf(ps.id());
    paperFormatBox->setCurrentIndex(i);
    // TODO zde „pro jistotu“ nastavit v opt hodnoty z PageSize?
  } else {
    paperFormatBox->setCurrentIndex(formatModel_->listedSizes()->indexOf(QPageSize::Custom));
  }


  landscapeCheckBox = new QCheckBox(trUtf8("Landscape"));
  landscapeCheckBox->setChecked(opt.orientation == QPageLayout::Landscape);

  paperHeightBox = new QDoubleSpinBox;
  paperHeightBox->setMinimum(1);
  paperHeightBox->setMaximum(9999.99);
  paperHeightBox->setValue(opt.pageSizeMM.height());

  paperWidthBox = new QDoubleSpinBox;
  paperWidthBox->setMinimum(1);
  paperWidthBox->setMaximum(9999.99);
  paperWidthBox->setValue(opt.pageSizeMM.width());

  rectHeightBox = new QDoubleSpinBox;
  rectHeightBox->setMinimum(1);
  rectHeightBox->setMaximum(9999.99);
  rectHeightBox->setValue(opt.targetRect.height());

  rectWidthBox = new QDoubleSpinBox;
  rectWidthBox->setMinimum(1);
  rectWidthBox->setMaximum(9999.99);
  rectWidthBox->setValue(opt.targetRect.width());

  widthHeightRatioLabel = new QLabel;

  topMarginBox = new QDoubleSpinBox;
  topMarginBox->setMinimum(-9999.99);
  topMarginBox->setMaximum(9999.99);
  topMarginBox->setValue(opt.targetRect.y());

  leftMarginBox = new QDoubleSpinBox;
  leftMarginBox->setMinimum(-9999.99);
  leftMarginBox->setMaximum(9999.99);
  leftMarginBox->setValue(opt.targetRect.x());

  rightMarginLabel = new QLabel;
  bottomMarginLabel = new QLabel;

  verticalCenterButton = new QPushButton(trUtf8("Center"));
  horisontalCenterButton = new QPushButton(trUtf8("Center"));

  updateLabelVals();

  QGridLayout *layout = new QGridLayout;
  layout->addWidget(new QLabel(trUtf8("Paper format")), 0, 0);
  layout->addWidget(paperFormatBox, 0, 1);
  layout->addWidget(landscapeCheckBox, 0, 3);

  layout->addWidget(new QLabel(trUtf8("Paper size [mm]")), 1, 0);
  layout->addWidget(paperWidthBox, 1, 1);
  layout->addWidget(new QLabel("×"), 1, 2, Qt::AlignCenter);
  layout->addWidget(paperHeightBox, 1, 3);

  layout->addWidget(new QLabel(trUtf8("Target rectangle [mm]")), 2, 0);
  layout->addWidget(rectWidthBox, 2, 1);
  layout->addWidget(new QLabel("×"), 2, 2, Qt::AlignCenter);
  layout->addWidget(rectHeightBox, 2, 3);

  layout->addWidget(new QLabel(trUtf8("Board aspect ratio")), 3, 0);
  layout->addWidget(widthHeightRatioLabel, 3, 1);

  layout->addWidget(new QLabel(trUtf8("Left Margin")), 4, 0);
  layout->addWidget(leftMarginBox, 4, 1);
  layout->addWidget(horisontalCenterButton, 4, 3);

  layout->addWidget(new QLabel(trUtf8("Right Margin")), 5, 0);
  layout->addWidget(rightMarginLabel, 5, 1);

  layout->addWidget(new QLabel(trUtf8("Top Margin")), 6, 0);
  layout->addWidget(topMarginBox, 6, 1);
  layout->addWidget(verticalCenterButton, 6, 3);

  layout->addWidget(new QLabel(trUtf8("Bottom Margin")), 7, 0);
  layout->addWidget(bottomMarginLabel, 7, 1);

  QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
      | QDialogButtonBox::Cancel);
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
  layout->addWidget(buttonBox, 8, 0, 1, -1);

  setLayout(layout);

  connect(paperFormatBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateFormat(int)));
  connect(landscapeCheckBox, SIGNAL(toggled(bool)), this, SLOT(updateOrientation(bool)));
  connect(verticalCenterButton, SIGNAL(clicked()), this, SLOT(verticalCenter()));
  connect(horisontalCenterButton, SIGNAL(clicked()), this, SLOT(horisontalCenter()));
  connect(paperHeightBox, SIGNAL(valueChanged(double)), this, SLOT(updatePaperHeight(double)));
  connect(paperHeightBox, SIGNAL(valueChanged(double)), this, SLOT(setPageSizeCustom()));
  connect(paperWidthBox, SIGNAL(valueChanged(double)), this, SLOT(updatePaperWidth(double)));
  connect(paperWidthBox, SIGNAL(valueChanged(double)), this, SLOT(setPageSizeCustom()));
  connect(topMarginBox, SIGNAL(valueChanged(double)), this, SLOT(updateRectTop(double)));
  connect(leftMarginBox, SIGNAL(valueChanged(double)), this, SLOT(updateRectLeft(double)));
  connect(rectHeightBox, SIGNAL(valueChanged(double)), this, SLOT(updateRectHeight(double)));
  connect(rectWidthBox, SIGNAL(valueChanged(double)), this, SLOT(updateRectWidth(double)));
}

/*!
 * \brief Spočte poměr pravý a dolní okraje.
 */
void PageDialog::updateLabelVals() {
  widthHeightRatioLabel->setText(locale().toString(opt.targetRect.width()/opt.targetRect.height()));
  rightMarginLabel->setText(locale().toString(opt.pageSizeMM.width() - opt.targetRect.right()));
  bottomMarginLabel->setText(locale().toString(opt.pageSizeMM.height() - opt.targetRect.bottom()));
}

void PageDialog::updatePaperWidth(double w) {
  opt.pageSizeMM.setWidth(w);
  updateLabelVals();
}

void PageDialog::updatePaperHeight(double h) {
  opt.pageSizeMM.setHeight(h);
  updateLabelVals();
}

void PageDialog::updateRectWidth(double w) {
  opt.targetRect.setWidth(w);
  updateLabelVals();
}

void PageDialog::updateRectHeight(double h) {
  opt.targetRect.setHeight(h);
  updateLabelVals();
}

void PageDialog::updateRectTop(double y) {
  opt.targetRect.moveTop(y);
  updateLabelVals();
}

void PageDialog::updateRectLeft(double x) {
  opt.targetRect.moveLeft(x);
  updateLabelVals();
}

#include <QDebug>
void PageDialog::verticalCenter() {
  qDebug() << "targetRect == " << opt.targetRect << endl << opt.pageSizeMM;
  topMarginBox->setValue((opt.pageSizeMM.height() - opt.targetRect.height())/2);
}

void PageDialog::horisontalCenter() {
  leftMarginBox->setValue((opt.pageSizeMM.width() - opt.targetRect.width())/2);
}

/*!
 * \brief Upraví hodnoty rozměrů papíru při výběru jiného formátu
 */
void PageDialog::updateFormat(int newindex) {
  if (formatModel_->listedSizes()->value(newindex) == QPageSize::Custom) {
    opt.pageSize = static_cast<QPagedPaintDevice::PageSize>(QPageSize::Custom);
    return;
  }

  opt.pageSize = static_cast<QPagedPaintDevice::PageSize>(formatModel_->data(formatModel_->index(newindex, 0)).toInt());
  double w = formatModel_->data(formatModel_->index(newindex, 2)).toDouble();
  double h = formatModel_->data(formatModel_->index(newindex, 3)).toDouble();
  if (opt.orientation == QPageLayout::Landscape) {
    double tmp = w;
    w = h;
    h = tmp;
  }
  paperHeightBox->blockSignals(true);
  paperWidthBox->blockSignals(true);
  paperWidthBox->setValue(w);
  paperHeightBox->setValue(h);
  opt.pageSizeMM = QSizeF(w, h);
  paperHeightBox->blockSignals(false);
  paperWidthBox->blockSignals(false);
  updateLabelVals();
}

void PageDialog::updateOrientation(bool isLandscape) {
  int index = paperFormatBox->currentIndex();
  if (formatModel_->listedSizes()->value(index) == QPageSize::Custom)
    return;

  double w = formatModel_->data(formatModel_->index(index, 2)).toDouble();
  double h = formatModel_->data(formatModel_->index(index, 3)).toDouble();
  if(isLandscape) {
    double tmp = w;
    w = h;
    h = tmp;
  }
  paperWidthBox->blockSignals(true);
  paperHeightBox->blockSignals(true);
  paperWidthBox->setValue(w);
  paperHeightBox->setValue(h);
  opt.pageSizeMM = QSizeF(w, h);
  paperHeightBox->blockSignals(false);
  paperWidthBox->blockSignals(false);
  updateLabelVals();
}

void PageDialog::setPageSizeCustom() {
  paperFormatBox->setCurrentIndex(formatModel_->listedSizes()->indexOf(QPageSize::Custom));
}

