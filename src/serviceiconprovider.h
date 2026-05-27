#pragma once

#include <QObject>
#include <QString>
#include <QStringList>
#include <QJsonArray>
#include <QHash>

struct ServiceMatchRule {
    QString text;
    QString field;
    QString matcher;
    bool ignoreCase = true;
};

struct ServiceEntry {
    QString id;
    QString name;
    QStringList issuers;
    QStringList tags;
    QList<ServiceMatchRule> matchRules;
    QString iconCollectionId;
    QString iconLightId;   // UUID of the light PNG
    QString iconDarkId;    // UUID of the dark PNG
};

class ServiceIconProvider : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool loaded READ isLoaded NOTIFY loadedChanged)

public:
    explicit ServiceIconProvider(QObject *parent = nullptr);

    bool load(const QString &servicesJsonPath);
    bool isLoaded() const { return !m_services.isEmpty(); }

    Q_INVOKABLE QString matchByName(const QString &name) const;
    Q_INVOKABLE QString iconPathForCollection(const QString &collectionId) const;
    Q_INVOKABLE QString labelFromName(const QString &name) const;
    Q_INVOKABLE QStringList getIconCollectionIds() const;
    Q_INVOKABLE QString getIconCollectionName(const QString &collectionId) const;

    const ServiceEntry* findService(const QString &collectionId) const;
    QString iconPath(const QString &iconId) const;

signals:
    void loadedChanged();

private:
    QList<ServiceEntry> m_services;
};
