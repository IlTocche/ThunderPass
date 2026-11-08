#ifndef AUTHENTICATORPAGE_H
#define AUTHENTICATORPAGE_H

#include <memory>
#include <QWidget>
#include <QTimer>

#include "vaultmanager.h"
#include "totpaccount.h"

QT_BEGIN_NAMESPACE
namespace Ui { class AuthenticatorPage; }
QT_END_NAMESPACE

class AuthenticatorPage : public QWidget
{
    Q_OBJECT

public:
    explicit AuthenticatorPage(VaultManager *vault, QWidget *parent = nullptr);
    ~AuthenticatorPage();

private slots:
    void onAddClicked();
    void onDeleteClicked();
    void onCopyClicked();
    void onSelectionChanged();
    void onTick();

private:
    void refreshList();
    void updateCurrentCode();

    std::unique_ptr<Ui::AuthenticatorPage> ui;
    VaultManager *m_vault = nullptr;
    QTimer m_timer;
    QString m_selectedId;
};

#endif // AUTHENTICATORPAGE_H