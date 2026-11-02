#ifndef VAULTWINDOW_H
#define VAULTWINDOW_H

#include <memory>
#include <QMainWindow>
#include <QTimer>

#include "vaultmanager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class VaultWindow; }
QT_END_NAMESPACE

class AuthenticatorPage;

class VaultWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit VaultWindow(const QString &userEmail, VaultManager *vault, QWidget *parent = nullptr);
    ~VaultWindow();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void onAddPasswordClicked();
    void onLogoutClicked();
    void onInactivityTimeout();
    void onSearchTextChanged(const QString &text);
    void onCustomContextMenu(const QPoint &pos);

    void onToggleMenuClicked();
    void onPasswordsNavClicked();
    void onAuthenticatorNavClicked();

private:
    void refreshList();
    void resetInactivityTimer();
    void setMenuVisible(bool visible);

    std::unique_ptr<Ui::VaultWindow> ui;
    QString m_userEmail;
    VaultManager *m_vault;

    QTimer m_inactivityTimer;
    static const int INACTIVITY_TIMEOUT_MS = 5 * 60 * 1000;

    AuthenticatorPage *m_authenticatorPage = nullptr;
    bool m_menuVisible = true;
};

#endif // VAULTWINDOW_H