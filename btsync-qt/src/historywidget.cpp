#include "historywidget.h"


HistoryWidget::HistoryWidget(QWidget *parent)
	:QWidget(parent)
{
	setupUi(this);
}

void HistoryWidget::setClient(BtsClient *newclient)
{
	client = newclient;
}
