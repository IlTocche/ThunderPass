#ifndef AUTHENTICATORPAGE_H
#define AUTHENTICATORPAGE_H

#include <memory>
#include <QWidget>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class AuthenticatorPage; }
QT_END_NAMESPACE

class AuthenticatorPage : public QWidget
{
    Q_OBJECT

public:
    explicit AuthenticatorPage(QWidget *parent = nullptr);
    ~AuthenticatorPage();

private slots:
    void onAddSecretClicked();
    void onUpdateCodes();          // aggiorna i codici ogni secondo
    void onCopyCodeClicked();

private:
    void refreshCode();

    std::unique_ptr<Ui::AuthenticatorPage> ui;
    QTimer m_timer;
    QString m_currentSecret;
};

#endif // AUTHENTICATORPAGE_H