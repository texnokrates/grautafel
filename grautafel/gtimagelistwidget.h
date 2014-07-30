#ifndef GTIMAGELISTWIDGET_H
#define GTIMAGELISTWIDGET_H

#include <QFrame>
#include <QVBoxLayout>
#include <QList>
#include <gtimage.h>
#include <QLabel>

class GTImageItem : public QFrame {
  Q_OBJECT
  GTImage *img_;
  QLabel *thumbnail_;
  QVBoxLayout *layout_;
//  QLabel *nameLine_;

public:
  explicit GTImageItem(const QString &srcname, QWidget *parent = 0);
  GTImage *image(void) const {return img_;}
  void drawSelected(void);
  void drawUnselected(void);


signals:
  void requestSelection(GTImageItem *myself);
  void destroyMe(GTImageItem *myself);

public slots:
protected:
  void focusInEvent(QFocusEvent *);
};

class GTImageListWidget : public QWidget
{
  Q_OBJECT
  QVBoxLayout * layout;
  QList <GTImageItem *> items;
public:
  bool deleteItem(GTImageItem *it);



public:
  explicit GTImageListWidget(QWidget *parent = 0);

signals:
  void imageSelected(GTImage *);

public slots:

};

#endif // GTIMAGELISTWIDGET_H
