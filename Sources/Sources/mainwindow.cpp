#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
//    ui->setupUi(this);

    SerialAssistant *serialAssistant = new SerialAssistant;
    serialAssistant->show();


    QThread threadA,threadB;
}

MainWindow::~MainWindow()
{
    delete ui;
}


