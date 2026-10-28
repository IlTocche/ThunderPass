#include "mainwindow.h"
#include "registerdialog.h"
#include "ui_mainwindow.h"

#include <QScreen>
#include <QStyle>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("ThunderPass");
    setFixedSize(420, 520);


    setGeometry(
        QStyle::alignedRect(
            Qt::LeftToRight,
            Qt::AlignCenter,
            size(),
            qApp->primaryScreen()->availableGeometry()
            )
        );

    connect(ui->loginButton, &QPushButton::clicked,
            this, &MainWindow::onLoginClicked);


    connect(ui->createAccountButton, &QPushButton::clicked,
            this, &MainWindow::onCreateAccountClicked);
}

MainWindow::~MainWindow()
{
}


void MainWindow::onLoginClicked()
{
    QString email = ui->emailEdit->text().trimmed();
    QString password = ui->passwordEdit->text();

    if (email.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Attenzione", "Inserisci email e password.");
        return;
    }

    if (m_auth.login(email, password)) {
        QMessageBox::information(this, "Successo",
                                 "Accesso effettuato!\nBenvenuto " + m_auth.currentUserEmail());
        // Più avanti qui apriremo la schermata principale del password manager
    } else {
        QMessageBox::critical(this, "Errore", "Email o password non corretti.");
    }
}

void MainWindow::onCreateAccountClicked()
{
    RegisterDialog dialog(&m_auth, this);
    dialog.exec();
}