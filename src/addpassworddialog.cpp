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

    m_entry = PasswordEntry::createNew(title, username, password, url, notes);
    accept();
}

void AddPasswordDialog::onCancelClicked()
{
    reject();
}