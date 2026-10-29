#include "vaultwindow.h"
#include "ui_vaultwindow.h"
#include "passwordentry.h"
#include "addpassworddialog.h"
#include "passworddetailsdialog.h"

#include <QMessageBox>
#include <QInputDialog>
#include <QMouseEvent>
#include <QKeyEvent>

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
                    connect(&dialog, &PasswordDetailsDialog::entryUpdated,
                            this, [this](const PasswordEntry &updated) {
                                if (m_vault->updateEntry(updated)) {
                                    refreshList();
                                }
                            });

                    dialog.exec();
                }
            });

    // Timer di inattività
    m_inactivityTimer.setSingleShot(true);
    connect(&m_inactivityTimer, &QTimer::timeout,
            this, &VaultWindow::onInactivityTimeout);

    resetInactivityTimer(); // avvia il timer
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
    m_inactivityTimer.stop();
    m_vault->lock();
    close();
}


void VaultWindow::resetInactivityTimer()
{
    m_inactivityTimer.start(INACTIVITY_TIMEOUT_MS);
}

void VaultWindow::onInactivityTimeout()
{
    // Tempo scaduto → blocchiamo il vault
    m_vault->lock();

    QMessageBox::information(this, "Sessione scaduta",
                             "Sei stato disconnesso per inattività.");

    close(); // torna al login
}

void VaultWindow::mousePressEvent(QMouseEvent *event)
{
    resetInactivityTimer();
    QMainWindow::mousePressEvent(event);
}

void VaultWindow::keyPressEvent(QKeyEvent *event)
{
    resetInactivityTimer();
    QMainWindow::keyPressEvent(event);
}