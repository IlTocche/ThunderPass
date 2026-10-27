#include "registerdialog.h"
#include "ui_registerdialog.h"

#include <QMessageBox>

RegisterDialog::RegisterDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::RegisterDialog)
{
    ui->setupUi(this);

    setWindowTitle("ThunderPass - Crea account");
    setFixedSize(420, 560);
    setModal(true);


    connect(ui->registerButton, &QPushButton::clicked,
            this, &RegisterDialog::onRegisterClicked);

    connect(ui->backToLoginButton, &QPushButton::clicked,
            this, &RegisterDialog::onBackToLoginClicked);
}

RegisterDialog::~RegisterDialog()
{
    delete ui;
}

void RegisterDialog::onRegisterClicked()
{
    QString email = ui->emailEdit->text().trimmed();
    QString password = ui->passwordEdit->text();
    QString confirm = ui->confirmPasswordEdit->text();


    if (email.isEmpty() || password.isEmpty() || confirm.isEmpty()) {
        QMessageBox::warning(this, "Attenzione", "Compila tutti i campi.");
        return;
    }

    if (password != confirm) {
        QMessageBox::warning(this, "Attenzione", "Le password non coincidono.");
        return;
    }

    if (password.length() < 8) {
        QMessageBox::warning(this, "Attenzione", "La password deve avere almeno 8 caratteri.");
        return;
    }

    // Per ora solo messaggio (più avanti salveremo l'account)
    QMessageBox::information(this, "Successo",
                             "Account creato con successo!\n(funzionalità di salvataggio in arrivo)");
    accept(); // Chiude la dialog
}

void RegisterDialog::onBackToLoginClicked()
{
    reject(); // Chiude la dialog e torna al login
}
