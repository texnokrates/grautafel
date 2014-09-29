#ifndef GTIMAGE_H
#define GTIMAGE_H

#include <QObject>
#include <QImage>
#include <QTransform>
#include <QPixmap>
#include <QLineF>
#include <QPointF>
#include <QVector>
#include <QPolygon>
#include <QPagedPaintDevice>
#include <QPageLayout>
namespace GT {
//! A class holding individual source image (if needed), info and transformation data.
  /*!
   * This class also holds the modified image if being currently edited
   * (or if the /unimplemented/ memory policy enables it; otherwise,
   * it is saved in the destFilename path).
   */
  class Image : public QObject {
    Q_OBJECT
  public:
    static const int ThumbnailWidth = 128;
    static const int ThumbnailHeight = 96;
    enum CornersStatus {
      NotSet = 0x0,
      SetToSourceCornersAtLoad = 0x1,
      SetManually = 0x2,
      GuessedByHoughTransform = 0x4
    };
//    enum ThumbnailType {
//      Fast = 0x1;
//      Full = 0x2;
//    };
    struct PageSettings {
      QRectF targetRect;
      QPagedPaintDevice::PageSize pageSize;
      QSizeF pageSizeMM; // Skutečné rozměry, závislé na orientation
      QPageLayout::Orientation orientation;
      static PageSettings defaultSettings(void);

    };
  private:
    bool isOk_;
    CornersStatus cstat_;
    QString srcFilename_, //!< Path to the source photograph.
            destFilename_; //!< Path to the transformed photograph.
    QImage src_, dest_;
    QPixmap thumbnail_; //!< Thumbnail to be displayed on the thumbnail area.
//  QTransform transform_; // Transforms the original image to the target rectangle
    QVector <QPointF> corners_; // Corners are authoritative, not borders or transform
    QSize size_;
    QRectF targetRect_;
    qreal lastZoom_;
    QPointF lastViewPoint_;
    struct PageSettings settings_;
    bool invertColors_;
    QRgb minColor_, maxColor_;
//    int minR_, maxR_, minG_, maxG_, minB_, maxB_; //!< minValue_ gets transformed to 0, maxValue_ to 255, linearly in between.
    //void makeThumbnail();
    bool checkSrcLoad(); //!< Reads the source image from srcFilename path if src is empty. OK=>true.
    bool checkSrcLoadARGB(); //!< Reads the source image from srcFilename path if src is empty, converting it to ARGB32 format
    void checkSrcUnload(); //!< Empties the src object if enabled by the memory policy.
  public:
    void setPageSettings(PageSettings &);
    const PageSettings &pageSettings(void);
    void setLastViewPoint(const QPointF &where);
    void setLastZoom(qreal zoom);
    QPointF lastViewPoint(void) const;
    qreal lastZoom(void) const;
    bool colorsInverted(void) const;
    void setColorsInverted(bool);

    explicit Image(QObject *parent = 0);
    Image(const QString &fn, QObject *parent = 0, const PageSettings &settings = PageSettings::defaultSettings());
    void setSrcFilename(const QString &fn) {
      srcFilename_ = fn;
    }
    QSizeF targetSize(void) const;
    QRectF targetRect(void) const;
    void setTargetSize(const QSizeF &);
    QPixmap thumbnail(void);
    QVector<QPointF> corners(void);
    bool setCorners(const QVector<QPointF> &corners);
    QImage srcImage(void);
    QImage targetImage(void);
    int srcWidth();
    int srcHeight();
    bool isOk(void) const;
    QVector<QLineF> borders(void) const { // FIXME A co když to ještě není načteno?
      QVector<QLineF> bs(4);
      for (int i = 0; i < 4; i++)
        bs[i] = QLineF(corners_[i],corners_[(i+1)%4]);
      return bs;
    }
    QTransform transform(void);
    QTransform transform(qreal unitScaling);
    QPointF transformDelta(void);
    QPointF transformDelta(qreal unitScaling);
    QRgb minColor(void) const;
    QRgb maxColor(void) const;
    void setMinColor(QRgb);
    void setMaxColor(QRgb);
    static QImage trimColors(QImage img, QRgb minC, QRgb maxC, bool invertColors = false);
    QImage trimColors(QRgb minC, QRgb maxC, bool invertColors_); // Používá hodnoty zvnějšku (kvůli náhledům)
    QImage trimColors(void); // Používá uložené hodnoty

//    QPolygon findRectangle(int diam, qreal medianThreshold); // Nejdůležitější funkce
//    //! Locates the board using the spiral algorithm.
//    QPolygon findRectangleSpiral(qreal relativeMedianThreshold);


//    static QColor getMedianColor(const QImage &img, const QRect &area);

    //! Fetches given color quantiles from a given area of the source image.
    /*!
     * \param area A given area.
     * \param probs Numerical levels at which quantiles are wanted.
     */
    QList<QColor> getColorQuantiles (const QRect &area, const QList<qreal> &probs);
    static QVector<int> getLightnessHistogram(const QImage &img, const QRect &area);
    QVector<int> getLightnessHistogram(const QRect &area);
    static QList<int> getLightnessQuantiles(const QImage &img, const QList<qreal> &probs, const QRect &area);
    QList<int> getLightnessQuantiles(const QList<qreal> &probs, const QRect &area);

  signals:
    void srcLoadFailed(Image *); // Nelze načíst obrázek (zpravidla vyšle checkSrcLoad())
    void changed(Image *);
  public slots:

  };
}
#endif // GTIMAGE_H
