#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QIcon>
#include <QQmlEngine>
#include <QJSEngine>
#include <QCommandLineParser>

#include "tokenmanager.h"
#include "models/tokenlistmodel.h"
#include "importparser.h"
#include "serviceiconprovider.h"
#include "qrdecoder.h"
#include "localemanager.h"
#include "nativefiledialog.h"
#include "version.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setWindowIcon(QIcon(":/assets/app_icon.png"));
    app.setApplicationName("AuthenDesk");
    app.setOrganizationName("AuthenDesk");
    app.setApplicationVersion(AUTHDESK_VERSION);
    app.setDesktopFileName("authendesk");

    QCommandLineParser parser;
    parser.setApplicationDescription("AuthenDesk - Desktop Two-Factor Authentication Token Manager");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.process(app);

    ServiceIconProvider iconProvider;
    iconProvider.load(":/assets/services.json");

    TokenManager tokenManager(&iconProvider);
    auto loadResult = tokenManager.loadFromDatabase();
    if (!loadResult) {
        qWarning() << "Failed to load database:" << loadResult.error;
    }

    TokenListModel model(&tokenManager, &iconProvider);

    QQmlApplicationEngine engine;

    engine.rootContext()->setContextProperty("tokenModel", &model);
    engine.rootContext()->setContextProperty("tokenManager", &tokenManager);
    engine.rootContext()->setContextProperty("iconProvider", &iconProvider);

    QrDecoder qrDecoder;
    engine.rootContext()->setContextProperty("qrDecoder", &qrDecoder);

    qmlRegisterSingletonType<ImportParser>("AuthenDesk", 1, 0, "ImportParser",
        [](QQmlEngine *, QJSEngine *) -> QObject * {
            return new ImportParser();
        });

    LocaleManager localeManager;
    SettingsManager settingsManager;
    NativeFileDialog nativeFileDialog;

    engine.rootContext()->setContextProperty("LocaleManager", &localeManager);
    engine.rootContext()->setContextProperty("SettingsManager", &settingsManager);
    engine.rootContext()->setContextProperty("nativeFileDialog", &nativeFileDialog);
    engine.rootContext()->setContextProperty("appVersion", AUTHDESK_VERSION);

    qmlRegisterSingletonType<ImportParser>("AuthenDesk", 1, 0, "ImportParser",
        [](QQmlEngine *, QJSEngine *) -> QObject * {
            return new ImportParser();
        });

    qmlRegisterSingletonType(QUrl(QStringLiteral("qrc:/qml/Strings.qml")), "AuthenDesk", 1, 0, "Strings");

    engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
