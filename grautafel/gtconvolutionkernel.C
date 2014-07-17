#include "gtconvolutionkernel.h"
#include <QColor>
#include <cmath>


static const int sobelXMatrix[3][3] = {{-1, 0, 1},
                                       {-2, 0, 2},
                                       {-1, 0, 1}};
static const int sobelYMatrix[3][3] = {{ 1,  2,  1},
                                       { 0,  0,  0},
                                       {-1, -2, -1}};

static const int prewittXMatrix[3][3] = {{-1, 0, 1},
                                         {-1, 0, 1},
                                         {-1, 0, 1}};
static const int prewittYMatrix[3][3] = {{ 1,  1,  1},
                                         { 0,  0,  0},
                                         {-1, -1, -1}};

static const int roberts1Matrix[2][2] = {{1,  0},
                                         {0, -1}};
static const int roberts2Matrix[2][2] = {{ 0, 1},
                                         {-1, 0}};

static inline int sq(int a){
    return a*a;
}

// Příklady konvolučních jader na výpočet gradientu.
// TODO namespace nebo něco
const struct GTConvolutionKernel sobelX = {3, 3, // xsiz, ysiz
                                     1, 1, // xorig, yorig
                                     (unsigned char *) sobelXMatrix, // data
                                     1, 8, // nNum, nDen
                                    };

const struct GTConvolutionKernel sobelY = {3, 3, // xsiz, ysiz
                                     1, 1, // xorig, yorig
                                     (unsigned char *) sobelYMatrix, // data
                                     1, 8  // nNum, nDen
                                    };

const struct GTConvolutionKernel prewittX = {3, 3, //xsiz, ysiz
                                       1, 1, //xorig, yorig
                                       (unsigned char *) prewittXMatrix, //data
                                       1, 6 // nNum, nDen
                                      };
const struct GTConvolutionKernel prewittY = {3, 3, //xsiz, ysiz
                                       1, 1, //xorig, yorig
                                       (unsigned char *) prewittYMatrix, //data
                                       1, 6 //nNum, nDen
                                      };

const struct GTConvolutionKernel  * const edgeDetectorX = &sobelX, * const edgeDetectorY = &sobelY;


/*!
 * \brief GTConvolutionKernel::convolveSquared
 * \param img
 * \param point
 * \return
 *
 * Evaluates convolution of an image with the kernel at a single point. Without normalization.
 * TODO
 *
 */

int GTConvolutionKernel::convolveSquaredNotNormalised(const QImage *img, QPoint point) const
{
//  if (img->format() != QImage::Format_ARGB32)
//    abort();
  int colsums[3] = {0};
  int (*colfuns[3])(QRgb) = {qRed, qBlue, qGreen};
    for(int yo = 0; yo < ysiz; yo++)
      for(int xo = 0; xo < xsiz; xo++){
          int x = point.x() - xorig + xo;
          int y = point.y() - yorig + yo;
          if (x > 0 && x < img->width() && y > 0 && y < img->height()) {
                QRgb pixval = img->pixel(x, y);
                int kernval = ((int *)data)[xsiz * yo + xo];
              // TODO neúplný obdélník, hranice, normalisace
               for(int color = 0; color < 3; color++) {
                   colsums[color] += kernval * colfuns[color](pixval);
                 }
            }

        }
    // TODO popř. jinou funkci. Možná by stálo za to vracet tři barvy.
    int totalsum = 0;
    for (int color = 0; color < 3; color++)
      totalsum += sq(colsums[color]);
    return totalsum ;// sq(normDenominator) / 9;
}

int GTConvolutionKernel::convolveSquared(const QImage *img, QPoint point, int additionalMultiplier) const
{
    return sqrt(convolveSquaredNotNormalised(img, point) * sq(additionalMultiplier) / sq(normDenominator) / 9);
}

double GTConvolutionKernel::dConvolveSquared(const QImage *img, QPoint point) const {
    return (double) convolveSquaredNotNormalised(img, point)  / (double) sq(normDenominator) / 9.;
}



// Slow and ugly version
QImage *edgePreview(const QImage *img, QRect area){ // Outputs grayscale, too
    // QImage supports only indexed 8-bit images, so we make it grayscale
    // by making "linear" index table
    QImage *preview = new QImage(area.size(), QImage::Format_Indexed8);
    QVector<QRgb> graytable;
    for(int i = 0; i < 256; i++) graytable.push_back(qRgb(i,i,i));
    preview->setColorTable(graytable);

    for (int i = 0; i < area.height(); i++)
        for (int j = 0; j < area.width(); j++) {
            QPoint offset(j,i);
            int xgrad = edgeDetectorX->convolveSquared(img, offset+area.topLeft(),100);
            int ygrad = edgeDetectorY->convolveSquared(img, offset+area.topLeft(),100);
            preview->setPixel(offset, (uint) sqrt(xgrad + ygrad)); //FIXME nanormovat
        }
    return preview;
}
