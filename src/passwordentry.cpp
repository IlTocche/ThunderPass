#include "passwordentry.h"

PasswordEntry PasswordEntry::createNew(const QString &title,
                                       const QString &username,
                                       const QString &password,
                                       const QString &url,
                                       const QString &notes)
{
    PasswordEntry entry;
    entry.m_id = QUuid::createUuid().toString(QUuid::WithoutBraces);
    entry.m_title = title;
    entry.m_username = username;
    entry.m_password = password;
    entry.m_url = url;
    entry.m_notes = notes;
    return entry;
}

QJsonObject PasswordEntry::toJson() const
{
    QJsonObject obj;
    obj["id"] = m_id;
    obj["title"] = m_title;
    obj["username"] = m_username;
    obj["password"] = m_password;
    obj["url"] = m_url;
    obj["notes"] = m_notes;
    return obj;
}

PasswordEntry PasswordEntry::fromJson(const QJsonObject &json)
{
    PasswordEntry entry;
    entry.m_id = json["id"].toString();
    entry.m_title = json["title"].toString();
    entry.m_username = json["username"].toString();
    entry.m_password = json["password"].toString();
    entry.m_url = json["url"].toString();
    entry.m_notes = json["notes"].toString();
    return entry;
}