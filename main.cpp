#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "mpintegrationexample.h"
#include "imagerenderer.h"
#include "graphs.h"

int main(int argc, char *argv[])
{
	StyleClass::declareQML();
	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

	QGuiApplication app(argc, argv);

	QQmlApplicationEngine engine;
	const QUrl url(QStringLiteral("qrc:/main.qml"));
	QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
			 &app, [url](QObject *obj, const QUrl &objUrl) {
		if (!obj && url == objUrl)
			QCoreApplication::exit(-1);
	}, Qt::QueuedConnection);

	MPIntegrationExample example;
	example.setGraph(StyleClass::Graphs::SelfieSegmentation);

	qmlRegisterType<ImageRenderer>("com.example.renderer", 1, 0, "ImageRenderer");
	engine.rootContext()->setContextProperty("MP", &example);

	engine.load(url);

	return app.exec();
}
