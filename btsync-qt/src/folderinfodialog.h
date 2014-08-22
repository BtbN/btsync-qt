#pragma once

#include <QDialog>
#include "ui_folderinfo.h"

class BtsApi;
class BtsApiFolder;

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

	void updateSecrets(const QString &rw, const QString &ro, const QString &ec);

	private:
	BtsApiFolder *folderApi;
	QPushButton *applyButton;
	QString roSecret;
	QString rwSecret;
};
