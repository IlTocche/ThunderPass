#include "vaultwindow.h"
#include "ui_vaultwindow.h"
#include "passwordentry.h"
#include "addpassworddialog.h"
#include "passworddetailsdialog.h"

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

    connect(ui->passwordList, &QListWidget::itemClicked,
            this, [this](QListWidgetItem *item) {
                int row = ui->passwordList->row(item);
                const auto entries = m_vault->entries();
                if (row >= 0 && row < entries.size()) {
                    PasswordDetailsDialog dialog(entries.at(row), this);

                    // Quando l'utente elimina la password
                    connect(&dialog, &PasswordDetailsDialog::entryDeleted,
                            this, [this](const QString &id) {
                                if (m_vault->removeEntry(id)) {
                                    refreshList();
                                }
                            });

                    dialog.exec();
                }
            });
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