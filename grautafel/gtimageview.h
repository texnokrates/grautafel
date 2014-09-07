#ifndef GTIMAGEVIEW_H
#define GTIMAGEVIEW_H

class QGraphicsScene;
#include <QGraphicsView>
#include <QGraphicsObject>
#include <QVector>
#include <QPointF>
class QAction;
class QTransform;

namespace GT {
  class Image;

  class CornerItem : public QGraphicsObject {
    Q_OBJECT // Je to třeba?
    static const int radius_ = 5;
  public:
    explicit CornerItem(QGraphicsItem *parent = 0) : QGraphicsObject(parent) {
      setCursor(Qt::BlankCursor);
    }
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
    QRectF boundingRect(void) const;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *);
  signals:
    void requestBoundingRectUpdate();
  };

  class ImageView : public QGraphicsView {
    Q_OBJECT

  public:
    explicit ImageView(QWidget *parent = 0);
    enum PreviewState {
      NotPreview = (int) Qt::Unchecked,
      NewPreview = (int) Qt::Checked,
      OldPreview = (int) Qt::PartiallyChecked // When the corners are moved
    };

    QVector<QPointF> corners(void) const;
    bool setCorners(const QVector<QPointF> &);
    QRectF cornersBoundingRect(void) const;
    void setZoom(qreal factor);
    qreal zoom(void) const;
    qreal fitToWidthZoom(void) const;
    QPointF center(void) const;
    QTransform quadToTarget(void) const;

    QAction *zoomInAction, *zoomOutAction,
            *zoomFitToWidthAction;
  signals:
    // TODO dodělat signály pro stavový řádek
    void imageChanged(Image *);
    void newPreviewState(int); // oznámí případnou změnu stavu (např. při změně rohů, načtení nového obr, ...) pro tlačítko
    void zoomChanged(qreal);
  public slots:
    void zoomToWidth(void);
    void zoomOut(qreal divisor = 1.25);
    void zoomIn(qreal factor = 1.25);
    void setPreview(int /*PreviewState*/ previewState); // partiallyChecked does nothing
    void setPreview(bool isPreview); // overloaded
    void updateSceneRect(void);
    void setImage(Image *); // uloží změny a načte nový obrázek
    void updateLines(void); // Přepočte polohy hranic
    void clear(void);
#if 0 // Doimplementovat
    void reset(void); // zruší změny na současném obrázku (načte hodnoty znova z něj)

    void setTopLeftCorner(QPointF);
    void setTopRightCorner(QPointF);
    void setBottomLeftCorner(QPointF);
    void setBottomRightCorner(QPointF);
#endif
  private:
    CornerItem *cornerItems_[4];
    QGraphicsLineItem *borderItems_[4];
    Image *img_;
    QGraphicsPixmapItem *pixmapItem_;
    QGraphicsScene *sc_;
    void original_(void);
    void transformed_(void);
    enum PreviewState previewState_;
    void saveAndEmitPreviewStateChange(enum PreviewState nps);
    void saveChanges(void); // uloží změny před přepnutím na další obrázek
  };
}
#endif // GTIMAGEVIEW_H
