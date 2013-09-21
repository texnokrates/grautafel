#include "mainwindow.h"
#include "ui_mainwindow.h"

#define TESTIMG "/home/mmn/foto/fotky1/DSC_0298.JPG"


// ================= test ===================
#include "gtimage.h"
#include <QColor>
#include <QLabel>
#include <QImage>
#include <QRect>
#include <QPalette>
#include <QVBoxLayout>

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
