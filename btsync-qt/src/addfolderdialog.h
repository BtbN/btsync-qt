#pragma once

#include <QDialog>
#include "ui_addfolderdialog.h"

class BtsApi;

class AddFolderDialog : public QDialog, private Ui::AddFolderDialog
{
	Q_OBJECT

	public:
	AddFolderDialog(BtsApi *api, QWidget *parent = 0);

	QString getPath();
	QString getSecret();

	void setPath(const QString &path);
	void setSecret(const QString &secret);

	private slots:
	void genSecret();
	void genEcSecret();
	void browsePath();
	void genSecretResult(const QString &secret);

	private:
	BtsApi *api;
};
