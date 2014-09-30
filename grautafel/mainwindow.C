#include "mainwindow.h"
//#include "ui_mainwindow.h"
#include "gtmainwidget.h"

using namespace GT;

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent)//,
//    ui(new Ui::MainWindow)
{
//    testfun(this);
//    ui->setupUi(this);
  setWindowTitle(trUtf8("Grautafel"));
  setCentralWidget(new MainWidget());

}

MainWindow::~MainWindow() {
//    delete ui;
}

