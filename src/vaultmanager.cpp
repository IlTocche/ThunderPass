#include "vaultmanager.h"
#include "crypto.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>
#include <sodium.h>

VaultManager::VaultManager(QObject *parent)
    : QObject(parent)
{
}

QString VaultManager::vaultFilePath(const QString &email) const
{
    QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(path);
    return path + "/vault_" + email + ".dat";
}

bool VaultManager::deriveKey(const QString &password, const QByteArray &salt, QByteArray &outKey)
{
    outKey.resize(crypto_secretbox_KEYBYTES);

    QByteArray passwordBytes = password.toUtf8();

    int result = crypto_pwhash(
        reinterpret_cast<unsigned char*>(outKey.data()),
        outKey.size(),
        passwordBytes.constData(),
        passwordBytes.size(),
        reinterpret_cast<const unsigned char*>(salt.constData()),
        crypto_pwhash_OPSLIMIT_INTERACTIVE,
        crypto_pwhash_MEMLIMIT_INTERACTIVE,
        crypto_pwhash_ALG_ARGON2ID13
        );

    return result == 0;
}

QByteArray VaultManager::encrypt(const QByteArray &plaintext)
{
    QByteArray nonce(crypto_secretbox_NONCEBYTES, 0);
    randombytes_buf(nonce.data(), nonce.size());

    QByteArray ciphertext(plaintext.size() + crypto_secretbox_MACBYTES, 0);

    int result = crypto_secretbox_easy(
        reinterpret_cast<unsigned char*>(ciphertext.data()),
        reinterpret_cast<const unsigned char*>(plaintext.constData()),
        plaintext.size(),
        reinterpret_cast<const unsigned char*>(nonce.constData()),
        reinterpret_cast<const unsigned char*>(m_key.constData())
        );

    if (result != 0) {
        return QByteArray();
    }

    return nonce + ciphertext;
}

QByteArray VaultManager::decrypt(const QByteArray &data)
{
    if (data.size() < crypto_secretbox_NONCEBYTES + crypto_secretbox_MACBYTES) {
        return QByteArray();
    }

    QByteArray nonce = data.left(crypto_secretbox_NONCEBYTES);
    QByteArray ciphertext = data.mid(crypto_secretbox_NONCEBYTES);

    QByteArray plaintext(ciphertext.size() - crypto_secretbox_MACBYTES, 0);

    int result = crypto_secretbox_open_easy(
        reinterpret_cast<unsigned char*>(plaintext.data()),
        reinterpret_cast<const unsigned char*>(ciphertext.constData()),
        ciphertext.size(),
        reinterpret_cast<const unsigned char*>(nonce.constData()),
        reinterpret_cast<const unsigned char*>(m_key.constData())
        );

    if (result != 0) {
        return QByteArray();
    }

    return plaintext;
}

bool VaultManager::unlock(const QString &email, const QString &masterPassword)
{
    m_email = email.toLower().trimmed();
    QString path = vaultFilePath(m_email);

    QFile file(path);
    if (!file.exists()) {
        // Prima volta → vault vuoto
        m_salt = Crypto::generateSalt();
        if (!deriveKey(masterPassword, m_salt, m_key)) {
            return false;
        }
        m_entries.clear();
        m_totpAccounts.clear();
        m_unlocked = true;
        return saveVault();
    }

    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    QByteArray fileData = file.readAll();
    file.close();

    if (fileData.size() < crypto_pwhash_SALTBYTES) {
        return false;
    }

    m_salt = fileData.left(crypto_pwhash_SALTBYTES);
    QByteArray encrypted = fileData.mid(crypto_pwhash_SALTBYTES);

    if (!deriveKey(masterPassword, m_salt, m_key)) {
        return false;
    }

    QByteArray decrypted = decrypt(encrypted);
    if (decrypted.isEmpty()) {
        m_key.clear();
        return false;
    }

    QJsonDocument doc = QJsonDocument::fromJson(decrypted);
    m_entries.clear();
    m_totpAccounts.clear();

    if (doc.isArray()) {
        // Formato vecchio (solo password) → compatibilità
        QJsonArray array = doc.array();
        for (const QJsonValue &val : array) {
            m_entries.append(PasswordEntry::fromJson(val.toObject()));
        }
    } else if (doc.isObject()) {
        QJsonObject root = doc.object();

        QJsonArray passwords = root["passwords"].toArray();
        for (const QJsonValue &val : passwords) {
            m_entries.append(PasswordEntry::fromJson(val.toObject()));
        }

        QJsonArray totp = root["totp"].toArray();
        for (const QJsonValue &val : totp) {
            m_totpAccounts.append(TotpAccount::fromJson(val.toObject()));
        }
    } else {
        m_key.clear();
        return false;
    }

    m_unlocked = true;
    return true;
}

