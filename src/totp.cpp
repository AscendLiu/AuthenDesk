#include "totp.h"
#include <cmath>

static const char BASE32_ALPHABET[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";

QByteArray TotpGenerator::decodeBase32(const QString &base32)
{
    QString input = base32.toUpper().remove(QChar(' ')).remove(QChar('-'));
    input.remove(QChar('='));

    QByteArray output;
    int buffer = 0;
    int bitsLeft = 0;

    for (const QChar &c : input) {
        int value = -1;
        for (int i = 0; i < 32; i++) {
            if (BASE32_ALPHABET[i] == c.toLatin1()) {
                value = i;
                break;
            }
        }
        if (value < 0) continue;

        buffer = (buffer << 5) | value;
        bitsLeft += 5;

        if (bitsLeft >= 8) {
            bitsLeft -= 8;
            output.append(static_cast<char>((buffer >> bitsLeft) & 0xFF));
        }
    }
    return output;
}

QCryptographicHash::Algorithm TotpGenerator::algorithmFromString(const QString &algo)
{
    if (algo == "SHA256")
        return QCryptographicHash::Sha256;
    if (algo == "SHA384")
        return QCryptographicHash::Sha384;
    if (algo == "SHA512")
        return QCryptographicHash::Sha512;
    return QCryptographicHash::Sha1;
}

QByteArray TotpGenerator::hmacSha(const QByteArray &key,
                                    const QByteArray &data,
                                    QCryptographicHash::Algorithm algo)
{
    // HMAC block size
    int blockSize = 64;
    if (algo == QCryptographicHash::Sha384 || algo == QCryptographicHash::Sha512)
        blockSize = 128;

    QByteArray keyUsed = key;
    if (keyUsed.size() > blockSize) {
        keyUsed = QCryptographicHash::hash(keyUsed, algo);
    }
    while (keyUsed.size() < blockSize) {
        keyUsed.append('\0');
    }

    QByteArray oKeyPad(blockSize, 0);
    QByteArray iKeyPad(blockSize, 0);
    for (int i = 0; i < blockSize; i++) {
        oKeyPad[i] = keyUsed[i] ^ 0x5c;
        iKeyPad[i] = keyUsed[i] ^ 0x36;
    }

    QByteArray innerHash = QCryptographicHash::hash(iKeyPad + data, algo);
    return QCryptographicHash::hash(oKeyPad + innerHash, algo);
}

TotpResult TotpGenerator::generateTotp(const QString &secretBase32,
                                        int digits,
                                        int period,
                                        const QString &algorithm,
                                        qint64 timestamp)
{
    TotpResult result;
    result.code = "------";

    QByteArray key = decodeBase32(secretBase32);
    if (key.isEmpty()) {
        result.remainingSeconds = period;
        result.nextUpdateTime = QDateTime::currentMSecsSinceEpoch() + period * 1000;
        return result;
    }

    if (timestamp < 0) {
        timestamp = QDateTime::currentSecsSinceEpoch();
    } else {
        timestamp = timestamp / 1000;
    }

    quint64 counter = static_cast<quint64>(timestamp) / period;
    result.remainingSeconds = period - (timestamp % period);
    result.nextUpdateTime = QDateTime::currentMSecsSinceEpoch() + result.remainingSeconds * 1000;

    // Convert counter to 8-byte big-endian
    QByteArray counterBytes(8, 0);
    for (int i = 7; i >= 0; i--) {
        counterBytes[i] = static_cast<char>(counter & 0xFF);
        counter >>= 8;
    }

    QCryptographicHash::Algorithm algo = algorithmFromString(algorithm);
    QByteArray hash = hmacSha(key, counterBytes, algo);

    // Dynamic truncation
    int offset = hash[hash.size() - 1] & 0x0F;
    int binary =
        ((hash[offset] & 0x7F) << 24) |
        ((hash[offset + 1] & 0xFF) << 16) |
        ((hash[offset + 2] & 0xFF) << 8) |
        (hash[offset + 3] & 0xFF);

    quint32 modulus = 1;
    for (int i = 0; i < digits; i++) {
        modulus *= 10;
    }

    quint32 code = binary % modulus;
    result.code = QString::number(code).rightJustified(digits, '0');
    return result;
}

TotpResult TotpGenerator::generateHotp(const QString &secretBase32,
                                        quint64 counter,
                                        int digits,
                                        const QString &algorithm)
{
    TotpResult result;
    result.code = "------";
    result.remainingSeconds = -1;
    result.nextUpdateTime = 0;

    QByteArray key = decodeBase32(secretBase32);
    if (key.isEmpty()) return result;

    QByteArray counterBytes(8, 0);
    quint64 c = counter;
    for (int i = 7; i >= 0; i--) {
        counterBytes[i] = static_cast<char>(c & 0xFF);
        c >>= 8;
    }

    QCryptographicHash::Algorithm algo = algorithmFromString(algorithm);
    QByteArray hash = hmacSha(key, counterBytes, algo);

    int offset = hash[hash.size() - 1] & 0x0F;
    int binary =
        ((hash[offset] & 0x7F) << 24) |
        ((hash[offset + 1] & 0xFF) << 16) |
        ((hash[offset + 2] & 0xFF) << 8) |
        (hash[offset + 3] & 0xFF);

    quint32 modulus = 1;
    for (int i = 0; i < digits; i++) {
        modulus *= 10;
    }

    quint32 code = binary % modulus;
    result.code = QString::number(code).rightJustified(digits, '0');
    return result;
}
