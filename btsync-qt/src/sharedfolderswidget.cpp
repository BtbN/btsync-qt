#include <QTimer>

#include <bts_api.h>
#include <bts_client.h>

#include "sharedfolderswidget.h"


SharedFoldersWidget::SharedFoldersWidget(QWidget *parent)
	:QWidget(parent)
	,client(nullptr)
	,api(nullptr)
{
	setupUi(this);

	QTimer *updateTimer = new QTimer(this);
	updateTimer->setInterval(10000);
	updateTimer->start();

	connect(updateTimer, SIGNAL(timeout()), this, SLOT(updateTick()));

	connect(addButton, SIGNAL(clicked()), this, SLOT(addFolder()));
	connect(removeButton, SIGNAL(clicked()), this, SLOT(removeFolder()));
}

void SharedFoldersWidget::setClient(BtsClient *newclient)
{
	if(client)
	{
		api->disconnect(this);
		api->deleteLater();

		client = nullptr;
		api = nullptr;
	}

	client = newclient;

	api = new BtsApi(client, this);

	//connect(api, SIGNAL(getFoldersResult(QVector<BtsGetFoldersResult>))
}

void SharedFoldersWidget::addFolder()
{

}

void SharedFoldersWidget::removeFolder()
{

}

void SharedFoldersWidget::updateTick()
{
	if(!api || !client || !client->isClientReady())
		return;

	api->getFolders();
}
