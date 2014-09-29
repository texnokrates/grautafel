#ifndef GTMAINWIDGET_H
#define GTMAINWIDGET_H
#include "gtimagelistwidget.h"
#include "gtimageview.h"
class QCheckBox;
#include <QWidget>

namespace GT {
  class MainWidget : public QWidget {
    Q_OBJECT

  public:
    explicit MainWidget(QWidget *parent = 0);
    ImageListWidget *listWidget;
    ImageView *view;

  signals:

  public slots:
    void ensurePreviewButtonNotTristate(int); // Ošklivý hack pro správné chování checkboxu
    void setPreviewButton(int); // int je Qt::CheckStatus

  private:
    QCheckBox *toggleTransformBox_;
  };
}
#endif // GTMAINWIDGET_H
