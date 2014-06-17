#include "gthoughtransform.h"
#include <QImage>
#include <QtGlobal>

static const int sobelX[3][3] = {{-1, 0, 1},
                                 {-2, 0, 2},
                                 {-1, 0, 1}};
static const int sobelY[3][3] = {{ 1,  2,  1},
                                 { 0,  0,  0},
                                 {-1, -2, -1}};

static const int prewittX[3][3] = {{-1, 0, 1},
                                   {-1, 0, 1},
                                   {-1, 0, 1}};
static const int prewittY[3][3] = {{ 1,  1,  1},
                                   { 0,  0,  0},
                                   {-1, -1, -1}};

static const int roberts1[2][2] = {{1,  0},
                                   {0, -1}};
static const int roberts2[2][2] = {{ 0, 1},
                                   {-1, 0}};

GTHoughTransform::GTHoughTransform(QObject *parent) :
  QObject(parent)
{
  data = NULL;
  validity = false;
}

GTHoughTransform(QImage *src, int angleResolution, QObject *parent = 0) :
  GTHoughTransform(parent)
{
   if(QImage::Format_ARGB32 != src->format()) {
       qWarning("Only ARGB32 format is supported by GTHoughTransform at the moment.");
       return;
   }
   const int kernXsiz = 3;
   const int kernYsiz = 3;
   const int kernXorig = 1;
   const int kernYorig = 1;
   static const int (*const kernelX)[kernXsiz] = sobelX;
   static const int (*const kernelY)[kernYsiz] = sobelY;

   data = new int[(src->height() -  * angleResolution]
   const uchar **lines = new *uchar[kernYsiz];
   for(int yu = 0; yu <= src->height() - kernYsiz; y++) {
     for(int l = 0; l < kernYsiz; l++)


   }


}

~GTHoughTransform();
