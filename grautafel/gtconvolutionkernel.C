#include "gtconvolutionkernel.h"
#include <QColor>

static inline int sq(int x)
{
  return x * x;
}

int convolveSquared(QImage *img, QPoint point)
{
  if (img->format() != QImage::Format_ARGB32)
    abort();
  int colsums[3] = 0;
  int *colfuns[3](QRgb) = {qRed, qBlue, qGreen};
    for(int yo = 0; yo < ysiz; y++)
      for(int xo = 0; xo < xsiz; x++){
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
      totalsum += sq(colsums[color]) / sq(normDenominator);
    return totalsum;
}

//QImage convolve(QImage *img, QRect area); // Outputs grayscale, too


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


// Příklady konvolučních jader na výpočet gradientu.
// TODO namespace nebo něco
struct GTConvolutionKernel sobelX = {3, 3, // xsiz, ysiz
                                     1, 1, // xorig, yorig
                                     (unsigned char *) sobelXMatrix, // data
                                     1, 8, // nNum, nDen
                                    };

struct GTConvolutionKernel sobelY = {3, 3, // xsiz, ysiz
                                     1, 1, // xorig, yorig
                                     (unsigned char *) sobelYMatrix, // data
                                     1, 8  // nNum, nDen
                                    };

struct GTConvolutionKernel prewittX = {3, 3, //xsiz, ysiz
                                       1, 1, //xorig, yorig
                                       (unsigned char *) prewittXMatrix, //data
                                       1, 6 // nNum, nDen
                                      };
struct GTConvolutionKernel prewittY = {3, 3, //xsiz, ysiz
                                       1, 1, //xorig, yorig
                                       (unsigned char *) prewittYMatrix, //data
                                       1, 6 //nNum, nDen
                                      };
