#include "preferenceswidget.h"


PreferencesWidget::PreferencesWidget(QWidget *parent)
	:QWidget(parent)
{
	setupUi(this);
}

void PreferencesWidget::setClient(BtsClient *newclient)
{
	client = newclient;
}
