#include "mainwindow.h"
#include "ui_mainwindow.h"

#define TESTIMG "/home/necadam1/repo/grautafel/testy/my_blackboard.jpg"


// ================= test ===================
#include "gtimage.h"
#include "gtconvolutionkernel.h"
#include <QColor>
#include <QLabel>
#include <QImage>
#include <QRect>
#include <QPalette>
#include <QVBoxLayout>
#include <gthoughtransform.h>

#include <QDir>
#include <QDebug>
#include <QTime>

void testfun(QMainWindow *where) {
    GTImage img(TESTIMG);
    QList<QColor> cols =
        img.getColorQuantiles(QRect(img.srcWidth()/3,img.srcHeight()/3,img.srcWidth()/3,img.srcHeight()/3),
                              QList<qreal>() << 0.2 << 0.5 << 0.8);
    QWidget *window = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout;
    for(QList<QColor>::const_iterator i = cols.begin(); i != cols.end(); ++i){
        QLabel *lab = new QLabel;
        QPalette pal = lab->palette();
        lab->setAutoFillBackground(true);
        pal.setColor(QPalette::Normal,QPalette::Background, *i);
        lab->setPalette(pal);
        lab->setText("Barva:" + (*i).name());

        layout->addWidget(lab);

    }
    window->setLayout(layout);
    window->show();

    // Another test
    QImage srcimg(TESTIMG);
    QLabel *win2 = new QLabel;
    QPixmap *pmap = new QPixmap;
    pmap->convertFromImage(srcimg);
    win2->setPixmap(*pmap);
    win2->show();
    QImage *edgeimg = edgePreview(&srcimg, QRect(3,3,srcimg.width(),srcimg.height()));
    QLabel *win3 = new QLabel;
    QPixmap *pmap2 = new QPixmap;
    pmap2->convertFromImage(*edgeimg);
    win3->setPixmap(*pmap2);
    win3->show();

    QTime t;
    t.start();
    GTHoughTransform *ht = new GTHoughTransform(&srcimg, 500);
    qDebug("Time elapsed: %d ms", t.elapsed());
    QPixmap *vizmap = new QPixmap;
    QImage vizimg = ht->visualise();
    vizimg.save("/tmp/test.png");
    vizmap->convertFromImage(vizimg);
    QLabel *win4 = new QLabel;
    win4->setPixmap(*vizmap);
    win4->show();

    qDebug() << QDir::currentPath();
}
// ================= test ===================

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    testfun(this);
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}
