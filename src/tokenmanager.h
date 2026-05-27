#pragma once

#include "tokendata.h"
#include "result.h"
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
    ~TokenManager() override;

    Q_INVOKABLE bool addToken(const QVariantMap &tokenData);
    Q_INVOKABLE bool removeToken(int index);
    Q_INVOKABLE bool updateToken(int index, const QVariantMap &tokenData);
    Q_INVOKABLE QVariantMap getToken(int index) const;
    Q_INVOKABLE void clearAllTokens();
    Q_INVOKABLE bool exportToFile(const QString &filePath);
    Q_INVOKABLE QString defaultExportPath() const;

    const QList<TokenInfo> &tokens() const { return m_tokens; }
    int count() const { return m_tokens.size(); }

    Result<bool> loadFromDatabase();
    Result<bool> importTokens(const QList<TokenInfo> &newTokens);

    QString defaultDbPath() const;

signals:
    void tokensChanged();
    void tokenUpdated(int index);

private:
    void resolveIcons(TokenInfo &token) const;
    Result<QSqlDatabase> initDatabase();
    Result<bool> saveToken(const TokenInfo &token);
    Result<bool> deleteToken(int id);
    Result<bool> updateTokenInDb(const TokenInfo &token);
    Result<bool> deleteAllTokens();

    QList<TokenInfo> m_tokens;
    QString m_dbPath;
    QString m_connectionName;
    ServiceIconProvider *m_iconProvider;
};
