#pragma once

#include <QWidget>
#include "ui_preferences.h"

class BtsClient;

class PreferencesWidget : public QWidget, private Ui::PreferencesWidget
{
	Q_OBJECT

	public:
	PreferencesWidget(QWidget *parent = 0);

	void setClient(BtsClient *newclient);

	private:
	BtsClient *client;
};
