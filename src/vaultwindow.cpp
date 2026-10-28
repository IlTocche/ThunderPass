#include "vaultwindow.h"
#include "ui_vaultwindow.h"

#include <QMessageBox>

VaultWindow::VaultWindow(const QString &userEmail, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::VaultWindow)
    , m_userEmail(userEmail)
{
    ui->setupUi(this);

    setWindowTitle("ThunderPass - " + m_userEmail);
    resize(900, 600);

    // Collegamenti
    connect(ui->addPasswordButton, &QPushButton::clicked,
            this, &VaultWindow::onAddPasswordClicked);

    connect(ui->logoutButton, &QPushButton::clicked,
            this, &VaultWindow::onLogoutClicked);
}

VaultWindow::~VaultWindow()
{
}

void VaultWindow::onAddPasswordClicked()
{
    QMessageBox::information(this, "Aggiungi",
                             "Qui più avanti apriremo il form per aggiungere una nuova password.");
}

void VaultWindow::onLogoutClicked()
{
    // Chiudiamo questa finestra (torneremo al login)
    close();
}