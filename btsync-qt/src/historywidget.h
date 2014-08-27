#pragma once

#include <QWidget>
#include "ui_history.h"

class BtsClient;

class HistoryWidget : public QWidget, private Ui::HistoryWidget
{
	Q_OBJECT

	public:
	HistoryWidget(QWidget *parent = 0);

	void setClient(BtsClient *newclient);

	signals:
	void error(const QString &error);

	private:
	BtsClient *client;
};
