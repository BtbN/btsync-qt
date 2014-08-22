#include "sharedfolderswidget.h"


SharedFoldersWidget::SharedFoldersWidget(QWidget *parent)
	:QWidget(parent)
{
	setupUi(this);
}

void SharedFoldersWidget::setClient(BtsClient *newclient)
{
	client = newclient;
}
