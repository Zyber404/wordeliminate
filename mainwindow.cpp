
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_loginwindow.h"
#include "qtmaterialtextfield.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loginclosed(loginWindow *ptr_loginWindow)
{
    delete ptr_loginWindow;
}


