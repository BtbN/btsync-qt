#include <QtDebug>

#include <QSystemTrayIcon>
#include <QCloseEvent>
#include <QStatusBar>
#include <QLabel>
#include <QTimer>
#include <QIcon>

#include <bts_spawnclient.h>
#include <bts_api.h>

#include "mainwin.h"
#include "utils.h"


MainWin::MainWin(QWidget *parent)
	:QMainWindow(parent)
{
	setupUi(this);

	QIcon btsIcon(":/icons/btsync.png");

	setWindowIcon(btsIcon);

	systray = new QSystemTrayIcon(btsIcon, this);
	systray->show();

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

	connect(systray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(show()));

	QTimer::singleShot(0, spcl, SLOT(startClient()));
}

void MainWin::clientReady()
{
	speedTimer->start();
	api->getSpeed();
}

void MainWin::updateSpeed(qint64 down, qint64 up)
{
	speedLabel->setText(tr("%1 down | %2 up")
	                    .arg(byteCountToString(down, false, true))
	                    .arg(byteCountToString(up, false, true)));
}

void MainWin::closeEvent(QCloseEvent *event)
{
	if(QSystemTrayIcon::isSystemTrayAvailable() && systray->isVisible())
	{
		systray->showMessage(tr("BTSync-Qt"), tr("BTSync-Qt was closed into the system tray.\nUse File->Exit to quit the application."));
		event->ignore();
		hide();
	}
	else
	{
		event->accept();
	}
}
