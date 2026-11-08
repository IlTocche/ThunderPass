#include "authenticatorpage.h"
#include "ui_authenticatorpage.h"
#include "totp.h"

#include <QMessageBox>
#include <QInputDialog>
#include <QClipboard>
#include <QGuiApplication>

AuthenticatorPage::AuthenticatorPage(VaultManager *vault, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AuthenticatorPage)
    , m_vault(vault)
{
    ui->setupUi(this);

    connect(ui->addButton, &QPushButton::clicked,
            this, &AuthenticatorPage::onAddClicked);
    connect(ui->deleteButton, &QPushButton::clicked,
            this, &AuthenticatorPage::onDeleteClicked);
    connect(ui->copyButton, &QPushButton::clicked,
            this, &AuthenticatorPage::onCopyClicked);
    connect(ui->accountList, &QListWidget::itemSelectionChanged,
            this, &AuthenticatorPage::onSelectionChanged);

    connect(&m_timer, &QTimer::timeout, this, &AuthenticatorPage::onTick);
    m_timer.start(1000);

    ui->codeLabel->setText("------");
    ui->timerLabel->setText("");
    ui->copyButton->setEnabled(false);
    ui->deleteButton->setEnabled(false);

    refreshList();
}

AuthenticatorPage::~AuthenticatorPage()
{
}

void AuthenticatorPage::refreshList()
{
    ui->accountList->clear();
    if (!m_vault) return;

    const auto accounts = m_vault->totpAccounts();
    for (const TotpAccount &acc : accounts) {
        QListWidgetItem *item = new QListWidgetItem(acc.name());
        item->setData(Qt::UserRole, acc.id());
        ui->accountList->addItem(item);
    }
}

void AuthenticatorPage::onAddClicked()
{
    if (!m_vault) return;

    bool ok = false;
    QString name = QInputDialog::getText(
        this, "Nuovo account 2FA",
        "Nome (es. Google, GitHub, Discord):",
        QLineEdit::Normal, "", &ok);
    if (!ok || name.trimmed().isEmpty()) return;

    QString secret = QInputDialog::getText(
        this, "Nuovo account 2FA",
        "Secret Base32 (quello che ti dà il sito):",
        QLineEdit::Normal, "", &ok);
    if (!ok || secret.trimmed().isEmpty()) return;

    if (!Totp::isValidSecret(secret)) {
        QMessageBox::warning(this, "Secret non valido",
                             "Il secret non sembra un codice Base32 valido.");
        return;
    }

    TotpAccount acc = TotpAccount::create(name, secret);
    if (m_vault->addTotpAccount(acc)) {
        refreshList();
    } else {
        QMessageBox::critical(this, "Errore", "Impossibile salvare l'account 2FA.");
    }
}

void AuthenticatorPage::onDeleteClicked()
{
    if (m_selectedId.isEmpty() || !m_vault) return;

    auto reply = QMessageBox::question(
        this, "Elimina account",
        "Vuoi eliminare questo account 2FA?",
        QMessageBox::Yes | QMessageBox::No);

    if (reply != QMessageBox::Yes) return;

    if (m_vault->removeTotpAccount(m_selectedId)) {
        m_selectedId.clear();
        ui->codeLabel->setText("------");
        ui->timerLabel->setText("");
        ui->copyButton->setEnabled(false);
        ui->deleteButton->setEnabled(false);
        refreshList();
    }
}

void AuthenticatorPage::onCopyClicked()
{
    QString code = ui->codeLabel->text();
    if (code == "------" || code.isEmpty()) return;
    QGuiApplication::clipboard()->setText(code);
}

void AuthenticatorPage::onSelectionChanged()
{
    QList<QListWidgetItem*> selected = ui->accountList->selectedItems();
    if (selected.isEmpty()) {
        m_selectedId.clear();
        ui->codeLabel->setText("------");
        ui->timerLabel->setText("");
        ui->copyButton->setEnabled(false);
        ui->deleteButton->setEnabled(false);
        return;
    }

    m_selectedId = selected.first()->data(Qt::UserRole).toString();
    ui->copyButton->setEnabled(true);
    ui->deleteButton->setEnabled(true);
    updateCurrentCode();
}

void AuthenticatorPage::onTick()
{
    updateCurrentCode();
}

void AuthenticatorPage::updateCurrentCode()
{
    if (m_selectedId.isEmpty() || !m_vault) return;

    const auto accounts = m_vault->totpAccounts();
    for (const TotpAccount &acc : accounts) {
        if (acc.id() == m_selectedId) {
            QString code = Totp::generateCode(acc.secret());
            int remaining = Totp::remainingSeconds();

            ui->codeLabel->setText(code);
            ui->timerLabel->setText(QString("Scade tra %1 s").arg(remaining));

            if (remaining <= 5) {
                ui->timerLabel->setStyleSheet("color: #ff453a; font-size: 13px;");
            } else {
                ui->timerLabel->setStyleSheet("color: #98989f; font-size: 13px;");
            }
            return;
        }
    }
}