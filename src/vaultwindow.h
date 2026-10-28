#ifndef VAULTWINDOW_H
#define VAULTWINDOW_H

#include <memory>
#include <QMainWindow>

#include "vaultmanager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class VaultWindow; }
QT_END_NAMESPACE

class VaultWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit VaultWindow(const QString &userEmail, VaultManager *vault, QWidget *parent = nullptr);
    ~VaultWindow();

private slots:
    void onAddPasswordClicked();
    void onLogoutClicked();

private:
    void refreshList();   // Ricarica la lista delle password

    std::unique_ptr<Ui::VaultWindow> ui;
    QString m_userEmail;
    VaultManager *m_vault;   // Non è owner
};

#endif // VAULTWINDOW_H