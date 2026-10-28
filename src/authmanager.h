#ifndef AUTHMANAGER_H
#define AUTHMANAGER_H

#include <QObject>
#include <QString>
#include <QMap>

/**
 * @brief Gestisce registrazione e login in modo sicuro
 */
class AuthManager : public QObject
{
    Q_OBJECT

public:
    explicit AuthManager(QObject *parent = nullptr);

    // Registra un nuovo utente
    // Ritorna true se ha successo
    bool registerUser(const QString &email, const QString &password);

    // Tenta il login
    // Ritorna true se email e password sono corretti
    bool login(const QString &email, const QString &password);

    // Controlla se esiste già un account registrato
    bool hasRegisteredUser() const;

    // Ritorna l'email dell'utente attualmente loggato (vuota se nessuno)
    QString currentUserEmail() const;

private:
    // Percorso del file dove salviamo i dati utente
    QString usersFilePath() const;

    // Carica i dati dal file
    bool loadUsers();

    // Salva i dati sul file
    bool saveUsers();

    // Struttura semplice: email → hash della password
    QMap<QString, QString> m_users;

    // Email dell'utente loggato in questo momento
    QString m_currentUser;
};

#endif // AUTHMANAGER_H