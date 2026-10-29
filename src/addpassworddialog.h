#ifndef ADDPASSWORDDIALOG_H
#define ADDPASSWORDDIALOG_H

#include <memory>
#include <QDialog>
#include "passwordentry.h"

QT_BEGIN_NAMESPACE
namespace Ui { class AddPasswordDialog; }
QT_END_NAMESPACE

class AddPasswordDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddPasswordDialog(QWidget *parent = nullptr);
    explicit AddPasswordDialog(const PasswordEntry &entryToEdit, QWidget *parent = nullptr);
    ~AddPasswordDialog();

    // Ritorna la password creata (chiamare solo se l'utente ha premuto Salva)
    PasswordEntry resultEntry() const;

private slots:
    void onSaveClicked();
    void onCancelClicked();

private:
    std::unique_ptr<Ui::AddPasswordDialog> ui;
    PasswordEntry m_entry;
};

#endif // ADDPASSWORDDIALOG_H