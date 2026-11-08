#include "settingspage.h"
#include "ui_settingspage.h"

#include <QSettings>
#include <QMessageBox>
#include <QInputDialog>

SettingsPage::SettingsPage(AuthManager *auth,
                           VaultManager *vault,
                           const QString &email,
                           QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SettingsPage)
    , m_auth(auth)
    , m_vault(vault)
    , m_email(email)
{
    ui->setupUi(this);

    ui->accountEmailLabel->setText(m_email);

    loadSettings();

    connect(ui->themeCombo, &QComboBox::currentIndexChanged,
            this, &SettingsPage::onThemeChanged);

    connect(ui->skipLoginCheck, &QCheckBox::toggled,
            this, &SettingsPage::onSkipLoginToggled);

    connect(ui->changePasswordButton, &QPushButton::clicked,
            this, &SettingsPage::onChangePasswordClicked);
}

SettingsPage::~SettingsPage()
{
}

void SettingsPage::loadSettings()
{
    QSettings settings("ThunderPass", "ThunderPass");

    bool dark = settings.value("ui/darkTheme", true).toBool();
    ui->themeCombo->setCurrentIndex(dark ? 0 : 1);

    bool skip = settings.value("security/skipLogin", false).toBool();
    ui->skipLoginCheck->setChecked(skip);
}

void SettingsPage::saveSettings()
{
    QSettings settings("ThunderPass", "ThunderPass");
    settings.setValue("ui/darkTheme", ui->themeCombo->currentIndex() == 0);
    settings.setValue("security/skipLogin", ui->skipLoginCheck->isChecked());
}

void SettingsPage::onThemeChanged()
{
    bool dark = (ui->themeCombo->currentIndex() == 0);
    saveSettings();
    emit themeChanged(dark);
}

void SettingsPage::onSkipLoginToggled(bool checked)
{
    if (checked) {
        auto reply = QMessageBox::warning(
            this,
            "Attenzione – sicurezza ridotta",
            "Se attivi questa opzione, chiunque apra ThunderPass su questo computer "
            "potrà vedere le tue password senza inserire la master password.\n\n"
            "Vuoi continuare?",
            QMessageBox::Yes | QMessageBox::No
            );

        if (reply != QMessageBox::Yes) {
            ui->skipLoginCheck->blockSignals(true);
            ui->skipLoginCheck->setChecked(false);
            ui->skipLoginCheck->blockSignals(false);
            return;
        }
    }

    saveSettings();
    emit skipLoginChanged(ui->skipLoginCheck->isChecked());
}

void SettingsPage::onChangePasswordClicked()
{
    QMessageBox::information(
        this,
        "Cambia password",
        "La modifica della master password arriverà nel prossimo passo.\n"
        "(Serve re-cifrare tutto il vault in modo sicuro)"
        );
}