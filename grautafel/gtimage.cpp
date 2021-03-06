#include "gtimage.h"
#include <QtDebug>

GTImage::GTImage(QObject *parent) :
    QObject(parent)
{
}

GTImage::GTImage(const QString &fn, QObject *parent) :
    GTImage(parent)
{
    setSrcFilename(fn);
}

int GTImage::srcWidth() {
    checkSrcLoad();
    return src.width();
}

int GTImage::srcHeight() {
    checkSrcLoad();
    return src.height();
}

#if 0
QColor GTImage::getMedianColor(const QImage &img, const QRect &area) {
    // "QImage::pixel() is expensive when used for massive pixel manipulations."
    int len = area.width()*area.height();
    QVector<int> r(len), g(len), b(len);
    for(int i = 0; i < area.height(); i++) {
        for(int j = 0; j < area.width(); j++){
            QRgb px = img.pixel(area.bottomLeft() + QPoint(j,i));
            r[i*area.width()+j] = qRed(px);
            g[i*area.width()+j] = qGreen(px);
            b[i*area.width()+j] = qBlue(px);
        }
    }
    qSort(r);
    qSort(g);
    qSort(b);
    return QColor(r[len/2], g[len/2], b[len/2]);
}
#endif

QList<QColor> GTImage::getColorQuantiles (const QRect &area, const QList<qreal> &quantiles){
    checkSrcLoad();
    // "QImage::pixel() is expensive when used for massive pixel manipulations."
    int len = area.width()*area.height();
    QVector<int> r(len), g(len), b(len);
    for(int i = 0; i < area.height(); i++) {
        for(int j = 0; j < area.width(); j++){
            QRgb px = src.pixel(area.bottomLeft() + QPoint(j,i));
            r[i*area.width()+j] = qRed(px);
            g[i*area.width()+j] = qGreen(px);
            b[i*area.width()+j] = qBlue(px);
        }
    }
    qSort(r);
    qSort(g);
    qSort(b);
    QList<QColor> qcols;

    for (QList<qreal>::const_iterator i = quantiles.begin(); i != quantiles.end(); ++i)
        qcols << QColor(r[(int)(len * (*i))],g[(int)(len * (*i))],b[(int)(len * (*i))]);
    return qcols;
}

#if 0
#include <QPoint>
QPolygon GTImage::findRectangle(int diam, qreal medianThreshold) {
    if(!checkSrcLoadARGB()) return QPolygon();

    /* Takže plán: začneme od prostředního bodu hledáním do šířky a pak... se uvidí*/


}
#endif

QPolygon GTImage::findRectangleSpiral(qreal relativeMedianThreshold){

}

bool GTImage::checkSrcLoadARGB(){
    if(!checkSrcLoad()) return false;
    if(src.format() != QImage::Format_ARGB32)
        src = src.convertToFormat(QImage::Format_ARGB32);
    return true;
}

bool GTImage::checkSrcLoad(){
    if(src.format() == QImage::Format_Invalid)
        src = QImage(srcFilename);
    // TODO testy na typ chyby, vypuštění chybové hlášky (do logu a na stavový řádek)...
    if(src.isNull()){
        emit srcLoadFailed(this);
        qWarning() << tr("Could not load image:") << srcFilename;
        return false;
    }
    else return true;
}

void GTImage::checkSrcUnload(){
    if(!src.isNull() && true /* TODO zde podmínka dle politiky */ )
        src = QImage();
}
