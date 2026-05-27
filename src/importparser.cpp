#include "importparser.h"
#include "tokenmanager.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/rand.h>
#include <openssl/err.h>

bool ImportParser::importFromFile(const QString &filePath, const QString &password,
                                    QObject *tokenManagerObj)
{
    TokenManager *manager = qobject_cast<TokenManager *>(tokenManagerObj);
    if (!manager) return false;

    auto [tokens, error] = parseFile(filePath, password);
    if (error != NoError || tokens.isEmpty())
        return false;

    return manager->importTokens(tokens);
}

QPair<QList<TokenInfo>, ImportParser::Error> ImportParser::parseFile(const QString &filePath,
                                                                       const QString &password)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
        return {{}, FileNotFound};

    QByteArray data = file.readAll();
    file.close();

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
    if (parseError.error != QJsonParseError::NoError)
        return {{}, InvalidJson};

    QJsonObject root = doc.object();

    bool encrypted = !root.value("reference").isNull() && !root.value("reference").toString().isEmpty();

    if (encrypted) {
        if (password.isEmpty())
            return {{}, WrongPassword};
        return decryptAndParse(data, password);
    }

    return parseBackupContent(data);
}

QPair<QList<TokenInfo>, ImportParser::Error> ImportParser::parseBackupContent(const QByteArray &jsonData)
{
    QJsonDocument doc = QJsonDocument::fromJson(jsonData);
    QJsonObject root = doc.object();
    QJsonArray services = root["services"].toArray();

    QList<TokenInfo> tokens;
    for (const QJsonValue &val : services) {
        TokenInfo info = TokenInfo::fromJson(val.toObject());
        if (!info.name.isEmpty() && !info.secret.isEmpty())
            tokens.append(info);
    }

    if (tokens.isEmpty())
        return {{}, EmptyServices};

    return {tokens, NoError};
}

QPair<QList<TokenInfo>, ImportParser::Error> ImportParser::decryptAndParse(const QByteArray &jsonData,
                                                                             const QString &password)
{
    QJsonDocument doc = QJsonDocument::fromJson(jsonData);
    QJsonObject root = doc.object();

    QString servicesEncrypted = root["servicesEncrypted"].toString();
    QString reference = root["reference"].toString();

    if (servicesEncrypted.isEmpty() || reference.isEmpty())
        return {{}, DecryptionFailed};

    auto splitParts = [](const QString &data) -> QStringList {
        int firstColon = data.indexOf(':');
        if (firstColon < 0) return {};
        int secondColon = data.indexOf(':', firstColon + 1);
        if (secondColon < 0) return {};

        return {
            data.left(firstColon),
            data.mid(firstColon + 1, secondColon - firstColon - 1),
            data.mid(secondColon + 1)
        };
    };

    auto decryptData = [&](const QString &encryptedData, const QByteArray &key) -> QByteArray {
        QStringList parts = splitParts(encryptedData);
        if (parts.size() != 3)
            return {};

        QByteArray ciphertext = QByteArray::fromBase64(parts[0].toUtf8());
        QByteArray iv = QByteArray::fromBase64(parts[2].toUtf8());

        if (ciphertext.isEmpty() || iv.isEmpty())
            return {};

        EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
        if (!ctx) return {};

        QByteArray plaintext(ciphertext.size() + 16, 0);
        int outLen = 0;
        int totalLen = 0;

        bool ok = false;
        do {
            if (EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), nullptr, nullptr, nullptr) != 1)
                break;
            if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, iv.size(), nullptr) != 1)
                break;
            if (EVP_DecryptInit_ex(ctx, nullptr, nullptr,
                                    reinterpret_cast<const unsigned char *>(key.constData()),
                                    reinterpret_cast<const unsigned char *>(iv.constData())) != 1)
                break;

            const unsigned char *ct = reinterpret_cast<const unsigned char *>(ciphertext.constData());
            int ctLen = ciphertext.size() - 16;
            if (ctLen <= 0) break;

            if (EVP_DecryptUpdate(ctx, reinterpret_cast<unsigned char *>(plaintext.data()),
                                   &outLen, ct, ctLen) != 1)
                break;
            totalLen = outLen;

            if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, 16,
                                     const_cast<unsigned char *>(ct + ctLen)) != 1)
                break;

            if (EVP_DecryptFinal_ex(ctx, reinterpret_cast<unsigned char *>(plaintext.data()) + totalLen,
                                     &outLen) != 1)
                break;
            totalLen += outLen;
            ok = true;
        } while (false);

        EVP_CIPHER_CTX_free(ctx);

        if (!ok) return {};
        return plaintext.left(totalLen);
    };

    auto deriveKey = [](const QString &password, const QByteArray &salt) -> QByteArray {
        QByteArray key(32, 0);
        PKCS5_PBKDF2_HMAC(password.toUtf8().constData(), password.toUtf8().size(),
                           reinterpret_cast<const unsigned char *>(salt.constData()), salt.size(),
                           10000, EVP_sha256(),
                           32, reinterpret_cast<unsigned char *>(key.data()));
        return key;
    };

    QStringList servicesParts = splitParts(servicesEncrypted);
    if (servicesParts.size() != 3)
        return {{}, DecryptionFailed};
    QByteArray salt = QByteArray::fromBase64(servicesParts[1].toUtf8());
    QByteArray key = deriveKey(password, salt);

    QByteArray refPlain = decryptData(reference, key);
    if (refPlain.isEmpty())
        return {{}, WrongPassword};

    QByteArray servicesPlain = decryptData(servicesEncrypted, key);
    if (servicesPlain.isEmpty())
        return {{}, DecryptionFailed};

    return parseBackupContent(servicesPlain);
}
