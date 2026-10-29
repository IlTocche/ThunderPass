#include "addpassworddialog.h"
#include "ui_addpassworddialog.h"

#include <QMessageBox>
#include <sodium.h>

AddPasswordDialog::AddPasswordDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AddPasswordDialog)
{
    ui->setupUi(this);

    setWindowTitle("Aggiungi password");
    setFixedSize(420, 480);
    setModal(true);

    setupConnections();
}

AddPasswordDialog::AddPasswordDialog(const PasswordEntry &entryToEdit, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AddPasswordDialog)
    , m_entry(entryToEdit)
{
    ui->setupUi(this);

    setWindowTitle("Modifica password");
    setFixedSize(420, 480);
    setModal(true);

    fillForm();
    setupConnections();
}

AddPasswordDialog::~AddPasswordDialog()
{
}

void AddPasswordDialog::setupConnections()
{
    connect(ui->saveButton, &QPushButton::clicked,
            this, &AddPasswordDialog::onSaveClicked);

    connect(ui->cancelButton, &QPushButton::clicked,
            this, &AddPasswordDialog::onCancelClicked);

    connect(ui->generateButton, &QPushButton::clicked,
            this, &AddPasswordDialog::onGenerateClicked);
}

void AddPasswordDialog::fillForm()
{
    ui->titleEdit->setText(m_entry.title());
    ui->usernameEdit->setText(m_entry.username());
    ui->passwordEdit->setText(m_entry.password());
    ui->urlEdit->setText(m_entry.url());
    ui->notesEdit->setPlainText(m_entry.notes());
}

PasswordEntry AddPasswordDialog::resultEntry() const
{
    return m_entry;
}

void AddPasswordDialog::onSaveClicked()
{
    QString title = ui->titleEdit->text().trimmed();
    QString username = ui->usernameEdit->text().trimmed();
    QString password = ui->passwordEdit->text();
    QString url = ui->urlEdit->text().trimmed();
    QString notes = ui->notesEdit->toPlainText().trimmed();

    if (title.isEmpty()) {
        QMessageBox::warning(this, "Attenzione", "Il titolo è obbligatorio.");
        return;
    }

    if (password.isEmpty()) {
        QMessageBox::warning(this, "Attenzione", "La password è obbligatoria.");
        return;
    }

    if (m_entry.id().isEmpty()) {
        // Nuova password
        m_entry = PasswordEntry::createNew(title, username, password, url, notes);
    } else {
        // Modifica esistente
        m_entry.setTitle(title);
        m_entry.setUsername(username);
        m_entry.setPassword(password);
        m_entry.setUrl(url);
        m_entry.setNotes(notes);
    }

    accept();
}

void AddPasswordDialog::onCancelClicked()
{
    reject();
}

void AddPasswordDialog::onGenerateClicked()
{
    // Genera una password sicura di 20 caratteri
    const char charset[] = "abcdefghijkmnopqrstuvwxyzABCDEFGHJKLMNPQRSTUVWXYZ23456789!@#$%&*";
    const int length = 20;
    QString password;

    for (int i = 0; i < length; ++i) {
        unsigned char randomByte;
        randombytes_buf(&randomByte, 1);
        password.append(charset[randomByte % (sizeof(charset) - 1)]);
    }

    ui->passwordEdit->setText(password);
    ui->passwordEdit->setEchoMode(QLineEdit::Normal); // la mostriamo
}