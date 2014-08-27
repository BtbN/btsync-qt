#include <QtDebug>

#include <QIntValidator>
#include <QPushButton>
#include <QGridLayout>
#include <QStringList>
#include <QLineEdit>
#include <QCheckBox>
#include <QLabel>

#include <bts_api.h>
#include <bts_client.h>

#include "preferenceswidget.h"

static const QStringList knownBooleanOptions =
{
	"lan_use_tcp",
	"rate_limit_local_peers",
	"use_upnp",
	"lan_encrypt_data",
	"disk_low_priority"
};

static const QStringList knownNumberOptions =
{
	"external_port",
	"max_file_size_diff_for_patching",
	"sync_trash_ttl",
	"upload_limit",
	"send_buf_size",
	"recv_buf_size",
	"log_size",
	"download_limit",
	"max_file_size_for_versioning",
	"folder_rescan_interval",
	"peer_expiration_days",
	"sync_max_time_diff",
	"listening_port"
};


PreferencesWidget::PreferencesWidget(QWidget *parent)
	:QWidget(parent)
	,client(nullptr)
	,api(nullptr)
{
	setupUi(this);

	QPushButton *saveButton = buttonBox->button(QDialogButtonBox::Save);
	connect(saveButton, SIGNAL(clicked()), this, SLOT(commitChanges()));
}

void PreferencesWidget::setClient(BtsClient *newclient)
{
	if(client)
	{
		api->disconnect(this);
		api->deleteLater();
		api = nullptr;
	}

	client = newclient;

	api = new BtsApi(client, this);

	connect(client, SIGNAL(clientStarted()), this, SLOT(clientReady()));

	connect(api, SIGNAL(error(QString)), this, SIGNAL(error(QString)));

	connect(api, SIGNAL(getPreferencesResult(QVariantHash)), this, SLOT(prefsResult(QVariantHash)));
	connect(api, SIGNAL(setPreferencesResult(QVariantHash)), this, SLOT(prefsResult(QVariantHash)));

	QPushButton *resetButton = buttonBox->button(QDialogButtonBox::Reset);
	connect(resetButton, SIGNAL(clicked()), api, SLOT(getPreferences()));

	if(client->isClientReady())
		clientReady();
}

void PreferencesWidget::clientReady()
{
	api->getPreferences();
}

void PreferencesWidget::prefsResult(const QVariantHash &prefs)
{
	QWidget *widget = scrollArea->takeWidget();
	if(widget)
		widget->deleteLater();

	widget = new QWidget(this);
	QGridLayout *layout = new QGridLayout(widget);

	int row = 0;
	QStringList keys = prefs.keys();
	keys.sort(Qt::CaseInsensitive);

	for(QStringList::const_iterator it = keys.constBegin(); it != keys.constEnd(); ++it, ++row)
	{
		QString key = *it;
		QVariant value = prefs[key];

		QLabel *titleLabel = new QLabel(widget);
		titleLabel->setText(key);

		layout->addWidget(titleLabel, row, 0);

		if(knownBooleanOptions.contains(key)
		        || (QMetaType::Type)value.type() == QMetaType::Bool)
		{
			QCheckBox *check = new QCheckBox(widget);
			layout->addWidget(check, row, 1);

			bool isChecked = value.toBool();

			check->setChecked(isChecked);

			connect(check, &QCheckBox::stateChanged, [this, key](int state)
			{
				changedSettings[key] = state ? 1 : 0;
				stuffChanged();
			});
		}
		else
		{
			QLineEdit *edit = new QLineEdit(widget);
			layout->addWidget(edit, row, 1);

			if(knownNumberOptions.contains(key)
			        || (QMetaType::Type)value.type() == QMetaType::Double
			        || (QMetaType::Type)value.type() == QMetaType::Float
			        || (QMetaType::Type)value.type() == QMetaType::Int
			        || (QMetaType::Type)value.type() == QMetaType::UInt)
			{
				edit->setValidator(new QIntValidator(widget));
				edit->setText(QString("%1").arg(value.toLongLong()));
			}
			else
			{
				edit->setText(value.toString());
			}

			connect(edit, &QLineEdit::textEdited, [this, key](const QString &text)
			{
				changedSettings[key] = text;
				stuffChanged();
			});
		}
	}

	scrollArea->setWidget(widget);
	widget->show();
	buttonBox->setEnabled(false);
	changedSettings.clear();
}

void PreferencesWidget::stuffChanged()
{
	buttonBox->setEnabled(true);
}

void PreferencesWidget::commitChanges()
{
	api->setPreferences(changedSettings);
}
