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
	~FolderInfoDialog();

	private slots:
	void changed();
	void onOkButton();
	void onApplyButton();

	private:
	BtsApiFolder *folderApi;
	QPushButton *applyButton;
};
