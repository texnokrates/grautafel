#include "gthoughtransform.h"
#include "gtconvolutionkernel.h"
#include <QImage>
#include <QtGlobal>


GTHoughTransform::GTHoughTransform(QObject *parent) :
    QObject(parent)
{
    data = NULL;
    validity = false;
}

static const double pi = 3.14159265358979323846;

#include <math.h>
static inline int sq(int x) {return x*x;}

void GTHoughTransform::set(int r, int alpha, double val) {
    data[r * angleRes + alpha] = val;
}

double GTHoughTransform::get(int r, int alpha) {
    return data[r * angleRes + alpha];
}

GTHoughTransform::GTHoughTransform(const QImage *src, int angleResolution, QObject *parent) :
    QObject(parent)
{
    originalHeight = src->height();
    originalWidth = src->width();
    radius = (int) sqrt(sq(src->height()) + sq(src->width())) / 2;

    data = new double[radius * angleResolution]();
    // Center coordinates
    int cX = src->width()/2;
    int cY = src->height()/2;

    for (int x = edgeDetectorX->xorig; x < src->width() - (edgeDetectorX->xsiz - edgeDetectorX->xorig); x++)
        for (int y = edgeDetectorX->yorig; y < src->height() - (edgeDetectorX->ysiz - edgeDetectorX->yorig); y++){
            QPoint pos(x,y);
            double val = edgeDetectorX->dConvolveSquared(src, pos);
            val += edgeDetectorY->dConvolveSquared(src, pos);
            val = sqrt(val);
            int r = (int) round(sqrt(sq(x-cX)+sq(y-cY))) % radius; //FIXME je modulo nutné
            int alpha = ((int) round(atan2(y-cY, x-cX) * angleRes / (2 * pi))) % angleRes;
            set(r, alpha, val);
	}
}



//   const uchar **lines = new *uchar[kernYsiz];
//   for(int yu = 0; yu <= src->height() - kernYsiz; y++) {
//     for(int l = 0; l < kernYsiz; l++)


//   }
//   validity = true;


//}

GTHoughTransform::~GTHoughTransform() {
    delete []data;
}
