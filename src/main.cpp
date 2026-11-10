#include "mainwindow.h"
#include "crypto.h"

#include <QApplication>
#include <QStyleFactory>
#include <QMessageBox>
#include <QSettings>
#include <QTranslator>
#include <QLibraryInfo>

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

    // ===== LANGUAGE =====
    QSettings settings("ThunderPass", "ThunderPass");
    QString language = settings.value("ui/language", "it").toString();

    QTranslator appTranslator;
    QTranslator qtTranslator;


    if (language != "it") {
        if (appTranslator.load("thunderpass_" + language, ":/translations")) {
            app.installTranslator(&appTranslator);
        } else if (appTranslator.load("thunderpass_" + language, app.applicationDirPath() + "/translations")) {
            app.installTranslator(&appTranslator);
        }
    }


    if (qtTranslator.load("qt_" + language,
                          QLibraryInfo::path(QLibraryInfo::TranslationsPath))) {
        app.installTranslator(&qtTranslator);
    }

    MainWindow window;
    window.show();

    return app.exec();
}