#include <bts_api.h>
#include <bts_client.h>

#include "sharedfolderswidget.h"


SharedFoldersWidget::SharedFoldersWidget(QWidget *parent)
	:QWidget(parent)
	,client(nullptr)
	,api(nullptr)
{
	setupUi(this);
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
}
