#ifndef GTMAINWIDGET_H
#define GTMAINWIDGET_H
#include <gtimagelistwidget.h>
#include <gtimageview.h>

#include <QWidget>

class GTMainWidget : public QWidget
{
  Q_OBJECT
public:
  explicit GTMainWidget(QWidget *parent = 0);
  GTImageListWidget *listWidget;
  GTImageView *view;
signals:

public slots:

};

#endif // GTMAINWIDGET_H
