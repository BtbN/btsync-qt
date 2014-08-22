#include "deviceswidget.h"


DevicesWidget::DevicesWidget(QWidget *parent)
	:QWidget(parent)
{
	setupUi(this);
}

void DevicesWidget::setClient(BtsClient *newclient)
{
	client = newclient;
}
