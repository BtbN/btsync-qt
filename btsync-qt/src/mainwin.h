#pragma once

#include <QMainWindow>
#include "ui_mainwin.h"

class QSystemTrayIcon;
class BtsClient;
class BtsApi;
class QLabel;
class QTimer;

class MainWin : public QMainWindow, private Ui::MainWin
{
	Q_OBJECT

	public:
	MainWin(BtsClient *client = nullptr, QWidget *parent = nullptr);

	private slots:
	void clientReady();
	void updateSpeed(qint64 down, qint64 up);
	void displayError(const QString &error);

	protected:
	void closeEvent(QCloseEvent *event);

	private:
	BtsClient *client;
	QSystemTrayIcon *systray;
	QTimer *speedTimer;
	QLabel *speedLabel;
	BtsApi *api;
};
