#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QIcon>

#include "tokenmanager.h"
#include "models/tokenlistmodel.h"
#include "importparser.h"
#include "serviceiconprovider.h"
#include "qrdecoder.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    app.setWindowIcon(QIcon(":/assets/app_icon.png"));
    app.setApplicationName("AuthenDesk");
    app.setOrganizationName("AuthenDesk");
    app.setApplicationVersion("1.0.0");
    app.setDesktopFileName("authendesk");

    ServiceIconProvider iconProvider;
    iconProvider.load(":/assets/services.json");

    TokenManager tokenManager(&iconProvider);
    tokenManager.loadFromDatabase();

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

    engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
