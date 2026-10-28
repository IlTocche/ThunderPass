#ifndef PASSWORDDETAILSDIALOG_H
#define PASSWORDDETAILSDIALOG_H

#include <memory>
#include <QDialog>
#include "passwordentry.h"

QT_BEGIN_NAMESPACE
namespace Ui { class PasswordDetailsDialog; }
QT_END_NAMESPACE

class PasswordDetailsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PasswordDetailsDialog(const PasswordEntry &entry, QWidget *parent = nullptr);
    ~PasswordDetailsDialog();

private slots:
    void onCopyUsername();
    void onCopyPassword();
    void onTogglePasswordVisibility();
    void onCloseClicked();

private:
    std::unique_ptr<Ui::PasswordDetailsDialog> ui;
    PasswordEntry m_entry;
    bool m_passwordVisible = false;
};

#endif // PASSWORDDETAILSDIALOG_H