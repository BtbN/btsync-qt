#include <QApplication>
#include <QClipboard>

#include <bts_api.h>
#include <bts_apifolder.h>

#include "folderinfodialog.h"


FolderInfoDialog::FolderInfoDialog(BtsApi *api, const QString &folderSecret, QWidget *parent)
	:QDialog(parent)
{
	setupUi(this);

	if(!api || folderSecret.isEmpty())
		qFatal("Invalid api/secret for FolderInfoDialog");

	folderApi = new BtsApiFolder(api, folderSecret, this);

	QPushButton *okButton = dialogButtons->button(QDialogButtonBox::Ok);
	applyButton = dialogButtons->button(QDialogButtonBox::Apply);

	applyButton->setEnabled(false);

	connect(copySecretButton, SIGNAL(clicked()), this, SLOT(onCopySecret()));
	connect(copyRoSecretButton, SIGNAL(clicked()), this, SLOT(onCopyRoSecret()));
	connect(copyEcSecretButton, SIGNAL(clicked()), this, SLOT(onCopyEcSecret()));

	connect(okButton, SIGNAL(clicked()), this, SLOT(onOkButton()));
	connect(applyButton, SIGNAL(clicked()), this, SLOT(onApplyButton()));

	connect(readOnlyRadio, SIGNAL(clicked()), this, SLOT(updateQr()));
	connect(fullAccessRadio, SIGNAL(clicked()), this, SLOT(updateQr()));

	connect(folderApi, SIGNAL(getSecretsResult(QString,QString,QString)), this, SLOT(updateSecrets(QString,QString,QString)));

	folderApi->getSecrets(true);
}

void FolderInfoDialog::changed()
{
	applyButton->setEnabled(true);
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
	onApplyButton();
	accept();
}

void FolderInfoDialog::onApplyButton()
{

}

void FolderInfoDialog::updateQr()
{
	QString secret = rwSecret;

	if(readOnlyRadio->isChecked())
		secret = roSecret;

	QString qrString = QString("btsync://%1").arg(secret);

	qrCodeWidget->setText(qrString);
}

void FolderInfoDialog::updateSecrets(const QString &rw, const QString &ro, const QString &ec)
{
	secretEdit->setText(rw);
	roSecretEdit->setText(ro);
	ecSecretEdit->setText(ec);

	roSecret = ro;
	rwSecret = rw;

	updateQr();
}
