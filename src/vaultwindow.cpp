#include "vaultwindow.h"
#include "ui_vaultwindow.h"
#include "passwordentry.h"

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
    // Per ora usiamo dei dialog semplici (dopo faremo una bella form)
    bool ok;
    QString title = QInputDialog::getText(this, "Nuova password",
                                          "Titolo (es. Gmail, Netflix):",
                                          QLineEdit::Normal, "", &ok);
    if (!ok || title.trimmed().isEmpty()) return;

    QString username = QInputDialog::getText(this, "Nuova password",
                                             "Username / Email:",
                                             QLineEdit::Normal, "", &ok);
    if (!ok) return;

    QString password = QInputDialog::getText(this, "Nuova password",
                                             "Password:",
                                             QLineEdit::Password, "", &ok);
    if (!ok || password.isEmpty()) return;

    QString url = QInputDialog::getText(this, "Nuova password",
                                        "Sito web (opzionale):",
                                        QLineEdit::Normal, "", &ok);

    PasswordEntry entry = PasswordEntry::createNew(title.trimmed(),
                                                   username.trimmed(),
                                                   password,
                                                   url.trimmed());

    if (m_vault->addEntry(entry)) {
        refreshList();
        QMessageBox::information(this, "Successo", "Password salvata in modo cifrato!");
    } else {
        QMessageBox::critical(this, "Errore", "Impossibile salvare la password.");
    }
}

void VaultWindow::onLogoutClicked()
{
    m_vault->lock(); // cancelliamo la chiave dalla memoria
    close();
}