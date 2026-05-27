#pragma once

#include <QString>
#include <QList>
#include <QJsonObject>
#include <QJsonArray>

struct TokenInfo {
    QString name;
    QString secret;
    QString issuer;
    QString account;
    int digits = 6;
    int period = 30;
    QString algorithm = "SHA1";
    QString tokenType = "TOTP";
    int hotpCounter = 0;
    QString badgeColor = "#3B82F6";
    QString iconCollectionId;
    QString groupId;
    int orderPosition = 0;
    int dbId = 0;

    QJsonObject toJson() const;
    static TokenInfo fromJson(const QJsonObject &obj);
};

struct GroupInfo {
    QString id;
    QString name;
    bool isExpanded = true;

    QJsonObject toJson() const;
    static GroupInfo fromJson(const QJsonObject &obj);
};
