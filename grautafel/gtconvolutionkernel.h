#ifndef GTCONVOLUTIONKERNEL_H
#define GTCONVOLUTIONKERNEL_H
#include <QIMage>
#include <QPoint>
#include <QRect>

// Now we suppose all image data is ARGB32
class GTConvolutionKernel
{
  int xsiz, ysiz;
  int xorig, yorig;
  unsigned char *data;
  int normNumerator, normDenominator; // rozmyslet: nemusí být float?
//  QImage::format format;
  GTConvolutionKernel();

public:
  GTConvolutionKernel(int xSiz, int ySiz, int xOrig, int yOrig, unsigned char *data);
  int convolve(QImage *img, QPoint point);
  QImage convolve(QImage *img, QRect area);

};

#endif // GTCONVOLUTIONKERNEL_H
