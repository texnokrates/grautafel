#ifndef GTCONVOLUTIONKERNEL_H
#define GTCONVOLUTIONKERNEL_H
#include <QImage>
#include <QPoint>
#include <QRect>

/**
 * Convolution kernel for QImage.
 *
 * The current implementation has the following assumptions:
 * All image data is ARGB32.
 * On the other hand, the kernel is grayscale and applies to each RGB symmetrically, using Euclidean metric.
 */
struct GTConvolutionKernel
{
  int xsiz, ysiz;
  int xorig, yorig;
  unsigned char *data;
  int normNumerator, normDenominator; // rozmyslet: nemusí být float?
//  QImage::format format;
  int convolveSquared(QImage *img, QPoint point); // Outputs *grayscale*; for test purposes only
//  QImage convolveSquared(QImage *img, QRect area); // Outputs grayscale, too

};

GTConvolutionKernel sobelX, sobelY, prewittX, prewittY, robertsPlus, robertsMinus;

#endif // GTCONVOLUTIONKERNEL_H
