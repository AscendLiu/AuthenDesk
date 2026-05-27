#include "tokenmanager.h"
#include "serviceiconprovider.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>
#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <optional>

TokenManager::TokenManager(ServiceIconProvider *icons, QObject *parent)
    : QObject(parent)
    , m_iconProvider(icons)
    , m_connectionName(QString("authendesk_db_%1").arg(reinterpret_cast<quintptr>(this), 0, 16))
{
}

TokenManager::~TokenManager()
{
    m_tokens.clear();

    if (QSqlDatabase::contains(m_connectionName)) {
        QSqlDatabase db = QSqlDatabase::database(m_connectionName);
        if (db.isOpen()) {
            db.close();
        }
    }
}

QString TokenManager::defaultDbPath() const
{
    QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dataDir);
    return dataDir + "/tokens.db";
}

Result<QSqlDatabase> TokenManager::initDatabase()
{
    m_dbPath = defaultDbPath();
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", m_connectionName);
    db.setDatabaseName(m_dbPath);

    if (!db.open()) {
        QString error = QString("Failed to open database: %1").arg(db.lastError().text());
        qWarning() << error;
        return Result<QSqlDatabase>::fail(error);
    }

    QSqlQuery q(db);
    if (!q.exec("CREATE TABLE IF NOT EXISTS tokens ("
           "id INTEGER PRIMARY KEY AUTOINCREMENT,"
           "name TEXT NOT NULL,"
           "secret TEXT NOT NULL,"
           "issuer TEXT DEFAULT '',"
           "account TEXT DEFAULT '',"
           "digits INTEGER DEFAULT 6,"
           "period INTEGER DEFAULT 30,"
           "algorithm TEXT DEFAULT 'SHA1',"
           "token_type TEXT DEFAULT 'TOTP',"
           "hotp_counter INTEGER DEFAULT 0,"
           "badge_color TEXT DEFAULT '#3B82F6',"
           "icon_collection_id TEXT DEFAULT '',"
           "group_id TEXT DEFAULT '',"
           "order_position INTEGER DEFAULT 0,"
           "created_at INTEGER DEFAULT 0,"
           "updated_at INTEGER DEFAULT 0"
           ")")) {
        QString error = QString("Failed to create table: %1").arg(q.lastError().text());
        qWarning() << error;
        return Result<QSqlDatabase>::fail(error);
    }

    if (!q.exec("CREATE INDEX IF NOT EXISTS idx_secret ON tokens(secret)")) {
        qWarning() << "Failed to create index:" << q.lastError().text();
    }

    return Result<QSqlDatabase>::ok(db);
}

Result<bool> TokenManager::loadFromDatabase()
{
    auto dbResult = initDatabase();
    if (!dbResult) {
        return Result<bool>::fail(dbResult.error);
    }

    m_tokens.clear();
    QSqlDatabase db = dbResult.value.value();
    QSqlQuery q(db);
    if (!q.exec("SELECT * FROM tokens ORDER BY order_position")) {
        QString error = QString("Failed to load tokens: %1").arg(q.lastError().text());
        qWarning() << error;
        return Result<bool>::fail(error);
    }

    while (q.next()) {
        TokenInfo info;
        info.dbId = q.value("id").toInt();
        info.name = q.value("name").toString();
        info.secret = q.value("secret").toString();
        info.issuer = q.value("issuer").toString();
        info.account = q.value("account").toString();
        info.digits = q.value("digits").toInt();
        info.period = q.value("period").toInt();
        info.algorithm = q.value("algorithm").toString();
        info.tokenType = q.value("token_type").toString();
        info.hotpCounter = q.value("hotp_counter").toInt();
        info.badgeColor = q.value("badge_color").toString();
        info.iconCollectionId = q.value("icon_collection_id").toString();
        info.groupId = q.value("group_id").toString();
        info.orderPosition = q.value("order_position").toInt();

        resolveIcons(info);
        m_tokens.append(info);
    }

    emit tokensChanged();
    return Result<bool>::ok(true);
}

