#ifndef GTIMAGE_H
#define GTIMAGE_H

#include <QObject>
#include <QImage>
#include <QTransform>
#include <QPixmap>
#include <QLineF>

class GTImage : public QObject
{
    Q_OBJECT
    QString srcFilename, destFilename;
    QImage src, dest;
    QPixmap thumbnail;
    QTransform transform;
    QLineF borderLeft, borderRight, borderTop, borderBottom;
public:
    explicit GTImage(QObject *parent = 0);
    static QColor getMedianColor(const QImage &img, const QRect &area);
    
signals:
    
public slots:
    
};

#endif // GTIMAGE_H
