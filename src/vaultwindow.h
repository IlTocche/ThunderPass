#ifndef VAULTWINDOW_H
#define VAULTWINDOW_H

#include <memory>
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class VaultWindow; }
QT_END_NAMESPACE

class VaultWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit VaultWindow(const QString &userEmail, QWidget *parent = nullptr);
    ~VaultWindow();

private slots:
    void onAddPasswordClicked();
    void onLogoutClicked();

private:
    std::unique_ptr<Ui::VaultWindow> ui;
    QString m_userEmail;
};

#endif // VAULTWINDOW_H