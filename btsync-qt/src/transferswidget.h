#pragma once

#include <QWidget>
#include "ui_transfers.h"

class BtsClient;

class TransfersWidget : public QWidget, private Ui::TransfersWidget
{
	Q_OBJECT

	public:
	TransfersWidget(QWidget *parent = 0);

	void setClient(BtsClient *newclient);

	private:
	BtsClient *client;
};