void VaultManager::lock()
{
    if (!m_key.isEmpty()) {
        sodium_memzero(m_key.data(), m_key.size());
        m_key.clear();
    }
    m_entries.clear();
    m_totpAccounts.clear();
    m_unlocked = false;
}

bool VaultManager::addEntry(const PasswordEntry &entry)
{
    if (!m_unlocked) return false;
    m_entries.append(entry);
    return saveVault();
}

bool VaultManager::removeEntry(const QString &id)
{
    if (!m_unlocked) return false;

    for (int i = 0; i < m_entries.size(); ++i) {
        if (m_entries[i].id() == id) {
            m_entries.removeAt(i);
            return saveVault();
        }
    }
    return false;
}

bool VaultManager::updateEntry(const PasswordEntry &entry)
{
    if (!m_unlocked) return false;

    for (int i = 0; i < m_entries.size(); ++i) {
        if (m_entries[i].id() == entry.id()) {
            m_entries[i] = entry;
            return saveVault();
        }
    }
    return false;
}

QVector<PasswordEntry> VaultManager::entries() const
{
    return m_entries;
}

bool VaultManager::isUnlocked() const
{
    return m_unlocked;
}

bool VaultManager::saveVault()
{
    if (!m_unlocked) return false;

    QJsonArray passwordsArray;
    for (const PasswordEntry &entry : m_entries) {
        passwordsArray.append(entry.toJson());
    }

    QJsonArray totpArray;
    for (const TotpAccount &acc : m_totpAccounts) {
        totpArray.append(acc.toJson());
    }

    QJsonObject root;
    root["passwords"] = passwordsArray;
    root["totp"] = totpArray;

    QByteArray plaintext = QJsonDocument(root).toJson(QJsonDocument::Compact);
    QByteArray encrypted = encrypt(plaintext);

    if (encrypted.isEmpty()) {
        return false;
    }

    QFile file(vaultFilePath(m_email));
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }

    file.write(m_salt);
    file.write(encrypted);
    file.close();
    return true;
}

bool VaultManager::loadVault()
{
    return true;
}

// ============================================================
// TOTP Accounts
// ============================================================

QVector<TotpAccount> VaultManager::totpAccounts() const
{
    return m_totpAccounts;
}

bool VaultManager::addTotpAccount(const TotpAccount &account)
{
    if (!m_unlocked) return false;
    m_totpAccounts.append(account);
    return saveVault();
}

bool VaultManager::removeTotpAccount(const QString &id)
{
    if (!m_unlocked) return false;

    for (int i = 0; i < m_totpAccounts.size(); ++i) {
        if (m_totpAccounts[i].id() == id) {
            m_totpAccounts.removeAt(i);
            return saveVault();
        }
    }
    return false;
}

bool VaultManager::updateTotpAccount(const TotpAccount &account)
{
    if (!m_unlocked) return false;

    for (int i = 0; i < m_totpAccounts.size(); ++i) {
        if (m_totpAccounts[i].id() == account.id()) {
            m_totpAccounts[i] = account;
            return saveVault();
        }
    }
    return false;
}