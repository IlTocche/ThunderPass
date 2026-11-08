#ifndef TOTPACCOUNT_H
#define TOTPACCOUNT_H

#include <QString>
#include <QJsonObject>
#include <QUuid>

class TotpAccount
{
public:
    TotpAccount() = default;

    static TotpAccount create(const QString &name, const QString &secret);

    QString id() const { return m_id; }
    QString name() const { return m_name; }
    QString secret() const { return m_secret; }

    void setName(const QString &name) { m_name = name; }
    void setSecret(const QString &secret) { m_secret = secret; }

    QJsonObject toJson() const;
    static TotpAccount fromJson(const QJsonObject &json);

private:
    QString m_id;
    QString m_name;
    QString m_secret; // Base32
};

#endif // TOTPACCOUNT_H