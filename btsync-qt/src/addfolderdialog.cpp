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

	genSecretUuid = QUuid::createUuid();

	connect(generateSecretButton, SIGNAL(clicked()), this, SLOT(genSecret()));
	connect(browseButton, SIGNAL(clicked()), this, SLOT(browsePath()));

	connect(api, SIGNAL(getSecretsResultUuid(QUuid,QString,QString,QString)), this, SLOT(genSecretResult(QUuid,QString)));
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
	api->getSecrets(genSecretUuid);
}

void AddFolderDialog::browsePath()
{
	QString fileName = QFileDialog::getExistingDirectory(this, tr("Select sync folder"), folderEdit->text());

	if(fileName.isNull())
		return;

	folderEdit->setText(fileName);
}

void AddFolderDialog::genSecretResult(const QUuid &uuid, const QString &secret)
{
	if(uuid != genSecretUuid)
		return;

	secretEdit->setText(secret);
}
