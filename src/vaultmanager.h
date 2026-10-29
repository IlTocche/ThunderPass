#ifndef VAULTMANAGER_H
#define VAULTMANAGER_H

#include <QObject>
#include <QVector>
#include <QString>
#include <QByteArray>

#include "passwordentry.h"

/**
 * @brief Gestisce il vault delle password in modo cifrato
 */
class VaultManager : public QObject
{
    Q_OBJECT

public:
    explicit VaultManager(QObject *parent = nullptr);

    // Sblocca il vault con la master password dell'utente
    // Deve essere chiamato dopo il login
    bool unlock(const QString &email, const QString &masterPassword);

    // Blocca il vault (cancella la chiave dalla memoria)
    void lock();

    // Aggiunge una nuova password
    bool addEntry(const PasswordEntry &entry);

    // Ritorna tutte le password (solo se sbloccato)
    QVector<PasswordEntry> entries() const;

    // Controlla se il vault è sbloccato
    bool isUnlocked() const;

    // Rimuove una password tramite il suo ID
    bool removeEntry(const QString &id);

    bool updateEntry(const PasswordEntry &entry);

private:
    // Percorso del file del vault
    QString vaultFilePath(const QString &email) const;

    // Deriva una chiave di 32 byte dalla master password
    bool deriveKey(const QString &password, const QByteArray &salt, QByteArray &outKey);

    // Cifra i dati
    QByteArray encrypt(const QByteArray &plaintext);

    // Decifra i dati
    QByteArray decrypt(const QByteArray &ciphertext);

    // Salva il vault su disco
    bool saveVault();

    // Carica il vault da disco
    bool loadVault();

    QVector<PasswordEntry> m_entries;
    QByteArray m_key;          // Chiave di cifratura (32 byte)
    QByteArray m_salt;         // Salt usato per derivare la chiave
    QString m_email;
    bool m_unlocked = false;
};

#endif // VAULTMANAGER_H