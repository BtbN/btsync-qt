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
	qDebug() << "Client ready, getting folders!";

	//p->api->getFolders();

	//p->api->addFolder("D:/Test");
	//p->api->getFiles("A5NR3VPU5W6K3KQN6L5WJROVXVNAMGYTT");
	//p->api->setFilePrefs("A5NR3VPU5W6K3KQN6L5WJROVXVNAMGYTT", "test1/Neues Textdokument.txt", false);

	p->api->setFolderHosts("A5NR3VPU5W6K3KQN6L5WJROVXVNAMGYTT", QStringList());
	p->api->getFolderHosts("A5NR3VPU5W6K3KQN6L5WJROVXVNAMGYTT");

	QTimer::singleShot(5000, this, SLOT(afterTimer()));
}

void MainWin::afterTimer()
{
	p->api->getFolderPeers("A5NR3VPU5W6K3KQN6L5WJROVXVNAMGYTT");
}
