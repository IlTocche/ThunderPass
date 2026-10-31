#include "authenticatorpage.h"
#include "ui_authenticatorpage.h"
#include "totp.h"

#include <QMessageBox>
#include <QInputDialog>
#include <QClipboard>
#include <QGuiApplication>

AuthenticatorPage::AuthenticatorPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AuthenticatorPage)
{
    ui->setupUi(this);

    connect(ui->addSecretButton, &QPushButton::clicked,
            this, &AuthenticatorPage::onAddSecretClicked);

    connect(ui->copyButton, &QPushButton::clicked,
            this, &AuthenticatorPage::onCopyCodeClicked);

    // Aggiorna il codice ogni secondo
    connect(&m_timer, &QTimer::timeout, this, &AuthenticatorPage::onUpdateCodes);
    m_timer.start(1000);

    ui->codeLabel->setText("------");
    ui->timerLabel->setText("");
    ui->copyButton->setEnabled(false);
}

AuthenticatorPage::~AuthenticatorPage()
{
}

void AuthenticatorPage::onAddSecretClicked()
{
    bool ok;
    QString secret = QInputDialog::getText(
        this,
        "Aggiungi account 2FA",
        "Incolla il secret Base32\n(quello che ti dà il sito quando attivi il 2FA):",
        QLineEdit::Normal,
        "",
        &ok
        );

    if (!ok || secret.trimmed().isEmpty()) return;

    if (!Totp::isValidSecret(secret)) {
        QMessageBox::warning(this, "Secret non valido",
                             "Il secret non sembra un codice Base32 valido.");
        return;
    }

    m_currentSecret = secret.trimmed().toUpper().remove(' ');
    ui->copyButton->setEnabled(true);
    refreshCode();
}

void AuthenticatorPage::onUpdateCodes()
{
    if (m_currentSecret.isEmpty()) return;
    refreshCode();
}

void AuthenticatorPage::refreshCode()
{
    QString code = Totp::generateCode(m_currentSecret);
    int remaining = Totp::remainingSeconds();

    ui->codeLabel->setText(code);
    ui->timerLabel->setText(QString("Scade tra %1 secondi").arg(remaining));

    // Colore del timer (rosso quando sta per scadere)
    if (remaining <= 5) {
        ui->timerLabel->setStyleSheet("color: #ff453a; font-size: 13px;");
    } else {
        ui->timerLabel->setStyleSheet("color: #8e8e93; font-size: 13px;");
    }
}

void AuthenticatorPage::onCopyCodeClicked()
{
    if (ui->codeLabel->text() == "------") return;
    QGuiApplication::clipboard()->setText(ui->codeLabel->text());
}