#include "tokendata.h"
#include <QJsonDocument>
#include <QDateTime>

QJsonObject TokenInfo::toJson() const
{
    QJsonObject otp;
    otp["link"] = QJsonValue::Null;
    otp["label"] = account;
    otp["account"] = account;
    otp["issuer"] = issuer;
    otp["digits"] = digits;
    otp["period"] = period;
    otp["algorithm"] = algorithm;
    otp["counter"] = (tokenType == "HOTP") ? QJsonValue(static_cast<int>(hotpCounter)) : QJsonValue::Null;
    otp["tokenType"] = tokenType;
    otp["source"] = "Manual";

    QJsonObject order;
    order["position"] = orderPosition;

    QJsonObject badge;
    badge["color"] = badgeColor;

    QJsonObject label;
    label["text"] = name.left(2).toUpper();
    label["backgroundColor"] = badgeColor;

    QJsonObject iconCollection;
    if (!iconCollectionId.isEmpty()) {
        iconCollection["id"] = iconCollectionId;
    }

    QJsonObject icon;
    icon["selected"] = iconCollectionId.isEmpty() ? "Label" : "IconCollection";
    icon["brand"] = QJsonValue::Null;
    icon["label"] = label;
    icon["iconCollection"] = iconCollectionId.isEmpty() ? QJsonValue::Null : QJsonValue(iconCollection);

    QJsonObject service;
    service["name"] = name;
    service["secret"] = secret;
    service["updatedAt"] = QDateTime::currentMSecsSinceEpoch();
    service["type"] = QJsonValue::Null;
    service["serviceTypeID"] = QJsonValue::Null;
    service["otp"] = otp;
    service["order"] = order;
    service["badge"] = badge;
    service["icon"] = icon;
    service["groupId"] = groupId.isEmpty() ? QJsonValue::Null : QJsonValue(groupId);

    return service;
}

TokenInfo TokenInfo::fromJson(const QJsonObject &obj)
{
    TokenInfo info;
    info.name = obj["name"].toString();
    info.secret = obj["secret"].toString();

    QJsonObject otp = obj["otp"].toObject();
    info.issuer = otp["issuer"].toString();
    info.account = otp["account"].toString();
    info.digits = otp["digits"].toInt(6);
    info.period = otp["period"].toInt(30);
    info.algorithm = otp["algorithm"].toString("SHA1");
    info.tokenType = otp["tokenType"].toString("TOTP");
    info.hotpCounter = otp["counter"].toInt(0);

    QJsonObject order = obj["order"].toObject();
    info.orderPosition = order["position"].toInt(0);

    QJsonObject badge = obj["badge"].toObject();
    if (!badge.isEmpty())
        info.badgeColor = badge["color"].toString("#3B82F6");

    if (!obj["groupId"].isNull())
        info.groupId = obj["groupId"].toString();

    QJsonObject icon = obj["icon"].toObject();
    QJsonObject iconColl = icon["iconCollection"].toObject();
    if (!iconColl.isEmpty())
        info.iconCollectionId = iconColl["id"].toString();

    return info;
}

QJsonObject GroupInfo::toJson() const
{
    QJsonObject obj;
    obj["id"] = id;
    obj["name"] = name;
    obj["isExpanded"] = isExpanded;
    obj["updatedAt"] = QDateTime::currentMSecsSinceEpoch();
    return obj;
}

GroupInfo GroupInfo::fromJson(const QJsonObject &obj)
{
    GroupInfo info;
    info.id = obj["id"].toString();
    info.name = obj["name"].toString();
    info.isExpanded = obj["isExpanded"].toBool(true);
    return info;
}
