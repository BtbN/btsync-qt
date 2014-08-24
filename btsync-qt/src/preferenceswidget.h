#pragma once

#include <QVariantHash>
#include <QWidget>
#include "ui_preferences.h"

class BtsClient;
class BtsApi;

class PreferencesWidget : public QWidget, private Ui::PreferencesWidget
{
	Q_OBJECT

	public:
	PreferencesWidget(QWidget *parent = 0);

	void setClient(BtsClient *newclient);

	private slots:
	void clientReady();
	void prefsResult(const QVariantHash &prefs);
	void stuffChanged();
	void commitChanges();

	private:
	QVariantHash changedSettings;
	BtsClient *client;
	BtsApi *api;
};
