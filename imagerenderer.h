#ifndef IMAGERENDERER_H
#define IMAGERENDERER_H

#include <QQuickPaintedItem>
#include <QImage>

class ImageRenderer : public QQuickPaintedItem
{
	Q_OBJECT
	public:
	    ImageRenderer(QQuickItem *parent = 0);
	    void paint(QPainter *painter);

	    Q_INVOKABLE void setImage(const QImage &image);

private:
	    QImage m_image;
};

#endif // IMAGERENDERER_H
