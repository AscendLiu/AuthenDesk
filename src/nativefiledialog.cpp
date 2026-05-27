#include "nativefiledialog.h"
#include <QFileDialog>

NativeFileDialog::NativeFileDialog(QObject *parent)
    : QObject(parent)
{
}

QString NativeFileDialog::getOpenFileName(const QString &title,
                                          const QString &dir,
                                          const QString &filter)
{
    return QFileDialog::getOpenFileName(nullptr, title, dir, filter);
}

QString NativeFileDialog::getSaveFileName(const QString &title,
                                          const QString &dir,
                                          const QString &fileName,
                                          const QString &filter)
{
    return QFileDialog::getSaveFileName(nullptr, title, dir + "/" + fileName, filter);
}

QStringList NativeFileDialog::getOpenFileNames(const QString &title,
                                               const QString &dir,
                                               const QString &filter)
{
    return QFileDialog::getOpenFileNames(nullptr, title, dir, filter);
}
