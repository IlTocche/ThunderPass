#include "totpaccount.h"

TotpAccount TotpAccount::create(const QString &name, const QString &secret)
{
    TotpAccount acc;
    acc.m_id = QUuid::createUuid().toString(QUuid::WithoutBraces);
    acc.m_name = name.trimmed();
    acc.m_secret = secret.trimmed().toUpper().remove(' ');
    return acc;
}

QJsonObject TotpAccount::toJson() const
{
    QJsonObject o;
    o["id"] = m_id;
    o["name"] = m_name;
    o["secret"] = m_secret;
    return o;
}

TotpAccount TotpAccount::fromJson(const QJsonObject &json)
{
    TotpAccount acc;
    acc.m_id = json["id"].toString();
    acc.m_name = json["name"].toString();
    acc.m_secret = json["secret"].toString();
    return acc;
}