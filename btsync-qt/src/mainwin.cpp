#include <QtDebug>

#include <QTimer>

#include <bts_spawnclient.h>
#include <bts_api.h>

#include "mainwin.h"


MainWin::MainWin(QWidget *parent)
	:QMainWindow(parent)
{
	setupUi(this);

	spcl = new BtsSpawnClient(this);

	devicesTab->setClient(spcl);
	historyTab->setClient(spcl);
	prefsTab->setClient(spcl);
	sharedFoldersTab->setClient(spcl);
	transfersTab->setClient(spcl);

	connect(spcl, SIGNAL(clientStarted()), this, SLOT(clientReady()));

	QTimer::singleShot(0, spcl, SLOT(startClient()));
}

void MainWin::clientReady()
{
	qDebug() << "Client ready";
}
