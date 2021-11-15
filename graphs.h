#ifndef GRAPHS_H
#define GRAPHS_H

#include <string>
#include <QObject>
#include <QString>
#include <QMap>
#include <QQmlEngine>

class StyleClass : public QObject
{
	Q_OBJECT

public:
	StyleClass();

	enum Graphs {
		SelfieSegmentation = 1,
		HandTracking,
		FaceDetection,
		FaceMesh,
		HairSegmentation,
		HolisticTracking,
		IrisTracking,
		ObjectDetection,
		PoseTracking
	};
	Q_ENUMS(Graphs)

	static void declareQML();
	static std::string getGraph(Graphs graph);

public slots:
	static QString graphName(Graphs graph);
};




#endif // GRAPHS_H
