#include "vaultwindow.h"
#include "ui_vaultwindow.h"
#include "passwordentry.h"
#include "addpassworddialog.h"
#include "passworddetailsdialog.h"

#include <QMessageBox>
#include <QInputDialog>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QMenu>
#include <QAction>
#include <QClipboard>
#include <QGuiApplication>
#include "addpassworddialog.h"

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

    connect(ui->searchEdit, &QLineEdit::textChanged,
            this, &VaultWindow::onSearchTextChanged);

    // Timer di inattività
    m_inactivityTimer.setSingleShot(true);
    connect(&m_inactivityTimer, &QTimer::timeout,
            this, &VaultWindow::onInactivityTimeout);

    resetInactivityTimer(); // avvia il timer


    // Abilita il menu contestuale
    ui->passwordList->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->passwordList, &QListWidget::customContextMenuRequested,
            this, &VaultWindow::onCustomContextMenu);
}

VaultWindow::~VaultWindow()
{
}

void VaultWindow::refreshList()
{
    // Riusiamo la stessa logica della ricerca
    onSearchTextChanged(ui->searchEdit->text());
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

void VaultWindow::onSearchTextChanged(const QString &text)
{
    ui->passwordList->clear();

    const QString search = text.trimmed().toLower();
    const auto entries = m_vault->entries();

    for (const PasswordEntry &entry : entries) {
        // Cerca nel titolo, username e url
        bool match = search.isEmpty()
                     || entry.title().toLower().contains(search)
                     || entry.username().toLower().contains(search)
                     || entry.url().toLower().contains(search);

        if (match) {
            QString display = entry.title();
            if (!entry.username().isEmpty()) {
                display += "  •  " + entry.username();
            }
            ui->passwordList->addItem(display);
        }
    }
}

void VaultWindow::onCustomContextMenu(const QPoint &pos)
{
    QListWidgetItem *item = ui->passwordList->itemAt(pos);
    if (!item) return;

    int row = ui->passwordList->row(item);
    const auto entries = m_vault->entries();
    if (row < 0 || row >= entries.size()) return;

    // Dobbiamo trovare l'entry corretta anche con la ricerca attiva
    // Per semplicità ricarichiamo tutte e filtriamo
    QString search = ui->searchEdit->text().trimmed().toLower();
    QVector<PasswordEntry> visibleEntries;

    for (const PasswordEntry &entry : entries) {
        bool match = search.isEmpty()
        || entry.title().toLower().contains(search)
            || entry.username().toLower().contains(search)
            || entry.url().toLower().contains(search);
        if (match) {
            visibleEntries.append(entry);
        }
    }

    if (row >= visibleEntries.size()) return;
    PasswordEntry entry = visibleEntries.at(row);

    // Creiamo il menu
    QMenu menu(this);

    QAction *copyUserAction = menu.addAction("Copia username");
    QAction *copyPassAction = menu.addAction("Copia password");
    menu.addSeparator();
    QAction *editAction = menu.addAction("Modifica");
    QAction *deleteAction = menu.addAction("Elimina");

    // Stile scuro per il menu
    menu.setStyleSheet(
        "QMenu { background-color: #2c2c2e; color: white; border: 1px solid #3a3a3c; padding: 6px; }"
        "QMenu::item { padding: 8px 24px 8px 14px; border-radius: 6px; }"
        "QMenu::item:selected { background-color: #0a84ff; }"
        "QMenu::separator { height: 1px; background-color: #3a3a3c; margin: 6px 8px; }"
        );

    QAction *selected = menu.exec(ui->passwordList->mapToGlobal(pos));
    if (!selected) return;

    if (selected == copyUserAction) {
        QGuiApplication::clipboard()->setText(entry.username());
    }
    else if (selected == copyPassAction) {
        QGuiApplication::clipboard()->setText(entry.password());
    }
    else if (selected == editAction) {
        AddPasswordDialog dialog(entry, this);
        if (dialog.exec() == QDialog::Accepted) {
            if (m_vault->updateEntry(dialog.resultEntry())) {
                refreshList();
            }
        }
    }
    else if (selected == deleteAction) {
        auto reply = QMessageBox::question(
            this,
            "Conferma eliminazione",
            "Sei sicuro di voler eliminare \"" + entry.title() + "\"?",
            QMessageBox::Yes | QMessageBox::No
            );

        if (reply == QMessageBox::Yes) {
            if (m_vault->removeEntry(entry.id())) {
                refreshList();
            }
        }
    }
}