#include <QtDebug>

#include <QTimer>

#include <bts_spawnclient.h>
#include <bts_api.h>

#include "mainwin.h"


struct MainWin_private
{
	BtsSpawnClient *spcl;
	BtsApi *api;
};

MainWin::MainWin(QWidget *parent)
	:QMainWindow(parent)
{
	setupUi(this);

	p = new MainWin_private();

	p->spcl = new BtsSpawnClient(this);
	p->api = new BtsApi(p->spcl, this);

	devicesTab->setClient(p->spcl);
	historyTab->setClient(p->spcl);
	prefsTab->setClient(p->spcl);
	sharedFoldersTab->setClient(p->spcl);
	transfersTab->setClient(p->spcl);

	connect(p->spcl, SIGNAL(clientStarted()), this, SLOT(clientReady()));
	connect(p->api, &BtsApi::error, [](const QString &error)
	{
		qDebug() << error;
	});

	QTimer::singleShot(0, p->spcl, SLOT(startClient()));
}

MainWin::~MainWin()
{
	delete p;
}

void MainWin::clientReady()
{
	qDebug() << "Client ready";
}
