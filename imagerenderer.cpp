#include "imagerenderer.h"
#include <QPainter>

ImageRenderer::ImageRenderer(QQuickItem *parent)
{

}

void ImageRenderer::paint(QPainter *painter)
{
	painter->drawImage(QPoint(0,0), m_image, m_image.rect());
}

void ImageRenderer::setImage(const QImage &image)
{
	m_image = image;
}
