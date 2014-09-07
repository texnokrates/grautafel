#ifndef GTMAINWIDGET_H
#define GTMAINWIDGET_H
#include <gtimagelistwidget.h>
#include <gtimageview.h>
class QCheckBox;
#include <QWidget>

namespace GT {
  class MainWidget : public QWidget {
    Q_OBJECT
    QCheckBox *toggleTransformBox;
  public:
    explicit MainWidget(QWidget *parent = 0);
    ImageListWidget *listWidget;
    ImageView *view;
  signals:

  public slots:
    void ensurePreviewButtonNotTristate(int);
    void setPreviewButton(int); // int je Qt::CheckStatus
  };
}
#endif // GTMAINWIDGET_H
