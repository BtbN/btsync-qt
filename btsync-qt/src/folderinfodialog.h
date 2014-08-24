#pragma once

#include <QDialog>
#include "ui_folderinfo.h"

class BtsApi;
class BtsApiFolder;
struct BtsGetFoldersResult;
struct BtsGetFolderPeersResult;

class FolderInfoDialog : public QDialog, private Ui::FolderInfoDialog
{
	Q_OBJECT

	public:
	FolderInfoDialog(BtsApi *api, const QString &folderSecret, QWidget *parent = 0);

	private slots:
	void changed();
	void onCopySecret();
	void onCopyRoSecret();
	void onCopyEcSecret();
	void onOkButton();
	void onApplyButton();
	void updateQr();
	void onAddHost();
	void onDelHost();

	void updateSecrets(const QString &rw, const QString &ro, const QString &ec);
	void updateHosts(const QStringList &hosts);
	void updatePrefs(const QVariantHash &prefs);
	void updateName(const QVector<BtsGetFoldersResult> &results);
	void updatePeers(const QVector<BtsGetFolderPeersResult> &peers);

	private slots:
	void sendCurHosts();

	private:
	BtsApiFolder *folderApi;
	QPushButton *applyButton;
	QString roSecret;
	QString rwSecret;
	QString ecSecret;
	bool blockChanges;
	QString name;
};
