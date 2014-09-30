#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#ifdef qtdui
namespace Ui {
  class MainWindow;
}
#endif

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

private:
#ifdef qtdui
  Ui::MainWindow *ui;
#endif
};

#endif // MAINWINDOW_H
