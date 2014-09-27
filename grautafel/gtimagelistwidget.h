#ifndef GTIMAGELISTWIDGET_H
#define GTIMAGELISTWIDGET_H

#include <QFrame>
class QVBoxLayout;
#include <QList>
class QLabel;
#include <QAction>

namespace GT {
  class Image;

  class ImageItem : public QFrame {
    Q_OBJECT
    Image *img_;
    QLabel *thumbnail_;
    QVBoxLayout *layout_;
//  QLabel *nameLine_;

  public:
    explicit ImageItem(const QString &srcname, QWidget *parent = 0);
    Image *image(void) const {
      return img_;
    }
    void drawSelected(void);
    void drawUnselected(void);


  signals:
    void requestSelection(ImageItem *myself);
    void destroyMe(ImageItem *myself);

  public slots:
    void unselect(void); // (potřebuji to jako slot?)
  protected:
    void focusInEvent(QFocusEvent *);
  };

  class ImageListWidget : public QWidget {
    Q_OBJECT
    QVBoxLayout * layout;
    QList <ImageItem *> items; // Nutné kvůli mazání a přehazování
    ImageItem *selected;
  public:
    bool deleteItem(ImageItem *it);
    bool addItem(const QString & filename);
    bool addItems(const QStringList & filenames);

    QAction *openAction;
    QAction *moveUpAction;
    QAction *moveDownAction;
    QAction *deleteAction;



  public:
    explicit ImageListWidget(QWidget *parent = 0);

  signals:
    void selectedImage(Image *);
    void emptied(void);

  public slots:
    void selectImage(ImageItem *);
    void moveSelectedUp(void);
    void moveSelectedDown(void);
    void startOpenDialog(void);
    void deleteSelected(void);

  };
}
#endif // GTIMAGELISTWIDGET_H
