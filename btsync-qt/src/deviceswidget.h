#pragma once

#include <QWidget>
#include "ui_devices.h"

class BtsClient;

class DevicesWidget : public QWidget, private Ui::DevicesWidget
{
	Q_OBJECT

	public:
	DevicesWidget(QWidget *parent = 0);

	void setClient(BtsClient *newclient);

	private:
	BtsClient *client;
};
