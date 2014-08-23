#ifndef GTMAINWIDGET_H
#define GTMAINWIDGET_H
#include <gtimagelistwidget.h>
#include <gtimageview.h>
class QCheckBox;
#include <QWidget>

class GTMainWidget : public QWidget
{
  Q_OBJECT
  QCheckBox *toggleTransformBox;
public:
  explicit GTMainWidget(QWidget *parent = 0);
  GTImageListWidget *listWidget;
  GTImageView *view;
signals:

public slots:
  void ensurePreviewButtonNotTristate(int);
  void setPreviewButton(int); // int je Qt::CheckStatus
};

#endif // GTMAINWIDGET_H
