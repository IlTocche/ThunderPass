#include "crypto.h"

#include <QDebug>
#include <sodium.h>

bool Crypto::initialize()
{
    // sodium_init() deve essere chiamato una sola volta all'avvio
    if (sodium_init() < 0) {
        qCritical() << "Errore: impossibile inizializzare libsodium";
        return false;
    }
    return true;
}

QByteArray Crypto::generateSalt()
{
    QByteArray salt(crypto_pwhash_SALTBYTES, 0);
    randombytes_buf(salt.data(), salt.size());
    return salt;
}

QString Crypto::hashPassword(const QString &password)
{
    // Buffer che conterrà l'hash completo (formato libsodium)
    char hashed[crypto_pwhash_STRBYTES];

    // Conversione QString → bytes
    QByteArray passwordBytes = password.toUtf8();

    // Argon2id con parametri interattivi (sicuri e ragionevolmente veloci)
    int result = crypto_pwhash_str(
        hashed,
        passwordBytes.constData(),
        passwordBytes.size(),
        crypto_pwhash_OPSLIMIT_INTERACTIVE,
        crypto_pwhash_MEMLIMIT_INTERACTIVE
        );

    if (result != 0) {
        qWarning() << "Errore durante l'hashing della password";
        return QString();
    }

    // Ritorniamo l'hash come stringa
    return QString::fromUtf8(hashed);
}

bool Crypto::verifyPassword(const QString &password, const QString &hashedPassword)
{
    QByteArray passwordBytes = password.toUtf8();
    QByteArray hashBytes = hashedPassword.toUtf8();

    // Confronta in modo sicuro (tempo costante)
    int result = crypto_pwhash_str_verify(
        hashBytes.constData(),
        passwordBytes.constData(),
        passwordBytes.size()
        );

    return result == 0;
}