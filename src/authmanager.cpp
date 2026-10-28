#include "authmanager.h"
#include "crypto.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>

AuthManager::AuthManager(QObject *parent)
    : QObject(parent)
{
    // All'avvio carichiamo gli utenti già registrati
    loadUsers();
}

QString AuthManager::usersFilePath() const
{
    // Salviamo il file nella cartella AppData dell'utente
    // (su Windows: C:/Users/Nome/AppData/Roaming/ThunderPass/)
    QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(path); // crea la cartella se non esiste
    return path + "/users.json";
}

bool AuthManager::loadUsers()
{
    QFile file(usersFilePath());
    if (!file.exists()) {
        return true; // nessun utente ancora registrato → ok
    }

    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Impossibile aprire il file utenti";
        return false;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();

    if (!doc.isObject()) {
        return false;
    }

    QJsonObject obj = doc.object();
    m_users.clear();

    for (auto it = obj.begin(); it != obj.end(); ++it) {
        m_users.insert(it.key(), it.value().toString());
    }

    return true;
}

bool AuthManager::saveUsers()
{
    QJsonObject obj;
    for (auto it = m_users.constBegin(); it != m_users.constEnd(); ++it) {
        obj.insert(it.key(), it.value());
    }

    QFile file(usersFilePath());
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Impossibile salvare il file utenti";
        return false;
    }

    file.write(QJsonDocument(obj).toJson(QJsonDocument::Indented));
    file.close();
    return true;
}

bool AuthManager::registerUser(const QString &email, const QString &password)
{
    QString cleanEmail = email.trimmed().toLower();

    if (cleanEmail.isEmpty() || password.isEmpty()) {
        return false;
    }

    // Controlla se l'email è già registrata
    if (m_users.contains(cleanEmail)) {
        return false; // utente già esistente
    }

    // Creiamo l'hash sicuro della password
    QString hashed = Crypto::hashPassword(password);
    if (hashed.isEmpty()) {
        return false;
    }

    m_users.insert(cleanEmail, hashed);

    if (!saveUsers()) {
        m_users.remove(cleanEmail); // rollback
        return false;
    }

    return true;
}

bool AuthManager::login(const QString &email, const QString &password)
{
    QString cleanEmail = email.trimmed().toLower();

    if (!m_users.contains(cleanEmail)) {
        return false; // email non trovata
    }

    QString storedHash = m_users.value(cleanEmail);

    // Verifica la password in modo sicuro
    if (!Crypto::verifyPassword(password, storedHash)) {
        return false;
    }

    m_currentUser = cleanEmail;
    return true;
}

bool AuthManager::hasRegisteredUser() const
{
    return !m_users.isEmpty();
}

QString AuthManager::currentUserEmail() const
{
    return m_currentUser;
}