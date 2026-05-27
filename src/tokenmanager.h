#pragma once

#include "tokendata.h"
#include <QObject>
#include <QString>
#include <QList>
#include <QJsonArray>
#include <QSqlDatabase>

class TokenManager;
class ServiceIconProvider;

class TokenManager : public QObject
{
    Q_OBJECT
public:
    explicit TokenManager(ServiceIconProvider *icons = nullptr, QObject *parent = nullptr);
    ~TokenManager();

    Q_INVOKABLE bool addToken(const QVariantMap &tokenData);
    Q_INVOKABLE bool removeToken(int index);
    Q_INVOKABLE bool updateToken(int index, const QVariantMap &tokenData);
    Q_INVOKABLE QVariantMap getToken(int index) const;
    Q_INVOKABLE void clearAllTokens();
    Q_INVOKABLE bool exportToFile(const QString &filePath);
    Q_INVOKABLE QString defaultExportPath() const;

    const QList<TokenInfo> &tokens() const { return m_tokens; }
    int count() const { return m_tokens.size(); }

    bool loadFromDatabase();
    bool importTokens(const QList<TokenInfo> &newTokens);

    QString defaultDbPath() const;

signals:
    void tokensChanged();
    void tokenUpdated(int index);

private:
    void resolveIcons(TokenInfo &token) const;
    bool initDatabase();
    bool saveToken(const TokenInfo &token);
    bool deleteToken(int id);
    bool updateTokenInDb(const TokenInfo &token);
    bool deleteAllTokens();

    QList<TokenInfo> m_tokens;
    QString m_dbPath;
    ServiceIconProvider *m_iconProvider;
};
