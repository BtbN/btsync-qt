#pragma once

#include <QWidget>
#include <QVector>
#include "ui_sharedfolders.h"

#include <bts_api.h>

class QTableWidgetItem;
class BtsClient;
class BtsApi;

class SharedFoldersWidget : public QWidget, private Ui::SharedFoldersWidget
{
	Q_OBJECT

	public:
	SharedFoldersWidget(QWidget *parent = 0);

	void setClient(BtsClient *newclient);

	private slots:
	void addFolder();
	void removeFolder();
	void folderInfo();
	void updateTick();
	void updateFolders(const QVector<BtsGetFoldersResult> result);
	void itemDoubleClicked(QTableWidgetItem *item);
	void showInfoDialog(const QString &secret);

	signals:
	void error(const QString &error);

	private:
	BtsClient *client;
	BtsApi *api;
};
