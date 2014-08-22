#pragma once

#include <QMainWindow>
#include "ui_mainwin.h"

class BtsSpawnClient;
class BtsApi;
class QLabel;
class QTimer;

class MainWin : public QMainWindow, private Ui::MainWin
{
	Q_OBJECT

	public:
	MainWin(QWidget *parent = 0);

	private slots:
	void clientReady();
	void updateSpeed(qint64 down, qint64 up);

	private:
	BtsSpawnClient *spcl;
	QTimer *speedTimer;
	QLabel *speedLabel;
	BtsApi *api;

};
