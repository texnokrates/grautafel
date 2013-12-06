#ifndef GTIMAGE_H
#define GTIMAGE_H

#include <QObject>
#include <QImage>
#include <QTransform>
#include <QPixmap>
#include <QLineF>
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
    QString srcFilename, //!< Path to the source photograph.
        destFilename; //!< Path to the transformed photograph.
    QImage src, dest;
    QPixmap thumbnail; //!< Thumbnail to be displayed on the thumbnail area.
    QTransform transform;
    QLineF borderLeft, borderRight, borderTop, borderBottom;

    bool checkSrcLoad(); //!< Reads the source image from srcFilename path if src is empty. OK=>true.
    bool checkSrcLoadARGB(); //!< Reads the source image from srcFilename path if src is empty, converting it to ARGB32 format
    void checkSrcUnload(); //!< Empties the src object if enabled by the memory policy.
public:
    explicit GTImage(QObject *parent = 0);
    GTImage(const QString &fn, QObject *parent = 0);
    void setSrcFilename(const QString &fn) {
        srcFilename = fn;
    }
    int srcWidth();
    int srcHeight();

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
    
public slots:
    
};

#endif // GTIMAGE_H
