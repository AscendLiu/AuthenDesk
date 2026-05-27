#include "serviceiconprovider.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDir>
#include <QDebug>
#include <QRegularExpression>

ServiceIconProvider::ServiceIconProvider(QObject *parent)
    : QObject(parent)
{
}

bool ServiceIconProvider::load(const QString &servicesJsonPath)
{
    QFile file(servicesJsonPath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Cannot open services.json:" << servicesJsonPath;
        return false;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonArray arr = doc.array();

    m_services.clear();
    for (const QJsonValue &val : arr) {
        QJsonObject obj = val.toObject();
        ServiceEntry entry;
        entry.id = obj["id"].toString();
        entry.name = obj["name"].toString();

        for (const QJsonValue &t : obj["tags"].toArray())
            entry.tags.append(t.toString().toLower());

        for (const QJsonValue &i : obj["issuers"].toArray())
            entry.issuers.append(i.toString());

        for (const QJsonValue &r : obj["match_rules"].toArray()) {
            QJsonObject robj = r.toObject();
            ServiceMatchRule mr;
            mr.text = robj["text"].toString();
            mr.field = robj["field"].toString();
            mr.matcher = robj["matcher"].toString();
            mr.ignoreCase = robj["ignore_case"].toBool(true);
            entry.matchRules.append(mr);
        }

        QJsonArray collections = obj["icons_collections"].toArray();
        if (!collections.isEmpty()) {
            QJsonObject col = collections[0].toObject();
            entry.iconCollectionId = col["id"].toString();
            QJsonArray icons = col["icons"].toArray();
            for (const QJsonValue &iv : icons) {
                QJsonObject ico = iv.toObject();
                QString type = ico["type"].toString();
                QString iconId = ico["id"].toString();
                if (type == "light" && entry.iconLightId.isEmpty())
                    entry.iconLightId = iconId;
                else if (type == "dark" && entry.iconDarkId.isEmpty())
                    entry.iconDarkId = iconId;
            }
            // Fallback: use any icon if light/dark not specified
            if (entry.iconLightId.isEmpty() && !icons.isEmpty())
                entry.iconLightId = icons[0].toObject()["id"].toString();
        }

        m_services.append(entry);
    }

    emit loadedChanged();
    return true;
}

QString ServiceIconProvider::matchByName(const QString &name) const
{
    QString n = name.trimmed().toLower();
    if (n.isEmpty()) return {};

    for (const ServiceEntry &entry : m_services) {
        if (entry.name.toLower() == n)
            return entry.iconCollectionId;

        for (const QString &tag : entry.tags) {
            if (tag == n)
                return entry.iconCollectionId;
        }

        for (const QString &issuer : entry.issuers) {
            if (issuer.toLower() == n)
                return entry.iconCollectionId;
        }
    }
    return {};
}

QString ServiceIconProvider::iconPathForCollection(const QString &collectionId) const
{
    for (const ServiceEntry &entry : m_services) {
        if (entry.iconCollectionId == collectionId) {
            QString iconId = entry.iconLightId.isEmpty() ? entry.iconDarkId : entry.iconLightId;
            return iconPath(iconId);
        }
    }
    return {};
}

QString ServiceIconProvider::iconPath(const QString &iconId) const
{
    if (iconId.isEmpty())
        return {};
    return "qrc:/icons/" + iconId + ".png";
}

QString ServiceIconProvider::labelFromName(const QString &name) const
{
    if (name.isEmpty()) return {};
    QStringList words = name.simplified().split(' ', Qt::SkipEmptyParts);
    QString result;
    if (words.size() >= 2) {
        result += words[0][0].toUpper();
        result += words[1][0].toUpper();
    } else {
        result = name.left(2).toUpper();
    }
    return result;
}

QStringList ServiceIconProvider::getIconCollectionIds() const
{
    QStringList ids;
    for (const ServiceEntry &e : m_services) {
        if (!e.iconCollectionId.isEmpty())
            ids.append(e.iconCollectionId);
    }
    return ids;
}

QString ServiceIconProvider::getIconCollectionName(const QString &collectionId) const
{
    for (const ServiceEntry &e : m_services) {
        if (e.iconCollectionId == collectionId)
            return e.name;
    }
    return {};
}

const ServiceEntry* ServiceIconProvider::findService(const QString &collectionId) const
{
    for (const ServiceEntry &e : m_services) {
        if (e.iconCollectionId == collectionId)
            return &e;
    }
    return nullptr;
}
