#ifndef REGISTERDIALOG_H
#define REGISTERDIALOG_H

#include <memory>
#include <QDialog>

#include "authmanager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class RegisterDialog; }
QT_END_NAMESPACE

class RegisterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RegisterDialog(AuthManager *authManager, QWidget *parent = nullptr);
    ~RegisterDialog();

private slots:
    void onRegisterClicked();
    void onBackToLoginClicked();

private:
    std::unique_ptr<Ui::RegisterDialog> ui;
    AuthManager *m_auth;
};

#endif // REGISTERDIALOG_H