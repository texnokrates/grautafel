#ifndef GTIMAGELISTWIDGET_H
#define GTIMAGELISTWIDGET_H

#include <QFrame>
#include <QVBoxLayout>
#include <QList>
#include <gtimage.h>
#include <QLabel>
#include <QAction>

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
  void unselect(void); // (potřebuji to jako slot?)
protected:
  void focusInEvent(QFocusEvent *);
};

class GTImageListWidget : public QWidget
{
  Q_OBJECT
  QVBoxLayout * layout;
  QList <GTImageItem *> items; // Nutné kvůli mazání a přehazování
  GTImageItem *selected;
public:
  bool deleteItem(GTImageItem *it);
  bool addItem(const QString & filename);
  bool addItems(const QStringList & filenames);

  QAction *openAction;
  QAction *moveUpAction;
  QAction *moveDownAction;
  QAction *deleteAction;



public:
  explicit GTImageListWidget(QWidget *parent = 0);

signals:
  void selectedImage(GTImage *);

public slots:
  void selectImage(GTImageItem *);
  void moveSelectedUp(void);
  void moveSelectedDown(void);
  void startOpenDialog(void);

};

#endif // GTIMAGELISTWIDGET_H
