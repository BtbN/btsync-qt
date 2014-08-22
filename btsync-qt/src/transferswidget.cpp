#include "transferswidget.h"


TransfersWidget::TransfersWidget(QWidget *parent)
	:QWidget(parent)
{
	setupUi(this);
}

void TransfersWidget::setClient(BtsClient *newclient)
{
	client = newclient;
}
