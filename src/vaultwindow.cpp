#include "vaultwindow.h"
#include "ui_vaultwindow.h"
#include "passwordentry.h"
#include "addpassworddialog.h"

#include <QMessageBox>
#include <QInputDialog>

VaultWindow::VaultWindow(const QString &userEmail, VaultManager *vault, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::VaultWindow)
    , m_userEmail(userEmail)
    , m_vault(vault)
{
    ui->setupUi(this);

    setWindowTitle("ThunderPass - " + m_userEmail);
    resize(900, 600);

    connect(ui->addPasswordButton, &QPushButton::clicked,
            this, &VaultWindow::onAddPasswordClicked);

    connect(ui->logoutButton, &QPushButton::clicked,
            this, &VaultWindow::onLogoutClicked);

    refreshList();
}

VaultWindow::~VaultWindow()
{
}

void VaultWindow::refreshList()
{
    ui->passwordList->clear();

    const auto entries = m_vault->entries();
    for (const PasswordEntry &entry : entries) {
        QString text = entry.title();
        if (!entry.username().isEmpty()) {
            text += "  •  " + entry.username();
        }
        ui->passwordList->addItem(text);
    }
}

void VaultWindow::onAddPasswordClicked()
{
    AddPasswordDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        PasswordEntry entry = dialog.resultEntry();
        if (m_vault->addEntry(entry)) {
            refreshList();
        } else {
            QMessageBox::critical(this, "Errore", "Impossibile salvare la password.");
        }
    }
}

void VaultWindow::onLogoutClicked()
{
    m_vault->lock(); // cancelliamo la chiave dalla memoria
    close();
}