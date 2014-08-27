#pragma once

#include <QWidget>
#include "ui_devices.h"

#include <bts_api.h>

struct BtsGetFoldersResult;
class BtsClient;

class DevicesWidget : public QWidget, private Ui::DevicesWidget
{
	Q_OBJECT

	public:
	DevicesWidget(QWidget *parent = 0);

	void setClient(BtsClient *newclient);

	private slots:
	void updateTable();
	void getFoldersRes(const QVector<BtsGetFoldersResult> &folders);
	void fillTable();

	signals:
	void error(const QString &error);

	private:
	BtsClient *client;
	BtsApi *api;
	QHash<QString, QVector<BtsGetFolderPeersResult> > folderPeersHash;
};
