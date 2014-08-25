#include <QtDebug>

#include <QApplication>
#include <QClipboard>
#include <QDateTime>
#include <QRegExp>
#include <QTimer>

#include <bts_api.h>
#include <bts_apifolder.h>

#include "folderinfodialog.h"


FolderInfoDialog::FolderInfoDialog(BtsApi *api, const QString &folderSecret, QWidget *parent)
	:QDialog(parent)
	,blockChanges(false)
{
	setupUi(this);

	if(!api || folderSecret.isEmpty())
		qFatal("Invalid api/secret for FolderInfoDialog");

	folderApi = new BtsApiFolder(api, folderSecret, this);

	QPushButton *okButton = dialogButtons->button(QDialogButtonBox::Ok);
	applyButton = dialogButtons->button(QDialogButtonBox::Apply);

	applyButton->setEnabled(false);

	QTimer *updatePeersTimer = new QTimer(this);
	updatePeersTimer->setInterval(2000);
	updatePeersTimer->setSingleShot(false);
	updatePeersTimer->start();

	connect(updatePeersTimer, SIGNAL(timeout()), folderApi, SLOT(getFolderPeers()));

	connect(copySecretButton, SIGNAL(clicked()), this, SLOT(onCopySecret()));
	connect(copyRoSecretButton, SIGNAL(clicked()), this, SLOT(onCopyRoSecret()));
	connect(copyEcSecretButton, SIGNAL(clicked()), this, SLOT(onCopyEcSecret()));

	connect(okButton, SIGNAL(clicked()), this, SLOT(onOkButton()));
	connect(applyButton, SIGNAL(clicked()), this, SLOT(onApplyButton()));

	connect(readOnlyRadio, SIGNAL(clicked()), this, SLOT(updateQr()));
	connect(fullAccessRadio, SIGNAL(clicked()), this, SLOT(updateQr()));
	connect(ecOnlyRadio, SIGNAL(clicked()), this, SLOT(updateQr()));

	connect(relayCheck, SIGNAL(stateChanged(int)), this, SLOT(changed()));
	connect(trackerCheck, SIGNAL(stateChanged(int)), this, SLOT(changed()));
	connect(lanCheck, SIGNAL(stateChanged(int)), this, SLOT(changed()));
	connect(dhtCheck, SIGNAL(stateChanged(int)), this, SLOT(changed()));
	connect(archiveCheck, SIGNAL(stateChanged(int)), this, SLOT(changed()));
	connect(predefHostsCheck, SIGNAL(stateChanged(int)), this, SLOT(changed()));
	connect(overwriteCheck, SIGNAL(stateChanged(int)), this, SLOT(changed()));
	connect(selSyncCheck, SIGNAL(stateChanged(int)), this, SLOT(changed()));

	connect(addHostButton, SIGNAL(clicked()), this, SLOT(onAddHost()));
	connect(removeHostButton, SIGNAL(clicked()), this, SLOT(onDelHost()));

	connect(folderApi, SIGNAL(getSecretsResult(QString,QString,QString)), this, SLOT(updateSecrets(QString,QString,QString)));
	connect(folderApi, SIGNAL(getFolderHostsResult(QStringList,QString)), this, SLOT(updateHosts(QStringList)));
	connect(folderApi, SIGNAL(setFolderHostsResult(QStringList,QString)), this, SLOT(updateHosts(QStringList)));
	connect(folderApi, SIGNAL(getFolderPrefsResult(QVariantHash,QString)), this, SLOT(updatePrefs(QVariantHash)));
	connect(folderApi, SIGNAL(setFolderPrefsResult(QVariantHash,QString)), this, SLOT(updatePrefs(QVariantHash)));
	connect(folderApi, SIGNAL(getFoldersResult(QVector<BtsGetFoldersResult>,QString)), this, SLOT(updateName(QVector<BtsGetFoldersResult>)));
	connect(folderApi, SIGNAL(getFolderPeersResult(QVector<BtsGetFolderPeersResult>,QString)), this, SLOT(updatePeers(QVector<BtsGetFolderPeersResult>)));

	folderApi->getSecrets(true);
	folderApi->getFolderHosts();
	folderApi->getFolderPrefs();
	folderApi->getFolderPeers();
	folderApi->getFolders();
}

void FolderInfoDialog::onCopySecret()
{
	QApplication::clipboard()->setText(secretEdit->text());
}

void FolderInfoDialog::onCopyRoSecret()
{
	QApplication::clipboard()->setText(roSecretEdit->text());
}

void FolderInfoDialog::onCopyEcSecret()
{
	QApplication::clipboard()->setText(ecSecretEdit->text());
}

void FolderInfoDialog::onOkButton()
{
	if(applyButton->isEnabled())
		onApplyButton();

	accept();
}

void FolderInfoDialog::onApplyButton()
{
	QVariantHash prefs;
	prefs["use_relay_server"] = relayCheck->isChecked() ? 1 : 0;
	prefs["use_tracker"] = trackerCheck->isChecked() ? 1 : 0;
	prefs["search_lan"] = lanCheck->isChecked() ? 1 : 0;
	prefs["use_dht"] = dhtCheck->isChecked() ? 1 : 0;
	prefs["use_sync_trash"] = archiveCheck->isChecked() ? 1 : 0;
	prefs["use_hosts"] = predefHostsCheck->isChecked() ? 1 : 0;
	prefs["overwrite_changes"] = overwriteCheck->isChecked() ? 1 : 0;
	prefs["selective_sync"] = selSyncCheck->isChecked() ? 1 : 0;

	folderApi->setFolderPrefs(prefs);

	sendCurHosts();
}

