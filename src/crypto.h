#ifndef CRYPTO_H
#define CRYPTO_H

#include <QString>
#include <QByteArray>

/**
 * @brief Classe di utilità per operazioni crittografiche sicure
 *        usando libsodium (Argon2id, random, ecc.)
 */
class Crypto
{
public:
    // Genera un salt casuale sicuro (16 byte)
    static QByteArray generateSalt();

    // Crea l'hash di una password usando Argon2id
    // Ritorna l'hash completo (comprende anche salt e parametri)
    static QString hashPassword(const QString &password);

    // Verifica se la password in chiaro corrisponde all'hash salvato
    static bool verifyPassword(const QString &password, const QString &hashedPassword);

    // Controlla se libsodium è stato inizializzato correttamente
    static bool initialize();
};

#endif // CRYPTO_H