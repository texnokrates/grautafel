#ifndef GTIMAGELISTWIDGET_H
#define GTIMAGELISTWIDGET_H

#include <QFrame>
class QVBoxLayout;
#include <QList>
class QLabel;
#include <QAction>
#include "gtimage.h"

namespace GT {

  class ImageItem : public QFrame {
    Q_OBJECT

  public:
    explicit ImageItem(const QString &srcname, QWidget *parent = 0,
          const Image::PageSettings &settings = Image::PageSettings::defaultSettings(),
                       bool guessColors = true, bool guessShape = false); // FIXME výchozí hodnoty
    Image *image(void) const;
    void drawSelected(void);
    void drawUnselected(void);


  signals:
    void requestSelection(ImageItem *myself);
    void destroyMe(ImageItem *myself);

  public slots:
    void unselect(void); // (potřebuji to jako slot?)
  protected:
    void focusInEvent(QFocusEvent *);

  private:
    Image *img_;
    QLabel *thumbnail_;
    QVBoxLayout *layout_;
  };

  class ImageListWidget : public QWidget {
    Q_OBJECT
  public:
    bool deleteItem(ImageItem *it);
    bool addItem(const QString & filename);
    bool addItems(const QStringList & filenames);

    QAction *openAction;
    QAction *moveUpAction;
    QAction *moveDownAction;
    QAction *deleteAction;
    QAction *writePdfAction;
    QAction *pageSetupAction;

    bool writePdf(QString &target);


  public:
    explicit ImageListWidget(QWidget *parent = 0);

  signals:
    void selectedImage(Image *);
    void emptied(void);

  public slots:
    void pageSetup(void);
    void selectImage(ImageItem *);
    void moveSelectedUp(void);
    void moveSelectedDown(void);
    void startOpenDialog(void);
    void deleteSelected(void);
    void writePdf(void);

  private:
    QVBoxLayout * layout_;
    QList <ImageItem *> items_; // Nutné kvůli mazání a přehazování
    ImageItem *selected_;
    Image::PageSettings defaultSettings_;
  };
}
#endif // GTIMAGELISTWIDGET_H
