#include "mainwindow.h"

#include <QApplication>
#include <QStyleFactory>

int main(int argc, char *argv[])
{

    QApplication app(argc, argv);


    app.setStyle(QStyleFactory::create("Fusion"));

    app.setApplicationName("ThunderPass");
    app.setOrganizationName("ThunderPass");
    app.setApplicationVersion("0.1.0");

    MainWindow window;

    window.show();

    return app.exec();
}
