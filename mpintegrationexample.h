#ifndef MPINTEGRATIONEXAMPLE_H
#define MPINTEGRATIONEXAMPLE_H

#include <QObject>
#include <QSharedPointer>
#include <QCamera>
#include <QCameraImageCapture>
#include "example.h"
#include "graphs.h"

class MPIntegrationExample : public QObject
{
	Q_OBJECT
	Q_PROPERTY(double fps READ fps NOTIFY fpsChanged)
public:
	explicit MPIntegrationExample(QObject *parent = nullptr);
	void processImage(const QImage& image);

	double fps() const;

public slots:
	void startCamera();
	void stopCamera();
	void setGraph(StyleClass::Graphs graph);
	void setGraph(int graph);

signals:
	void imageProcessed(const QImage& image);
	void fpsChanged(double fps);

private:
	QSharedPointer<mediapipe::Example> m_mp;
	bool m_processing = false;
	QSharedPointer<QCamera> m_camera;
	QSharedPointer<QCameraImageCapture> m_imageCapture;
	QSharedPointer<QTimer> m_timer;
	qint64 m_start;
	double m_fps = 0;

};

#endif // MPINTEGRATIONEXAMPLE_H
