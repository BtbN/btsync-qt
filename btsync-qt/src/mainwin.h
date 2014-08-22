#pragma once

#include <QMainWindow>
#include "ui_mainwin.h"

struct MainWin_private;

class MainWin : public QMainWindow, private Ui::MainWin
{
	Q_OBJECT

	public:
	MainWin(QWidget *parent = 0);
	~MainWin();

	private slots:
	void clientReady();

	private:
	MainWin_private *p;
};
