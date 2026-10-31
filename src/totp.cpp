#include "totp.h"

#include <QDateTime>
#include <QMessageAuthenticationCode>
#include <QCryptographicHash>
#include <QtEndian>
#include <QRegularExpression>

quint64 Totp::currentTimeStep(int timeStep)
{
    return static_cast<quint64>(QDateTime::currentSecsSinceEpoch()) / static_cast<quint64>(timeStep);
}

int Totp::remainingSeconds(int timeStep)
{
    qint64 now = QDateTime::currentSecsSinceEpoch();
    return timeStep - static_cast<int>(now % timeStep);
}

bool Totp::isValidSecret(const QString &base32Secret)
{
    QString clean = base32Secret.trimmed().toUpper().remove(' ');
    // Base32: A-Z e 2-7
    static QRegularExpression re("^[A-Z2-7]+=*$");
    return clean.length() >= 16 && re.match(clean).hasMatch();
}

QByteArray Totp::base32Decode(const QString &base32)
{
    static const char *alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";
    QString clean = base32.trimmed().toUpper().remove(' ').remove('=');

    QByteArray result;
    int buffer = 0;
    int bitsLeft = 0;

    for (QChar c : clean) {
        const char *p = strchr(alphabet, c.toLatin1());
        if (!p) continue;

        buffer = (buffer << 5) | static_cast<int>(p - alphabet);
        bitsLeft += 5;

        if (bitsLeft >= 8) {
            result.append(static_cast<char>((buffer >> (bitsLeft - 8)) & 0xFF));
            bitsLeft -= 8;
        }
    }
    return result;
}

QString Totp::generateCode(const QString &base32Secret, int timeStep, int digits)
{
    QByteArray key = base32Decode(base32Secret);
    if (key.isEmpty()) return QString();

    quint64 counter = currentTimeStep(timeStep);

    // Counter in big-endian (8 byte)
    QByteArray counterBytes(8, 0);
    qToBigEndian(counter, reinterpret_cast<uchar*>(counterBytes.data()));

    // HMAC-SHA1
    QByteArray hash = QMessageAuthenticationCode::hash(
        counterBytes,
        key,
        QCryptographicHash::Sha1
        );

    // Dynamic truncation (RFC 4226)
    int offset = hash.at(hash.size() - 1) & 0x0F;
    quint32 binary =
        ((static_cast<quint8>(hash.at(offset)) & 0x7F) << 24) |
        ((static_cast<quint8>(hash.at(offset + 1)) & 0xFF) << 16) |
        ((static_cast<quint8>(hash.at(offset + 2)) & 0xFF) << 8) |
        (static_cast<quint8>(hash.at(offset + 3)) & 0xFF);

    quint32 otp = binary % static_cast<quint32>(qPow(10, digits));

    return QString("%1").arg(otp, digits, 10, QChar('0'));
}