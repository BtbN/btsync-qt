#pragma once

#include <QWidget>
#include "ui_sharedfolders.h"

class BtsClient;
class BtsApi;

class SharedFoldersWidget : public QWidget, private Ui::SharedFoldersWidget
{
	Q_OBJECT

	public:
	SharedFoldersWidget(QWidget *parent = 0);

	void setClient(BtsClient *newclient);

	private slots:
	void updateTick();

	private:
	BtsClient *client;
	BtsApi *api;
};
