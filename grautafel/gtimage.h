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

//! A class holding individual source image (if needed), info and transformation data.
/*!
 * This class also holds the modified image if being currently edited
 * (or if the /unimplemented/ memory policy enables it; otherwise,
 * it is saved in the destFilename path).
 */
class GTImage : public QObject
{
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

private:
    bool isOk_;
    CornersStatus cstat_;
    QString srcFilename_, //!< Path to the source photograph.
        destFilename_; //!< Path to the transformed photograph.
    QImage src_, dest_;
    QPixmap thumbnail_; //!< Thumbnail to be displayed on the thumbnail area.
    QTransform transform_; // Transforms the original image to the target rectangle
    QVector <QPointF> corners_; // Corners are authoritative, not borders or transform

    QSize size_;
    //void makeThumbnail();
    bool checkSrcLoad(); //!< Reads the source image from srcFilename path if src is empty. OK=>true.
    bool checkSrcLoadARGB(); //!< Reads the source image from srcFilename path if src is empty, converting it to ARGB32 format
    void checkSrcUnload(); //!< Empties the src object if enabled by the memory policy.
public:

    explicit GTImage(QObject *parent = 0);
    GTImage(const QString &fn, QObject *parent = 0);
    void setSrcFilename(const QString &fn) {
        srcFilename_ = fn;
    }
    QPixmap thumbnail(void);
    QVector<QPointF> corners(void);
    bool setCorners(const QVector<QPointF> &corners);
    QImage srcImage(void);
    int srcWidth();
    int srcHeight();
    bool isOk(void) const;
    QVector<QLineF> borders(void) const { // FIXME A co když to ještě není načteno?
      QVector<QLineF> bs(4);
      for (int i = 0; i < 4; i++)
        bs[i] = QLineF(corners_[i],corners_[(i+1)%4]);
      return bs;
    }

//    QPolygon findRectangle(int diam, qreal medianThreshold); // Nejdůležitější funkce
    //! Locates the board using the spiral algorithm.
    QPolygon findRectangleSpiral(qreal relativeMedianThreshold);


//    static QColor getMedianColor(const QImage &img, const QRect &area);

    //! Fetches given color quantiles from a given area of the source image.
    /*!
     * \param area A given area.
     * \param probs Numerical levels at which quantiles are wanted.
     */
    QList<QColor> getColorQuantiles (const QRect &area, const QList<qreal> &probs);
    
signals:
    void srcLoadFailed(GTImage *); // Nelze načíst obrázek (zpravidla vyšle checkSrcLoad())
    void changed(GTImage *);
public slots:
    
};

#endif // GTIMAGE_H
