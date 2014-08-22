#pragma once

#include <QWidget>
#include <QPixmap>

class QRCodeWidget : public QWidget
{
	Q_OBJECT

	public:
	QRCodeWidget(QWidget *parent = 0);

	public slots:
	void setText(const QString &text);

	protected:
	void paintEvent(QPaintEvent *event);

	private:
	QPixmap codeImage;
};
