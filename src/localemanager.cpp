#include "localemanager.h"
#include <QCoreApplication>
#include <QDebug>
#include <QDir>

static const QString SETTINGS_ORG = QStringLiteral("AuthenDesk");
static const QString SETTINGS_APP = QStringLiteral("AuthenDesk");
static const QString LANGUAGE_KEY = QStringLiteral("language");
static const QString SETTINGS_FILE = QStringLiteral("authendesk.conf");

LocaleManager::LocaleManager(QObject *parent)
    : QObject(parent)
    , m_isSystemDefault(true)
{
    Language savedLang = loadSavedLanguage();
    if (savedLang == Language::Unset) {
        m_currentLanguage = detectSystemLanguage();
    } else {
        m_currentLanguage = savedLang;
        m_isSystemDefault = false;
    }
}

QString LocaleManager::currentLanguage() const
{
    return m_currentLanguage == Language::Chinese ? QStringLiteral("zh") : QStringLiteral("en");
}

QStringList LocaleManager::availableLanguages() const
{
    return {QStringLiteral("zh"), QStringLiteral("en")};
}

bool LocaleManager::isSystemDefault() const
{
    return m_isSystemDefault;
}

void LocaleManager::setLanguage(LocaleManager::Language lang)
{
    if (m_currentLanguage == lang && !m_isSystemDefault)
        return;

    m_currentLanguage = lang;
    m_isSystemDefault = false;
    saveLanguage(lang);
    emit languageChanged();
}

void LocaleManager::resetToSystemDefault()
{
    m_currentLanguage = detectSystemLanguage();
    m_isSystemDefault = true;

    QSettings settings(SETTINGS_ORG, SETTINGS_APP);
    settings.remove(QStringLiteral("language"));

    emit languageChanged();
}

QString LocaleManager::systemLanguage() const
{
    Language lang = detectSystemLanguage();
    return lang == Language::Chinese ? QStringLiteral("zh") : QStringLiteral("en");
}

LocaleManager::Language LocaleManager::detectSystemLanguage() const
{
    QString langEnv = QString::fromUtf8(qgetenv("LANG"));
    if (langEnv.contains(QStringLiteral("zh_CN")) || langEnv.contains(QStringLiteral("zh_TW")) ||
        langEnv.contains(QStringLiteral("zh_HK")) || langEnv.contains(QStringLiteral("zh_MO"))) {
        return Language::Chinese;
    }

    QString lcAll = QString::fromUtf8(qgetenv("LC_ALL"));
    if (lcAll.contains(QStringLiteral("zh"))) {
        return Language::Chinese;
    }

    QString langEnvLower = langEnv.toLower();
    if (langEnvLower.startsWith(QStringLiteral("zh"))) {
        return Language::Chinese;
    }

    QString language = QString::fromUtf8(qgetenv("LANGUAGE"));
    if (language.contains(QStringLiteral("zh"))) {
        return Language::Chinese;
    }

    QLocale locale;
    if (locale.language() == QLocale::Chinese) {
        return Language::Chinese;
    }

    return Language::English;
}

LocaleManager::Language LocaleManager::loadSavedLanguage() const
{
    QSettings settings(SETTINGS_ORG, SETTINGS_APP);
    int langValue = settings.value(QStringLiteral("language"), -1).toInt();

    if (langValue == static_cast<int>(Language::Chinese))
        return Language::Chinese;
    else if (langValue == static_cast<int>(Language::English))
        return Language::English;

    return Language::Unset;
}

void LocaleManager::saveLanguage(Language lang)
{
    QSettings settings(SETTINGS_ORG, SETTINGS_APP);
    settings.setValue(QStringLiteral("language"), static_cast<int>(lang));
    settings.sync();
}

SettingsManager::SettingsManager(QObject *parent)
    : QObject(parent)
    , m_settings(SETTINGS_ORG, SETTINGS_APP)
{
}

QVariant SettingsManager::get(const QString &group, const QString &key, const QVariant &defaultValue)
{
    m_settings.beginGroup(group);
    QVariant value = m_settings.value(key, defaultValue);
    m_settings.endGroup();
    return value;
}

void SettingsManager::set(const QString &group, const QString &key, const QVariant &value)
{
    m_settings.beginGroup(group);
    m_settings.setValue(key, value);
    m_settings.endGroup();
    m_settings.sync();
}

void SettingsManager::remove(const QString &group, const QString &key)
{
    m_settings.beginGroup(group);
    m_settings.remove(key);
    m_settings.endGroup();
    m_settings.sync();
}
