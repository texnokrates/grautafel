#ifndef GTIMAGE_H
#define GTIMAGE_H

#include <QObject>
#include <QImage>
#include <QTransform>
#include <QPixmap>
#include <QLineF>
#include <QVector>
#include <QPolygon>

class GTImage : public QObject
{
    Q_OBJECT
    QString srcFilename, destFilename;
    QImage src, dest;
    QPixmap thumbnail;
    QTransform transform;
    QLineF borderLeft, borderRight, borderTop, borderBottom;

    bool checkSrcLoad(); // Načte soubor srcFilename do src, je-li src prázdný. OK=>true.
    bool checkSrcLoadARGB(); // Totéž co předchozí s převodem do ARGB32
    void checkSrcUnload(); // Vyprázdní src, pokud to politika nařizuje
public:
    explicit GTImage(QObject *parent = 0);
    GTImage(const QString &fn, QObject *parent = 0);
    void setSrcFilename(const QString &fn) {
        srcFilename = fn;
    }
    int srcWidth();
    int srcHeight();

//    QPolygon findRectangle(int diam, qreal medianThreshold); // Nejdůležitější funkce
    QPolygon findRectangleSpiral(qreal relativeMedianThreshold);


//    static QColor getMedianColor(const QImage &img, const QRect &area);

    QList<QColor> getColorQuantiles (const QRect &area, const QList<qreal> &quantiles);
    
signals:
    void srcLoadFailed(GTImage *); // Nelze načíst obrázek (zpravidla vyšle checkSrcLoad())
    
public slots:
    
};

#endif // GTIMAGE_H
