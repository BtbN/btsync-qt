#pragma once

#include <QMainWindow>
#include "ui_mainwin.h"

class BtsSpawnClient;

class MainWin : public QMainWindow, private Ui::MainWin
{
	Q_OBJECT

	public:
	MainWin(QWidget *parent = 0);

	private slots:
	void clientReady();

	private:
	BtsSpawnClient *spcl;
};
