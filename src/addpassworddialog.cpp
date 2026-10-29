#include "addpassworddialog.h"
#include "ui_addpassworddialog.h"

#include <QMessageBox>

AddPasswordDialog::AddPasswordDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AddPasswordDialog)
{
    ui->setupUi(this);

    setWindowTitle("Aggiungi password");
    setFixedSize(420, 480);
    setModal(true);

    connect(ui->saveButton, &QPushButton::clicked,
            this, &AddPasswordDialog::onSaveClicked);

    connect(ui->cancelButton, &QPushButton::clicked,
            this, &AddPasswordDialog::onCancelClicked);
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

    // Riempiamo i campi con i dati esistenti
    ui->titleEdit->setText(m_entry.title());
    ui->usernameEdit->setText(m_entry.username());
    ui->passwordEdit->setText(m_entry.password());
    ui->urlEdit->setText(m_entry.url());
    ui->notesEdit->setPlainText(m_entry.notes());

    connect(ui->saveButton, &QPushButton::clicked,
            this, &AddPasswordDialog::onSaveClicked);

    connect(ui->cancelButton, &QPushButton::clicked,
            this, &AddPasswordDialog::onCancelClicked);
}

AddPasswordDialog::~AddPasswordDialog()
{
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

    // Se m_entry ha già un ID → stiamo modificando
    if (m_entry.id().isEmpty()) {
        m_entry = PasswordEntry::createNew(title, username, password, url, notes);
    } else {
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