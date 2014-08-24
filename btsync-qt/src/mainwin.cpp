#include <QtDebug>

#include <QStatusBar>
#include <QLabel>
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

	speedLabel = new QLabel(this);
	statusBar()->addPermanentWidget(speedLabel);

	api = new BtsApi(spcl, this);

	connect(spcl, SIGNAL(clientStarted()), this, SLOT(clientReady()));

	speedTimer = new QTimer(this);
	speedTimer->setInterval(1000);

	connect(speedTimer, SIGNAL(timeout()), api, SLOT(getSpeed()));
	connect(api, SIGNAL(getSpeedResult(qint64,qint64)), this, SLOT(updateSpeed(qint64,qint64)));

	QTimer::singleShot(0, spcl, SLOT(startClient()));
}

void MainWin::clientReady()
{
	speedTimer->start();
	api->getSpeed();
}

void MainWin::updateSpeed(qint64 down, qint64 up)
{
	speedLabel->setText(tr("%1 kB/s down | %2 kB/s up").arg(down / 1024.0, 0, 'f', 1).arg(up / 1024.0, 0, 'f', 1));
}
