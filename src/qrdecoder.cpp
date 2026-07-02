#include "qrdecoder.h"
#include <QUrl>
#include <QUrlQuery>
#include <QDebug>
#include <QScreen>
#include <QPixmap>
#include <QGuiApplication>
#include <QTemporaryFile>
#include <QDir>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusPendingCall>
#include <QDBusPendingCallWatcher>
#include <QDBusPendingReply>
#include <QImage>
#include <QEventLoop>
#include <QTimer>
#include <QFile>
#include <QUuid>

QrDecoder::QrDecoder(QObject *parent)
    : QObject(parent)
    , m_process(new QProcess(this))
    , m_cropX(0), m_cropY(0), m_cropW(0), m_cropH(0)
{
    connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &QrDecoder::onProcessFinished);
}

void QrDecoder::decodeImage(const QString &filePath)
{
    m_pendingFile = filePath;
    m_process->start("zbarimg", {"-q", "--raw", filePath});
}

void QrDecoder::onProcessFinished(int exitCode, QProcess::ExitStatus status)
{
    if (status != QProcess::NormalExit || exitCode != 0) {
        QString err = QString::fromUtf8(m_process->readAllStandardError());
        emit decodeError(err.isEmpty() ? "Failed to decode QR code" : err.trimmed());
        return;
    }

    QString result = QString::fromUtf8(m_process->readAllStandardOutput()).trimmed();
    if (result.isEmpty()) { emit decodeError("No QR code found in image"); return; }
    if (result.contains('\n')) result = result.split('\n')[0];
    if (!result.startsWith("otpauth://")) { emit decodeError("Not a valid OTP auth QR code"); return; }

    QVariantMap data = parseOtpAuthUri(result);
    if (data.isEmpty()) { emit decodeError("Failed to parse OTP auth URI"); return; }
    data["_rawUri"] = result;
    emit decodeFinished(data);
}

void QrDecoder::captureScreenAndDecode()
{
    QScreen *screen = QGuiApplication::primaryScreen();
    if (!screen) { emit decodeError("Cannot get screen"); return; }

    QPixmap pixmap = screen->grabWindow(0);
    if (!pixmap.isNull()) {
        delete m_tempFile;
        m_tempFile = new QTemporaryFile(this);
        m_tempFile->setFileTemplate("/tmp/authendesk_qr_XXXXXX.png");
        if (!m_tempFile->open()) { emit decodeError("Failed to create temp file"); return; }
        pixmap.save(m_tempFile, "PNG"); m_tempFile->close();
        m_pendingFile = m_tempFile->fileName();
        m_process->start("zbarimg", {"-q", "--raw", m_tempFile->fileName()});
        return;
    }

    doWaylandCapture(0, 0, 0, 0);
}

void QrDecoder::captureRect(int x, int y, int w, int h)
{
    QScreen *screen = QGuiApplication::primaryScreen();
    if (!screen) { emit decodeError("Cannot get screen"); return; }

    QPixmap fullScreen = screen->grabWindow(0);
    if (!fullScreen.isNull()) {
        qreal dpr = screen->devicePixelRatio();
        int rx = qRound(x * dpr), ry = qRound(y * dpr);
        int rw = qRound(w * dpr), rh = qRound(h * dpr);
        QPixmap cropped = fullScreen.copy(rx, ry, rw, rh);
        if (cropped.isNull()) { emit decodeError("Failed to crop region"); return; }
        delete m_tempFile;
        m_tempFile = new QTemporaryFile(this);
        m_tempFile->setFileTemplate("/tmp/authendesk_qr_XXXXXX.png");
        if (!m_tempFile->open()) { emit decodeError("Failed to create temp file"); return; }
        cropped.save(m_tempFile, "PNG"); m_tempFile->close();
        m_pendingFile = m_tempFile->fileName();
        m_process->start("zbarimg", {"-q", "--raw", m_tempFile->fileName()});
        return;
    }

    doWaylandCapture(x, y, w, h);
}