Result<bool> TokenManager::saveToken(const TokenInfo &token)
{
    QSqlDatabase db = QSqlDatabase::database(m_connectionName);
    QSqlQuery q(db);
    q.prepare("INSERT INTO tokens (name, secret, issuer, account, digits, period, "
              "algorithm, token_type, hotp_counter, badge_color, icon_collection_id, "
              "group_id, order_position, created_at, updated_at) "
              "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
    q.addBindValue(token.name);
    q.addBindValue(token.secret);
    q.addBindValue(token.issuer);
    q.addBindValue(token.account);
    q.addBindValue(token.digits);
    q.addBindValue(token.period);
    q.addBindValue(token.algorithm);
    q.addBindValue(token.tokenType);
    q.addBindValue(token.hotpCounter);
    q.addBindValue(token.badgeColor);
    q.addBindValue(token.iconCollectionId);
    q.addBindValue(token.groupId);
    q.addBindValue(token.orderPosition);

    qint64 now = QDateTime::currentMSecsSinceEpoch();
    q.addBindValue(now);
    q.addBindValue(now);

    if (!q.exec()) {
        QString error = QString("Failed to save token: %1").arg(q.lastError().text());
        qWarning() << error;
        return Result<bool>::fail(error);
    }
    return Result<bool>::ok(true);
}

Result<bool> TokenManager::deleteToken(int dbId)
{
    QSqlDatabase db = QSqlDatabase::database(m_connectionName);
    QSqlQuery q(db);
    q.prepare("DELETE FROM tokens WHERE id = ?");
    q.addBindValue(dbId);
    if (!q.exec()) {
        QString error = QString("Failed to delete token: %1").arg(q.lastError().text());
        qWarning() << error;
        return Result<bool>::fail(error);
    }
    return Result<bool>::ok(true);
}

Result<bool> TokenManager::updateTokenInDb(const TokenInfo &token)
{
    QSqlDatabase db = QSqlDatabase::database(m_connectionName);
    QSqlQuery q(db);
    q.prepare("UPDATE tokens SET name=?, secret=?, issuer=?, account=?, digits=?, period=?, "
              "algorithm=?, token_type=?, hotp_counter=?, badge_color=?, icon_collection_id=?, "
              "group_id=?, order_position=?, updated_at=? WHERE id=?");
    q.addBindValue(token.name);
    q.addBindValue(token.secret);
    q.addBindValue(token.issuer);
    q.addBindValue(token.account);
    q.addBindValue(token.digits);
    q.addBindValue(token.period);
    q.addBindValue(token.algorithm);
    q.addBindValue(token.tokenType);
    q.addBindValue(token.hotpCounter);
    q.addBindValue(token.badgeColor);
    q.addBindValue(token.iconCollectionId);
    q.addBindValue(token.groupId);
    q.addBindValue(token.orderPosition);
    q.addBindValue(QDateTime::currentMSecsSinceEpoch());
    q.addBindValue(token.dbId);

    if (!q.exec()) {
        QString error = QString("Failed to update token: %1").arg(q.lastError().text());
        qWarning() << error;
        return Result<bool>::fail(error);
    }
    return Result<bool>::ok(true);
}

Result<bool> TokenManager::deleteAllTokens()
{
    QSqlDatabase db = QSqlDatabase::database(m_connectionName);
    QSqlQuery q(db);
    if (!q.exec("DELETE FROM tokens")) {
        QString error = QString("Failed to delete all tokens: %1").arg(q.lastError().text());
        qWarning() << error;
        return Result<bool>::fail(error);
    }
    return Result<bool>::ok(true);
}

void TokenManager::resolveIcons(TokenInfo &token) const
{
    if (!m_iconProvider || !token.iconCollectionId.isEmpty())
        return;

    token.iconCollectionId = m_iconProvider->matchByName(token.name);
    if (!token.iconCollectionId.isEmpty())
        return;

    if (!token.issuer.isEmpty())
        token.iconCollectionId = m_iconProvider->matchByName(token.issuer);
}

// ---- Q_INVOKABLE public API ----

bool TokenManager::addToken(const QVariantMap &data)
{
    TokenInfo info;
    info.name = data.value("name").toString();
    info.secret = data.value("secret").toString().remove(' ');
    info.issuer = data.value("issuer").toString();
    info.account = data.value("account").toString();
    info.digits = data.value("digits", 6).toInt();
    info.period = data.value("period", 30).toInt();
    info.algorithm = data.value("algorithm", "SHA1").toString();
    info.tokenType = data.value("tokenType", "TOTP").toString();
    info.hotpCounter = data.value("hotpCounter", 1).toInt();
    info.badgeColor = data.value("badgeColor", "#3B82F6").toString();
    info.iconCollectionId = data.value("iconCollectionId").toString();
    info.orderPosition = m_tokens.size();

    if (info.name.isEmpty() || info.secret.isEmpty())
        return false;

    auto saveResult = saveToken(info);
    if (!saveResult) {
        return false;
    }

    QSqlDatabase db = QSqlDatabase::database(m_connectionName);
    QSqlQuery q(db);
    if (q.exec("SELECT last_insert_rowid()") && q.next())
        info.dbId = q.value(0).toInt();

    resolveIcons(info);

    m_tokens.append(info);
    emit tokensChanged();
    return true;
}

bool TokenManager::removeToken(int index)
{
    if (index < 0 || index >= m_tokens.size())
        return false;

    auto deleteResult = deleteToken(m_tokens[index].dbId);
    if (!deleteResult) {
        qWarning() << "Failed to delete token from DB:" << deleteResult.error;
    }
    m_tokens.removeAt(index);
    emit tokensChanged();
    return true;
}

bool TokenManager::updateToken(int index, const QVariantMap &data)
{
    if (index < 0 || index >= m_tokens.size())
        return false;

    TokenInfo &info = m_tokens[index];
    if (data.contains("name")) info.name = data["name"].toString();
    if (data.contains("secret")) info.secret = data["secret"].toString();
    if (data.contains("issuer")) info.issuer = data["issuer"].toString();
    if (data.contains("account")) info.account = data["account"].toString();
    if (data.contains("digits")) info.digits = data["digits"].toInt();
    if (data.contains("period")) info.period = data["period"].toInt();
    if (data.contains("algorithm")) info.algorithm = data["algorithm"].toString();
    if (data.contains("hotpCounter")) info.hotpCounter = data["hotpCounter"].toInt();
    if (data.contains("iconCollectionId")) info.iconCollectionId = data["iconCollectionId"].toString();

    auto updateResult = updateTokenInDb(info);
    if (!updateResult) {
        qWarning() << "Failed to update token in DB:" << updateResult.error;
    }
    emit tokenUpdated(index);
    return true;
}

QVariantMap TokenManager::getToken(int index) const
{
    QVariantMap map;
    if (index < 0 || index >= m_tokens.size())
        return map;

    const TokenInfo &t = m_tokens[index];
    map["name"] = t.name;
    map["secret"] = t.secret;
    map["issuer"] = t.issuer;
    map["account"] = t.account;
    map["digits"] = t.digits;
    map["period"] = t.period;
    map["algorithm"] = t.algorithm;
    map["tokenType"] = t.tokenType;
    map["hotpCounter"] = t.hotpCounter;
    map["badgeColor"] = t.badgeColor;
    map["iconCollectionId"] = t.iconCollectionId;
    return map;
}

void TokenManager::clearAllTokens()
{
    auto deleteResult = deleteAllTokens();
    if (!deleteResult) {
        qWarning() << "Failed to delete all tokens from DB:" << deleteResult.error;
    }
    m_tokens.clear();
    emit tokensChanged();
}

Result<bool> TokenManager::importTokens(const QList<TokenInfo> &newTokens)
{
    int added = 0;
    for (TokenInfo token : newTokens) {
        bool duplicate = false;
        for (const TokenInfo &existing : m_tokens) {
            if (existing.secret.compare(token.secret, Qt::CaseInsensitive) == 0) {
                duplicate = true;
                break;
            }
        }
        if (!duplicate) {
            resolveIcons(token);
            token.orderPosition = m_tokens.size();
            auto saveResult = saveToken(token);
            if (saveResult) {
                QSqlDatabase db = QSqlDatabase::database(m_connectionName);
                QSqlQuery q(db);
                if (q.exec("SELECT last_insert_rowid()") && q.next())
                    token.dbId = q.value(0).toInt();
                m_tokens.append(token);
                added++;
            }
        }
    }

    if (added > 0) {
        emit tokensChanged();
    }
    return Result<bool>::ok(added > 0);
}

QString TokenManager::defaultExportPath() const
{
    return QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/2fas_backup.2fas";
}

bool TokenManager::exportToFile(const QString &filePath)
{
    QString path = filePath;
    if (!path.endsWith(".2fas", Qt::CaseInsensitive))
        path += ".2fas";

    QJsonObject root;
    root["schemaVersion"] = 4;
    root["appVersionCode"] = 1;
    root["appVersionName"] = "1.0.0";
    root["appOrigin"] = "desktop";
    root["updatedAt"] = QDateTime::currentMSecsSinceEpoch();

    QJsonArray services;
    for (const TokenInfo &t : m_tokens) {
        services.append(t.toJson());
    }
    root["services"] = services;
    root["groups"] = QJsonArray();

    QJsonDocument doc(root);
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly))
        return false;
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
    return true;
}
