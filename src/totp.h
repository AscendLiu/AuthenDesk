#pragma once

#include <QString>
#include <QByteArray>
#include <QCryptographicHash>
#include <QMessageAuthenticationCode>
#include <QDateTime>

struct TotpResult {
    QString code;
    int remainingSeconds;
    qint64 nextUpdateTime; // epoch ms
};

class TotpGenerator {
public:
    static TotpResult generateTotp(const QString &secretBase32,
                                   int digits = 6,
                                   int period = 30,
                                   const QString &algorithm = "SHA1",
                                   qint64 timestamp = -1);

    static TotpResult generateHotp(const QString &secretBase32,
                                   quint64 counter,
                                   int digits = 6,
                                   const QString &algorithm = "SHA1");

private:
    static QByteArray decodeBase32(const QString &base32);
    static QCryptographicHash::Algorithm algorithmFromString(const QString &algo);
    static QByteArray hmacSha(const QByteArray &key,
                               const QByteArray &data,
                               QCryptographicHash::Algorithm algo);
};
