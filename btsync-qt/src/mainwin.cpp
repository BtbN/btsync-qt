#include <QtDebug>

#include <QSystemTrayIcon>
#include <QCloseEvent>
#include <QStatusBar>
#include <QAction>
#include <QLabel>
#include <QTimer>
#include <QMenu>
#include <QIcon>

#include <bts_spawnclient.h>
#include <bts_api.h>

#include "mainwin.h"
#include "utils.h"


MainWin::MainWin(QWidget *parent)
	:QMainWindow(parent)
{
	setupUi(this);

	setWindowIcon(QIcon(":/icons/btsync.png"));

	if(!QSystemTrayIcon::isSystemTrayAvailable())
	{
		qWarning("System tray not available!");
	}

	systray = new QSystemTrayIcon(QIcon(":/icons/btsync-tray.ico"), this);

	QMenu *sysMenu = new QMenu(this);
	QAction *showSysAction = sysMenu->addAction(tr("Show"));
	sysMenu->addSeparator();
	QAction *exitSysAction = sysMenu->addAction(tr("Exit"));
	systray->setContextMenu(sysMenu);

	systray->setToolTip(tr("BTSync-Qt"));

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

	connect(api, SIGNAL(error(QString)), this, SLOT(displayError(QString)));
	connect(devicesTab, SIGNAL(error(QString)), this, SLOT(displayError(QString)));
	connect(historyTab, SIGNAL(error(QString)), this, SLOT(displayError(QString)));
	connect(prefsTab, SIGNAL(error(QString)), this, SLOT(displayError(QString)));
	connect(sharedFoldersTab, SIGNAL(error(QString)), this, SLOT(displayError(QString)));
	connect(transfersTab, SIGNAL(error(QString)), this, SLOT(displayError(QString)));

	connect(systray, &QSystemTrayIcon::activated, [this](QSystemTrayIcon::ActivationReason reason)
	{
		if(reason == QSystemTrayIcon::DoubleClick)
		{
			show();
			activateWindow();
		}
	});

	connect(showSysAction, &QAction::triggered, [this]()
	{
		show();
		activateWindow();
	});

	connect(actionExit, SIGNAL(triggered()), qApp, SLOT(quit()));
	connect(exitSysAction, SIGNAL(triggered()), qApp, SLOT(quit()));

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

void MainWin::displayError(const QString &error)
{
	statusBar()->showMessage(error, 30000);
}

void MainWin::closeEvent(QCloseEvent *event)
{
	if(QSystemTrayIcon::isSystemTrayAvailable() && systray->isVisible())
	{
		systray->showMessage(tr("BTSync-Qt"), tr("BTSync-Qt was minimized into the system tray."), QSystemTrayIcon::Information, 2500);
		event->ignore();
		hide();
	}
	else
	{
		event->accept();
	}
}