void QrDecoder::doWaylandCapture(int rx, int ry, int rw, int rh)
{
    m_cropX = rx; m_cropY = ry; m_cropW = rw; m_cropH = rh;

    if (m_restoreToken.isEmpty())
        m_restoreToken = "authendesk_" + QUuid::createUuid().toString(QUuid::Id128);

    auto msg = QDBusMessage::createMethodCall(
        "org.freedesktop.portal.Desktop",
        "/org/freedesktop/portal/desktop",
        "org.freedesktop.portal.Screenshot",
        "Screenshot");

    QVariantMap options;
    options["interactive"] = false;
    options["modal"] = false;
    options["restore_token"] = m_restoreToken;
    msg << QString("") << options;

    auto call = QDBusConnection::sessionBus().asyncCall(msg);
    auto watcher = new QDBusPendingCallWatcher(call, this);

    connect(watcher, &QDBusPendingCallWatcher::finished, this, [this](QDBusPendingCallWatcher *w) {
        w->deleteLater();
        QDBusPendingReply<QDBusObjectPath> reply = *w;
        if (reply.isError()) {
            qWarning() << "Portal call error:" << reply.error().message();
            emit decodeError("Cannot capture screen (Portal not available)");
            return;
        }

        QString handle = reply.value().path();

        QDBusConnection::sessionBus().connect(
            "org.freedesktop.portal.Desktop",
            handle,
            "org.freedesktop.portal.Request",
            "Response",
            this, SLOT(onPortalResponse(uint, QVariantMap)));
    });
}

void QrDecoder::onPortalResponse(uint response, QVariantMap results)
{
    if (response != 0) {
        emit decodeError("Screen capture cancelled or denied");
        return;
    }

    QString uri = results.value("uri").toString();
    if (uri.isEmpty()) { emit decodeError("Screen capture returned empty data"); return; }
    uri = QUrl(uri).toLocalFile();
    if (uri.isEmpty() || !QFile::exists(uri)) { emit decodeError("Screen capture file does not exist"); return; }

    QImage img(uri);
    if (img.isNull()) { emit decodeError("Cannot read screen capture image"); return; }

    if (m_cropW > 0 && m_cropH > 0) {
        QScreen *screen = QGuiApplication::primaryScreen();
        qreal dpr = screen ? screen->devicePixelRatio() : 1.0;
        int cx = qRound(m_cropX * dpr), cy = qRound(m_cropY * dpr);
        int cw = qRound(m_cropW * dpr), ch = qRound(m_cropH * dpr);
        img = img.copy(cx, cy, cw, ch);
        if (img.isNull()) { emit decodeError("Screen capture crop failed"); return; }
    }

    delete m_tempFile;
    m_tempFile = new QTemporaryFile(this);
    m_tempFile->setFileTemplate("/tmp/authendesk_qr_XXXXXX.png");
    if (!m_tempFile->open()) { emit decodeError("Failed to create temp file"); return; }
    m_tempFile->close();
    img.save(m_tempFile->fileName(), "PNG");

    QFile::remove(uri); // clean up portal temp file

    m_pendingFile = m_tempFile->fileName();
    m_process->start("zbarimg", {"-q", "--raw", m_tempFile->fileName()});
}

QVariantMap QrDecoder::parseOtpAuthUri(const QString &uri)
{
    QVariantMap map;
    QUrl url(uri);
    if (url.scheme() != "otpauth") return {};

    QString path = url.path();
    map["tokenType"] = url.host();
    if (path.startsWith('/')) path = path.mid(1);

    int colonPos = path.indexOf(':');
    if (colonPos >= 0) {
        map["issuer"] = QUrl::fromPercentEncoding(path.left(colonPos).toUtf8());
        map["account"] = QUrl::fromPercentEncoding(path.mid(colonPos + 1).toUtf8());
    } else {
        map["account"] = QUrl::fromPercentEncoding(path.toUtf8());
    }

    QUrlQuery query(url);
    map["name"] = map["issuer"].toString().isEmpty() ? map["account"].toString() : map["issuer"].toString();
    if (query.hasQueryItem("secret"))
        map["secret"] = query.queryItemValue("secret").toUpper();
    if (query.hasQueryItem("issuer"))
        map["issuer"] = query.queryItemValue("issuer");
    if (query.hasQueryItem("algorithm"))
        map["algorithm"] = query.queryItemValue("algorithm");
    if (query.hasQueryItem("digits"))
        map["digits"] = query.queryItemValue("digits").toInt();
    if (query.hasQueryItem("period"))
        map["period"] = query.queryItemValue("period").toInt();
    if (query.hasQueryItem("counter"))
        map["hotpCounter"] = query.queryItemValue("counter").toInt();

    if (map["digits"].toInt() == 0) map["digits"] = 6;
    if (map["period"].toInt() == 0) map["period"] = 30;
    if (map["algorithm"].toString().isEmpty()) map["algorithm"] = "SHA1";
    if (!map.contains("tokenType") || map["tokenType"].toString().isEmpty()) map["tokenType"] = "TOTP";

    return map;
}
