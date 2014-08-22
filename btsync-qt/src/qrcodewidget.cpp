#include <QtDebug>

#include <QPaintEvent>
#include <QPainter>

#include <qrencode.h>

#include "qrcodewidget.h"


QRCodeWidget::QRCodeWidget(QWidget *parent)
	:QWidget(parent)
{
	setText("");
}

void QRCodeWidget::setText(const QString &text)
{
	QRcode *qr = QRcode_encodeString(text.toUtf8().constData(), 0, QR_ECLEVEL_L, QR_MODE_8, 1);

	if(!qr)
	{
		codeImage = QPixmap(128, 128);
		codeImage.fill(Qt::red);
		setMinimumSize(codeImage.size());
		update();
		return;
	}

	codeImage = QPixmap(qr->width, qr->width);
	codeImage.fill(Qt::white);

	QPainter painter(&codeImage);
	painter.setBrush(Qt::black);
	painter.setPen(Qt::SolidLine);

	int size = qr->width;
	if(size <= 0)
		size = 1;

	for(int y = 0; y < size; ++y)
	{
		for(int x = 0; x < size; ++x)
		{
			int ppos = y * size + x;
			if(qr->data[ppos] & 1)
			{
				painter.drawPoint(x, y);
			}
		}
	}

	painter.end();

	QRcode_free(qr);

	setMinimumSize(codeImage.size());
	update();
}

void QRCodeWidget::paintEvent(QPaintEvent *event)
{
	Q_UNUSED(event)

	QPainter painter(this);

	qreal scaleFactorX = width() / (qreal)codeImage.width();
	qreal scaleFactorY = height() / (qreal)codeImage.height();

	qreal scaleFactor = qMin(scaleFactorX, scaleFactorY);

	painter.scale(scaleFactor, scaleFactor);

	qreal x = (width() - (codeImage.width() * scaleFactor)) / (2.0 * scaleFactor);
	qreal y = (height() - (codeImage.height() * scaleFactor)) / (2.0 * scaleFactor);

	painter.drawPixmap(QPointF(x, y), codeImage);
}
