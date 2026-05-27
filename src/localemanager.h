#pragma once

#include <QObject>
#include <QString>
#include <QSettings>
#include <QLocale>

class LocaleManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString currentLanguage READ currentLanguage NOTIFY languageChanged)
    Q_PROPERTY(QStringList availableLanguages READ availableLanguages CONSTANT)
    Q_PROPERTY(bool isSystemDefault READ isSystemDefault NOTIFY languageChanged)

public:
    enum Language {
        Unset,
        Chinese,
        English
    };
    Q_ENUM(Language)

    explicit LocaleManager(QObject *parent = nullptr);

    QString currentLanguage() const;
    QStringList availableLanguages() const;
    bool isSystemDefault() const;

    Q_INVOKABLE void setLanguage(LocaleManager::Language lang);
    Q_INVOKABLE void resetToSystemDefault();
    Q_INVOKABLE QString systemLanguage() const;

signals:
    void languageChanged();

private:
    Language detectSystemLanguage() const;
    Language loadSavedLanguage() const;
    void saveLanguage(Language lang);

    Language m_currentLanguage;
    bool m_isSystemDefault;
    QString m_settingsPath;
};

class SettingsManager : public QObject
{
    Q_OBJECT
public:
    explicit SettingsManager(QObject *parent = nullptr);

    Q_INVOKABLE QVariant get(const QString &group, const QString &key, const QVariant &defaultValue = QVariant());
    Q_INVOKABLE void set(const QString &group, const QString &key, const QVariant &value);
    Q_INVOKABLE void remove(const QString &group, const QString &key);

private:
    QSettings m_settings;
};
