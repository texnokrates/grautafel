#ifndef PAGEDIALOG_H
#define PAGEDIALOG_H

#include <QDialog>
#include <QMap>
#include <QString>
#include "gtimage.h"
#include <QAbstractTableModel>
#include <QPageSize>
class QComboBox;
class QCheckBox;
class QDoubleSpinBox;
class QLabel;

namespace GT {

  // Auxilliary class for the page format combobox
  class PageFormatModel : public QAbstractTableModel {
    Q_OBJECT

  public:
    PageFormatModel(QObject *parent);
    int rowCount(const QModelIndex &parent = QModelIndex()) const ;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    int defaultRowIndex(void);
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    const QList<QPageSize::PageSizeId> *listedSizes(void) const;

  private:
    QList<QPageSize::PageSizeId> formatList_;
  };


  class PageDialog : public QDialog {
    Q_OBJECT

  public:
    explicit PageDialog(Image::PageSettings &opt,  QWidget *parent = 0);

  private:
    Image::PageSettings &opt;

    PageFormatModel *formatModel_;

    QComboBox *paperFormatBox;
    QCheckBox *landscapeCheckBox;
    QPushButton *verticalCenterButton, *horisontalCenterButton;
    QDoubleSpinBox *paperHeightBox, *paperWidthBox;
    QDoubleSpinBox *topMarginBox, *leftMarginBox;
    QDoubleSpinBox *rectHeightBox, *rectWidthBox;
    QLabel *widthHeightRatioLabel, *rightMarginLabel,
           *bottomMarginLabel;

  signals:

  public slots:
    void setPageSizeCustom(void);
    void updateLabelVals(void);
    void updatePaperWidth(double);
    void updatePaperHeight(double);
    void updateRectHeight(double);
    void updateRectWidth(double);
    void updateRectTop(double);
    void updateRectLeft(double);
    void updateFormat(int);
    void horisontalCenter(void);
    void verticalCenter(void);
    void updateOrientation(bool isLandscape);
  };
}
#endif // PAGEDIALOG_H
