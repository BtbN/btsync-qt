#pragma once

#include <QWidget>
#include "ui_sharedfolders.h"

class SharedFoldersWidget : public QWidget, private Ui::SharedFoldersWidget
{
	Q_OBJECT

	public:
	SharedFoldersWidget(QWidget *parent = 0);
};
