#include "registerdialog.h"
#include "ui_registerdialog.h"

#include <QMessageBox>

RegisterDialog::RegisterDialog(AuthManager *authManager, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::RegisterDialog)
    , m_auth(authManager)
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
        QMessageBox::warning(this, "Attenzione",
                             "La password deve avere almeno 8 caratteri.");
        return;
    }

    // Registrazione vera
    if (m_auth->registerUser(email, password)) {
        QMessageBox::information(this, "Successo",
                                 "Account creato con successo!\nOra puoi accedere.");
        accept();
    } else {
        QMessageBox::critical(this, "Errore",
                              "Impossibile creare l'account.\nL'email è già registrata o si è verificato un errore.");
    }
}

void RegisterDialog::onBackToLoginClicked()
{
    reject();
}