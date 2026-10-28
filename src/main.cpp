#include "mainwindow.h"
#include "crypto.h"

#include <QApplication>
#include <QStyleFactory>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);


    if (!Crypto::initialize()) {
        QMessageBox::critical(nullptr, "Errore critico",
                              "Impossibile inizializzare il motore crittografico.");
        return 1;
    }

    app.setStyle(QStyleFactory::create("Fusion"));
    app.setApplicationName("ThunderPass");
    app.setOrganizationName("ThunderPass");
    app.setApplicationVersion("0.1.0");

    MainWindow window;
    window.show();

    return app.exec();
}