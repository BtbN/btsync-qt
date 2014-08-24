#include <QFileDialog>

#include <bts_api.h>

#include "addfolderdialog.h"


AddFolderDialog::AddFolderDialog(BtsApi *api, QWidget *parent)
	:QDialog(parent)
	,api(api)
{
	if(!api)
		qFatal("Created AddFolderDialog with NULL api!");

	setupUi(this);

	connect(generateSecretButton, SIGNAL(clicked()), this, SLOT(genSecret()));
	connect(genEcButton, SIGNAL(clicked()), this, SLOT(genEcSecret()));
	connect(browseButton, SIGNAL(clicked()), this, SLOT(browsePath()));
}

QString AddFolderDialog::getPath()
{
	return folderEdit->text();
}

QString AddFolderDialog::getSecret()
{
	return secretEdit->text();
}

void AddFolderDialog::setPath(const QString &path)
{
	folderEdit->setText(path);
}

void AddFolderDialog::setSecret(const QString &secret)
{
	secretEdit->setText(secret);
}

void AddFolderDialog::genSecret()
{
	connect(api->getSecrets(false), SIGNAL(getSecretsResult(QString,QString,QString)), this, SLOT(genSecretResult(QString)));
}

void AddFolderDialog::genEcSecret()
{
	connect(api->getSecrets(true), SIGNAL(getSecretsResult(QString,QString,QString)), this, SLOT(genSecretResult(QString)));
}

void AddFolderDialog::browsePath()
{
	QString fileName = QFileDialog::getExistingDirectory(this, tr("Select sync folder"), folderEdit->text());

	if(fileName.isNull())
		return;

	folderEdit->setText(fileName);
}

void AddFolderDialog::genSecretResult(const QString &secret)
{
	secretEdit->setText(secret);
}
