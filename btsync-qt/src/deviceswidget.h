#pragma once

#include <QWidget>
#include "ui_devices.h"

class DevicesWidget : public QWidget, private Ui::DevicesWidget
{
	Q_OBJECT

	public:
	DevicesWidget(QWidget *parent = 0);
};
