#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QScreen>
#include <QStyle>
#include <QMenuBar>
#include <QStatusBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Titolo
    setWindowTitle("ThunderPass");

    // Dimensioni fisse
    setFixedSize(420, 520);

    // Centra la finestra
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
