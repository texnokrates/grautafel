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

//GTHoughTransform::GTHoughTransform(QImage *src, int angleResolution, QObject *parent) :
//  QObject(parent)
//{
//   if(QImage::Format_ARGB32 != src->format()) {
//       qWarning("Only ARGB32 format is supported by GTHoughTransform at the moment.");
//       return;
//   }
//   const int kernXsiz = 3;
//   const int kernYsiz = 3;
//   const int kernXorig = 1;
//   const int kernYorig = 1;
//   static const int (*const kernelX)[kernXsiz] = sobelX;
//   static const int (*const kernelY)[kernYsiz] = sobelY;

//   data = new int[(src->height() -  * angleResolution]
//   const uchar **lines = new *uchar[kernYsiz];
//   for(int yu = 0; yu <= src->height() - kernYsiz; y++) {
//     for(int l = 0; l < kernYsiz; l++)


//   }
//   validity = true;


//}

GTHoughTransform::~GTHoughTransform() {}
