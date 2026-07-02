#pragma once

#include <QObject>
#include <QString>
#include <QVariantMap>
#include <QProcess>
#include <QTemporaryFile>

class QrDecoder : public QObject
{
    Q_OBJECT
public:
    explicit QrDecoder(QObject *parent = nullptr);

    Q_INVOKABLE void decodeImage(const QString &filePath);
    Q_INVOKABLE void captureScreenAndDecode();
    Q_INVOKABLE void captureRect(int x, int y, int w, int h);
    Q_INVOKABLE QVariantMap parseOtpAuthUri(const QString &uri);

public slots:
    void onPortalResponse(uint response, QVariantMap results);

signals:
    void decodeFinished(const QVariantMap &result);
    void decodeError(const QString &error);

private slots:
    void onProcessFinished(int exitCode, QProcess::ExitStatus status);

private:
    void doWaylandCapture(int rx, int ry, int rw, int rh);

    QProcess *m_process;
    QString m_pendingFile;
    QTemporaryFile *m_tempFile = nullptr;
    QString m_restoreToken;
    int m_cropX, m_cropY, m_cropW, m_cropH;
};
