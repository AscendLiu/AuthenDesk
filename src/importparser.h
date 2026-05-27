#pragma once

#include "tokendata.h"
#include <QObject>
#include <QString>
#include <QList>
#include <QPair>

class ImportParser : public QObject
{
    Q_OBJECT
public:
    enum Error {
        NoError,
        FileNotFound,
        InvalidJson,
        WrongPassword,
        DecryptionFailed,
        EmptyServices
    };
    Q_ENUM(Error)

    explicit ImportParser(QObject *parent = nullptr) : QObject(parent) {}

    Q_INVOKABLE bool importFromFile(const QString &filePath, const QString &password,
                                     QObject *tokenManagerObj);

    static QPair<QList<TokenInfo>, Error> parseFile(const QString &filePath,
                                                       const QString &password = QString());

private:
    static QPair<QList<TokenInfo>, Error> parseBackupContent(const QByteArray &jsonData);
    static QPair<QList<TokenInfo>, Error> decryptAndParse(const QByteArray &jsonData,
                                                             const QString &password);
};
