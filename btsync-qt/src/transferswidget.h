#pragma once

#include <QWidget>
#include "ui_transfers.h"

class TransfersWidget : public QWidget, private Ui::TransfersWidget
{
	Q_OBJECT

	public:
	TransfersWidget(QWidget *parent = 0);
};
