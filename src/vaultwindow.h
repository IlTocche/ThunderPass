#ifndef VAULTWINDOW_H
#define VAULTWINDOW_H

#include <memory>
#include <QMainWindow>
#include <QTimer>

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

protected:
    // Queste funzioni vengono chiamate automaticamente da Qt
    void mousePressEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void onAddPasswordClicked();
    void onLogoutClicked();
    void onInactivityTimeout();   // chiamato quando scade il timer

private:
    void refreshList();
    void resetInactivityTimer();  // resetta il timer a ogni attività

    std::unique_ptr<Ui::VaultWindow> ui;
    QString m_userEmail;
    VaultManager *m_vault;

    QTimer m_inactivityTimer;
    static const int INACTIVITY_TIMEOUT_MS = 5 * 60 * 1000; // 5 minuti
};

#endif // VAULTWINDOW_H