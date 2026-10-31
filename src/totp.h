#ifndef TOTP_H
#define TOTP_H

#include <QString>
#include <QByteArray>

/**
 * @brief Generatore di codici TOTP (Time-based One-Time Password)
 *        Compatibile con Google Authenticator, Authy, ecc.
 */
class Totp
{
public:
    // Genera il codice a 6 cifre a partire dal secret (Base32)
    // timeStep = 30 secondi (standard)
    static QString generateCode(const QString &base32Secret, int timeStep = 30, int digits = 6);

    // Secondi rimasti prima che il codice scada
    static int remainingSeconds(int timeStep = 30);

    // Valida se una stringa è un secret Base32 accettabile
    static bool isValidSecret(const QString &base32Secret);

private:
    static QByteArray base32Decode(const QString &base32);
    static quint64 currentTimeStep(int timeStep);
};

#endif // TOTP_H