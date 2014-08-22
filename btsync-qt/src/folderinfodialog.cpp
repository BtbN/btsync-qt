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

	connect(okButton, SIGNAL(clicked()), this, SLOT(onOkButton()));
	connect(applyButton, SIGNAL(clicked()), this, SLOT(onApplyButton()));
}

FolderInfoDialog::~FolderInfoDialog()
{
	qDebug("Deleted");
}

void FolderInfoDialog::changed()
{
	applyButton->setEnabled(true);
}

void FolderInfoDialog::onOkButton()
{
	onApplyButton();
	accept();
}

void FolderInfoDialog::onApplyButton()
{

}
