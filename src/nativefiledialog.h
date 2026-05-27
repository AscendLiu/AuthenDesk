#pragma once

#include <QObject>
#include <QString>
#include <QStringList>
#include <QFileDialog>
#include <QWidget>

class NativeFileDialog : public QObject
{
    Q_OBJECT

public:
    explicit NativeFileDialog(QObject *parent = nullptr);

    Q_INVOKABLE QString getOpenFileName(const QString &title,
                                         const QString &dir,
                                         const QString &filter);

    Q_INVOKABLE QString getSaveFileName(const QString &title,
                                         const QString &dir,
                                         const QString &fileName,
                                         const QString &filter);

    Q_INVOKABLE QStringList getOpenFileNames(const QString &title,
                                             const QString &dir,
                                             const QString &filter);
};
