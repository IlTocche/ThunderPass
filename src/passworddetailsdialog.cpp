#include "passworddetailsdialog.h"
#include "ui_passworddetailsdialog.h"
#include "addpassworddialog.h"

#include <QClipboard>
#include <QGuiApplication>
#include <QToolTip>
#include <qmessagebox.h>

PasswordDetailsDialog::PasswordDetailsDialog(const PasswordEntry &entry, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::PasswordDetailsDialog)
    , m_entry(entry)
{
    ui->setupUi(this);

    setWindowTitle(m_entry.title());
    setFixedSize(420, 420);
    setModal(true);

    // Riempiamo i campi
    ui->titleValue->setText(m_entry.title());
    ui->usernameValue->setText(m_entry.username().isEmpty() ? "—" : m_entry.username());
    ui->passwordValue->setText("••••••••••••");
    ui->urlValue->setText(m_entry.url().isEmpty() ? "—" : m_entry.url());
    ui->notesValue->setText(m_entry.notes().isEmpty() ? "—" : m_entry.notes());

    connect(ui->copyUsernameButton, &QPushButton::clicked,
            this, &PasswordDetailsDialog::onCopyUsername);

    connect(ui->copyPasswordButton, &QPushButton::clicked,
            this, &PasswordDetailsDialog::onCopyPassword);

    connect(ui->togglePasswordButton, &QPushButton::clicked,
            this, &PasswordDetailsDialog::onTogglePasswordVisibility);

    connect(ui->closeButton, &QPushButton::clicked,
            this, &PasswordDetailsDialog::onCloseClicked);

    connect(ui->deleteButton, &QPushButton::clicked,
            this, &PasswordDetailsDialog::onDeleteClicked);

    connect(ui->editButton, &QPushButton::clicked,
            this, &PasswordDetailsDialog::onEditClicked);
}

PasswordDetailsDialog::~PasswordDetailsDialog()
{
}

void PasswordDetailsDialog::onCopyUsername()
{
    QGuiApplication::clipboard()->setText(m_entry.username());
    QToolTip::showText(QCursor::pos(), "Username copiato!");
}

void PasswordDetailsDialog::onCopyPassword()
{
    QGuiApplication::clipboard()->setText(m_entry.password());
    QToolTip::showText(QCursor::pos(), "Password copiata!");
}

void PasswordDetailsDialog::onTogglePasswordVisibility()
{
    m_passwordVisible = !m_passwordVisible;

    if (m_passwordVisible) {
        ui->passwordValue->setText(m_entry.password());
        ui->togglePasswordButton->setText("Nascondi");
    } else {
        ui->passwordValue->setText("••••••••••••");
        ui->togglePasswordButton->setText("Mostra");
    }
}

void PasswordDetailsDialog::onEditClicked()
{
    AddPasswordDialog dialog(m_entry, this);
    if (dialog.exec() == QDialog::Accepted) {
        m_entry = dialog.resultEntry();
        emit entryUpdated(m_entry);
        accept(); // chiudiamo i dettagli dopo la modifica
    }
}

void PasswordDetailsDialog::onDeleteClicked()
{
    auto reply = QMessageBox::question(
        this,
        "Conferma eliminazione",
        "Sei sicuro di voler eliminare questa password?\nQuesta azione non può essere annullata.",
        QMessageBox::Yes | QMessageBox::No
        );

    if (reply == QMessageBox::Yes) {
        emit entryDeleted(m_entry.id());
        accept(); // chiude la dialog
    }
}
void PasswordDetailsDialog::onCloseClicked()
{
    accept();
}