void FolderInfoDialog::changed()
{
	if(blockChanges)
		return;

	applyButton->setEnabled(true);
}

void FolderInfoDialog::updateQr()
{
	QString secret = rwSecret;

	if(readOnlyRadio->isChecked())
		secret = roSecret;
	else if(ecOnlyRadio->isChecked())
		secret = ecSecret;

	QString qrString = QString("btsync://%1?n=%2").arg(secret).arg(name);

	qrCodeWidget->setText(qrString);
}

void FolderInfoDialog::onAddHost()
{
	QString txt = newHostEdit->text().trimmed();
	QRegExp hostMatch("^[\\.a-zA-Z0-9\\-\\_]+:[0-9]{1,5}$");

	if(!hostMatch.exactMatch(txt))
		return;

	hostsList->addItem(newHostEdit->text());
	newHostEdit->clear();

	changed();
}

void FolderInfoDialog::onDelHost()
{
	for(QListWidgetItem *item: hostsList->selectedItems())
	{
		delete item;
	}

	changed();
}

void FolderInfoDialog::sendCurHosts()
{
	QStringList hosts;

	for(int i = 0; i < hostsList->count(); ++i)
	{
		hosts << hostsList->item(i)->text();
	}

	folderApi->setFolderHosts(hosts);
}

void FolderInfoDialog::updateSecrets(const QString &rw, const QString &ro, const QString &ec)
{
	secretEdit->setText(rw);
	roSecretEdit->setText(ro);
	ecSecretEdit->setText(ec);

	roSecret = ro;
	rwSecret = rw;
	ecSecret = ec;

	if(!ec.isEmpty())
		ecOnlyRadio->setEnabled(true);

	updateQr();
}

void FolderInfoDialog::updateHosts(const QStringList &hosts)
{
	hostsList->clear();
	hostsList->addItems(hosts);
	hostsList->setEnabled(true);
	addHostButton->setEnabled(true);
	removeHostButton->setEnabled(true);
	newHostEdit->setEnabled(true);
}

void FolderInfoDialog::updatePrefs(const QVariantHash &prefs)
{
	relayCheck->setEnabled(true);
	trackerCheck->setEnabled(true);
	lanCheck->setEnabled(true);
	dhtCheck->setEnabled(true);
	archiveCheck->setEnabled(true);
	predefHostsCheck->setEnabled(true);
	overwriteCheck->setEnabled(true);
	selSyncCheck->setEnabled(true);
	applyButton->setEnabled(false);

	blockChanges = true;

	relayCheck->setChecked(prefs.value("use_relay_server").toInt() == 1);
	trackerCheck->setChecked(prefs.value("use_tracker").toInt() == 1);
	lanCheck->setChecked(prefs.value("search_lan").toInt() == 1);
	dhtCheck->setChecked(prefs.value("use_dht").toInt() == 1);
	archiveCheck->setChecked(prefs.value("use_sync_trash").toInt() == 1);
	predefHostsCheck->setChecked(prefs.value("use_hosts").toInt() == 1);
	overwriteCheck->setChecked(prefs.value("overwrite_changes").toInt() == 1);
	selSyncCheck->setChecked(prefs.value("selective_sync").toInt() == 1);

	blockChanges = false;
}

void FolderInfoDialog::updateName(const QVector<BtsGetFoldersResult> &results)
{
	if(results.isEmpty())
		return;

	const BtsGetFoldersResult &thisDir = results.first();

	QString dir = thisDir.dir;

	if(dir.startsWith("\\\\?\\"))
		dir = dir.mid(4);

	pathEdit->setText(dir);

	name = thisDir.dir.mid(thisDir.dir.lastIndexOf(QRegExp("[\\\\/]")) + 1);

	updateQr();
}

static QString convertSpeedToString(double speed)
{
	QString speedString = QObject::tr("%L1 Byte/s").arg(speed, 0, 'f', 0);

	if(speed >= 1000.0)
	{
		speed /= 1024.0;
		speedString = QObject::tr("%L1 kB/s").arg(speed, 0, 'f', 1);
	}

	if(speed >= 1000.0)
	{
		speed /= 1024.0;
		speedString = QObject::tr("%L1 MB/s").arg(speed, 0, 'f', 1);
	}

	return speedString;
}

void FolderInfoDialog::updatePeers(const QVector<BtsGetFolderPeersResult> &peers)
{
	if(peersTable->rowCount() != peers.size())
		peersTable->setRowCount(peers.size());

	int row = -1;
	for(const BtsGetFolderPeersResult &peer: peers)
	{
		row += 1;

		QTableWidgetItem *items[6];

		for(int i = 0; i < 6; ++i)
		{
			items[i] = peersTable->item(row, i);

			if(!items[i])
			{
				items[i] = new QTableWidgetItem();
				peersTable->setItem(row, i, items[i]);
			}
		}

		QTableWidgetItem *nameItem = items[0];
		QTableWidgetItem *idItem = items[1];
		QTableWidgetItem *conItem = items[2];
		QTableWidgetItem *syncItem = items[3];
		QTableWidgetItem *downItem = items[4];
		QTableWidgetItem *upItem = items[5];

		QDateTime time = QDateTime::fromTime_t(peer.synced);

		nameItem->setText(peer.name);
		idItem->setText(peer.id);
		conItem->setText(peer.connection);
		syncItem->setText(time.toString());
		downItem->setText(convertSpeedToString(peer.download));
		upItem->setText(convertSpeedToString(peer.upload));
	}
}
