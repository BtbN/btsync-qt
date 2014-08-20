#include <QTimer>

#include <bts_spawnclient.h>

#include "ui_devices.h"
#include "ui_sharedFolders.h"
#include "ui_transfers.h"
#include "ui_history.h"
#include "ui_preferences.h"

#include "mainwin.h"


struct MainWin_private
{
	Ui::devicesWidget devicesUi;
	Ui::sharedFoldersWidget sharedFoldersUi;
	Ui::transfersWidget transfersUi;
	Ui::historyWidget historyUi;
	Ui::preferencesWidget preferencesUi;

	BtsSpawnClient *spcl;
};

MainWin::MainWin(QWidget *parent)
	:QMainWindow(parent)
{
	setupUi(this);

	p = new MainWin_private();

	p->devicesUi.setupUi(devicesTab);
	p->sharedFoldersUi.setupUi(sharedFoldersTab);
	p->transfersUi.setupUi(transfersTab);
	p->historyUi.setupUi(historyTab);
	p->preferencesUi.setupUi(prefsTab);

	p->spcl = new BtsSpawnClient(this);

	QTimer::singleShot(0, p->spcl, SLOT(startClient()));
}

MainWin::~MainWin()
{
	delete p;
}
