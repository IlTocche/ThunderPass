#include "mainwindow.h"
#include "registerdialog.h"
#include "ui_mainwindow.h"
#include "vaultwindow.h"

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

    // Prima autenticazione classica
    if (!m_auth.login(email, password)) {
        QMessageBox::critical(this, "Errore", "Email o password non corretti.");
        return;
    }

    // Poi sblocchiamo il vault con la stessa password (master password)
    if (!m_vault.unlock(email, password)) {
        QMessageBox::critical(this, "Errore", "Impossibile sbloccare il vault.");
        return;
    }

    // Tutto ok → apriamo la vault
    VaultWindow *vaultWin = new VaultWindow(email, &m_vault);
    vaultWin->setAttribute(Qt::WA_DeleteOnClose);
    vaultWin->show();

    this->hide();
}

void MainWindow::onCreateAccountClicked()
{
    RegisterDialog dialog(&m_auth, this);
    dialog.exec();
}