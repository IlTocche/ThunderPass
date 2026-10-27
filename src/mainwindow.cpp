#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QScreen>
#include <QStyle>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

    ui->setupUi(this);


    setWindowTitle("ThunderPass");


    resize(920, 620);


    setGeometry(
        QStyle::alignedRect(
            Qt::LeftToRight,
            Qt::AlignCenter,
            size(),
            qApp->primaryScreen()->availableGeometry()
            )
        );
}


MainWindow::~MainWindow()
{
    delete ui;
}
