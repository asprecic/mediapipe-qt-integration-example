#include "mpintegrationexample.h"
#include <QTimer>
#include <QThreadPool>
#include <QtConcurrent/QtConcurrentRun>
#include <QDateTime>

MPIntegrationExample::MPIntegrationExample(QObject *parent) : QObject(parent)
{
	m_camera = QSharedPointer<QCamera>(new QCamera());
	m_imageCapture = QSharedPointer<QCameraImageCapture>(new QCameraImageCapture(m_camera.get()));

	m_camera->setCaptureMode(QCamera::CaptureStillImage);

	m_timer = QSharedPointer<QTimer>(new QTimer());
}

void MPIntegrationExample::setGraph(StyleClass::Graphs graph)
{
	m_mp = QSharedPointer<mediapipe::Example>(mediapipe::Example::Create(StyleClass::getGraph(graph)));
}

void MPIntegrationExample::setGraph(int graph)
{
	setGraph((StyleClass::Graphs)graph);
}

double MPIntegrationExample::fps() const
{
	return m_fps;
}

void MPIntegrationExample::processImage(const QImage &image)
{
	// drop frame
	if (m_processing || m_mp == nullptr){
		return;
	}

	m_processing = true;
	m_start = QDateTime::currentMSecsSinceEpoch();
	QtConcurrent::run([this, image]()
	{
		QImage input_img = image.convertToFormat(QImage::Format_RGB888);
		uint8_t* data = m_mp->Process(input_img.constBits(), input_img.width(), input_img.height());

		if (data == nullptr)
			return;

		QImage processed_image(data, input_img.width(), input_img.height(), QImage::Format_RGB888, [](void* ptr){delete (uint8_t*)ptr;}, data);
		emit imageProcessed(processed_image);
		m_processing = false;
		m_fps = 1000./(QDateTime::currentMSecsSinceEpoch() - m_start);
		emit fpsChanged(m_fps);
	});
}

void MPIntegrationExample::startCamera()
{
	m_timer->setInterval(50);
	connect(m_timer.get(), &QTimer::timeout, this, [this](){
		m_camera->start();
		//on half pressed shutter button
		m_camera->searchAndLock();

		//on shutter button pressed
		m_imageCapture->capture();

		//on shutter button released
		m_camera->unlock();
	});

	connect(m_imageCapture.get(), &QCameraImageCapture::imageCaptured, this, [this](int id, const QImage& image) {
		processImage(image);
	});
	m_timer->start();

}

void MPIntegrationExample::stopCamera()
{
	m_timer->stop();

}
