#include <QtDebug>

#include <QTableWidgetItem>
#include <QDateTime>
#include <QTimer>

#include <bts_api.h>
#include <bts_client.h>

#include "deviceswidget.h"
#include "utils.h"


DevicesWidget::DevicesWidget(QWidget *parent)
	:QWidget(parent)
	,client(nullptr)
	,api(nullptr)
{
	setupUi(this);

	QTimer *updateTimer = new QTimer(this);
	updateTimer->setInterval(2000);
	updateTimer->setSingleShot(false);

	connect(updateTimer, SIGNAL(timeout()), this, SLOT(updateTable()));

	updateTimer->start();
}

void DevicesWidget::setClient(BtsClient *newclient)
{
	if(client)
	{
		api->disconnect(this);
		api->deleteLater();
		api = nullptr;
	}

	client = newclient;

	api = new BtsApi(client, this);

	connect(client, SIGNAL(clientStarted()), this, SLOT(updateTable()));
	connect(api, SIGNAL(error(QString)), this, SIGNAL(error(QString)));

	if(client->isClientReady())
		updateTable();
}

void DevicesWidget::updateTable()
{
	if(!api || !client || !client->isClientReady())
		return;

	connect(api->getFolders(), &BtsApiNotifier::getFoldersResult, this, &DevicesWidget::getFoldersRes);
}

void DevicesWidget::getFoldersRes(const QVector<BtsGetFoldersResult> &folders)
{
	QStringList currentFolders;

	int row = -1;
	for(const BtsGetFoldersResult &folder: folders)
	{
		row += 1;

		QString dir = folder.dir;

		currentFolders << dir;

		connect(api->getFolderPeers(folder.secret), &BtsApiNotifier::getFolderPeersResult,
		        [this, dir](const QVector<BtsGetFolderPeersResult> &result)
		{
			folderPeersHash[dir] = result;
			fillTable();
		});
	}

	bool updated = false;
	for(const QString &oldFolder: folderPeersHash.keys())
	{
		if(!currentFolders.contains(oldFolder))
		{
			folderPeersHash.remove(oldFolder);
			updated = true;
		}
	}

	if(updated)
	{
		fillTable();
	}
}

void DevicesWidget::fillTable()
{
	bool sort = devicesTable->isSortingEnabled();
	devicesTable->setSortingEnabled(false);

	int row = -1;
	for(auto it = folderPeersHash.constBegin(); it != folderPeersHash.constEnd(); ++it)
	{
		QString dir = it.key();

		for(const BtsGetFolderPeersResult &peer: it.value())
		{
			row += 1;

			if(devicesTable->rowCount() < row + 1)
				devicesTable->setRowCount(row + 1);

			QTableWidgetItem *items[6];

			for(int i = 0; i < 6; ++i)
			{
				items[i] = devicesTable->item(row, i);

				if(!items[i])
				{
					items[i] = new QTableWidgetItem();
					devicesTable->setItem(row, i, items[i]);
				}
			}

			QTableWidgetItem *deviceItem = items[0];
			QTableWidgetItem *folderItem = items[1];
			QTableWidgetItem *conItem = items[2];
			QTableWidgetItem *syncItem = items[3];
			QTableWidgetItem *downItem = items[4];
			QTableWidgetItem *upItem = items[5];

			QString dirName = dir;

			if(dirName.startsWith("\\\\?\\"))
				dirName = dir.mid(4);

			QDateTime time = QDateTime::fromTime_t(peer.synced);

			QString downString = byteCountToString(peer.download, true, false);
			QString upString = byteCountToString(peer.upload, true, false);

			deviceItem->setText(peer.name);
			folderItem->setText(dirName);
			conItem->setText(peer.connection);
			syncItem->setText(time.toString());
			downItem->setText(downString);
			upItem->setText(upString);
		}
	}

	if(devicesTable->rowCount() != row + 1)
		devicesTable->setRowCount(row + 1);

	devicesTable->setSortingEnabled(sort);
}
