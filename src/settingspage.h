#ifndef SETTINGSPAGE_H
#define SETTINGSPAGE_H

#include <memory>
#include <QWidget>

#include "authmanager.h"
#include "vaultmanager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class SettingsPage; }
QT_END_NAMESPACE

class SettingsPage : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsPage(AuthManager *auth,
                          VaultManager *vault,
                          const QString &email,
                          QWidget *parent = nullptr);
    ~SettingsPage();

signals:
    void themeChanged(bool dark);
    void skipLoginChanged(bool enabled);

private slots:
    void onThemeChanged();
    void onSkipLoginToggled(bool checked);
    void onChangePasswordClicked();

private:
    void loadSettings();
    void saveSettings();

    std::unique_ptr<Ui::SettingsPage> ui;
    AuthManager *m_auth = nullptr;
    VaultManager *m_vault = nullptr;
    QString m_email;
};

#endif // SETTINGSPAGE_H