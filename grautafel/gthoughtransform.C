#include "gthoughtransform.h"
#include "gtconvolutionkernel.h"
#include <QImage>
#include <QtGlobal>


static const double pi = 3.14159265358979323846;

#include <math.h>
static inline int sq(int x) {return x*x;}

void GTHoughTransform::set(int r, int alpha, double val) {
    data[r * angleRes + alpha] = val;
}

void GTHoughTransform::add(int r, int alpha, double val) {
    data[r * angleRes + alpha] += val;
}

double GTHoughTransform::get(int r, int alpha) const {
    return data[r * angleRes + alpha];
}

double GTHoughTransform::get(const coords c) const {
  return data[c.r + angleRes +c.alpha];
}

GTHoughTransform::GTHoughTransform(const QImage *src, int angleResolution, int margin)
{
    leftMargin = rightMargin = topMargin = bottomMargin = margin;
    originalHeight = src->height();
    originalWidth = src->width();
    radius = (int) sqrt(sq(src->height()) + sq(src->width())) / 2;
    angleRes = angleResolution;

    data = new double[radius * angleResolution]();
    // Center coordinates
    int cX = src->width()/2;
    int cY = src->height()/2;

    for (int x = edgeDetectorX->xorig + leftMargin; x < src->width() - (edgeDetectorX->xsiz - edgeDetectorX->xorig) - rightMargin; x++)
        for (int y = edgeDetectorX->yorig + topMargin; y < src->height() - (edgeDetectorX->ysiz - edgeDetectorX->yorig)- bottomMargin; y++){
            QPoint pos(x,y);
            double val = edgeDetectorX->dConvolveSquared(src, pos);
            val += edgeDetectorY->dConvolveSquared(src, pos);
            val = sqrt(val);
//            int R = (int) round(sqrt(sq(x-cX)+sq(y-cY))) % radius; //FIXME je modulo nutné
            int Alpha = ((int) round(atan2(y-cY, x-cX) * angleRes / (2 * pi))) % angleRes;
            for (int alphaDif = -angleRes / 4; alphaDif < angleRes / 4 ; alphaDif++) {
                int alpha = (Alpha + alphaDif + angleRes) % angleRes;
                double a = alpha * 2 * pi / angleRes;
                int r = (int) round((x-cX)*cos(a) + (y-cY)*sin(a));
                if (r >= 0)
                    add(r, alpha, val);
            }
        }
    coords_by_value_init();
    validity = true;
}

#include <algorithm>

void GTHoughTransform::coords_by_value_init(void) {
  coords_by_value.resize(radius * angleRes);
  for(int r = 0; r < radius; r++)
    for(int alpha = 0; alpha < angleRes; alpha++){
      coords_by_value[r * angleRes + alpha].r = r;
      coords_by_value[r * angleRes + alpha].alpha = alpha;
      }
  cmpstruct s(this);
  std::sort(coords_by_value.begin(), coords_by_value.end(), s);
}


std::list<coords> GTHoughTransform::roughCorners(){
  TODO

}

QImage GTHoughTransform::visualise(void) const {
    double max = 0;
    for (int r = 0; r < radius; r++)
        for (int alpha = 0; alpha < angleRes; alpha++)
            if (max < get(r,alpha)) max = get(r,alpha);

    QImage v(angleRes, radius, QImage::Format_Indexed8);
    QVector<QRgb> graytable;
    for(int i = 0; i < 256; i++) graytable.push_back(qRgb(0,i,0));
    v.setColorTable(graytable);

    for (int r = 0; r < radius; r++)
        for (int alpha = 0; alpha < angleRes; alpha++)
            v.setPixel(alpha, r, get(r,alpha)*255/max);
    return v;
}

//   const uchar **lines = new *uchar[kernYsiz];
//   for(int yu = 0; yu <= src->height() - kernYsiz; y++) {
//     for(int l = 0; l < kernYsiz; l++)


//   }
//   validity = true;


//}

double *GTHoughTransform::operator[](int r) const{
  return data + r * angleRes;
}


GTHoughTransform::~GTHoughTransform() {
    delete []data;
}
