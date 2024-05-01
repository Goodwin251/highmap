#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    QPushButton pb("Text", this);
    QImage image("/Iraq_mountains.png", "png");
    ui->setupUi(this);

}

MainWindow::~MainWindow()
{
    delete ui;
}
