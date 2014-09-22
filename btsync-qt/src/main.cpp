#include <QtDebug>

#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QSystemTrayIcon>
#include <QApplication>
#include <QMessageBox>
#include <QStringList>
#include <QFileInfo>
#include <QTime>
#include <QDir>

#include <iostream>
#include <memory>

#include <bts_global.h>
#include <bts_remoteclient.h>

#if defined(Q_OS_UNIX) || defined(Q_OS_LINUX) || defined(Q_OS_OSX)
#define HAVE_SIGNALHANDLER 1
#include "sighandler.h"
#endif

#include "mainwin.h"

static const char *builtinApiKey = "7VZ7OB5PSMAY6Q6MZH6VXJEAYMJZFA3F7PE2Y2JUUJK7TXZ67JUWIGAPNXXJBS2I3R7M"
                                   "UZQAR7KB3QPONKZ356BNKLZUZLH6ZBTOWAC5XNOJVYJXDIXHVKBNVN3WHKA6RWMKYCA";

static void displayHelp(const QCommandLineParser &parser)
{
#ifdef Q_OS_WIN
	QMessageBox::information(nullptr, qApp->applicationDisplayName(), parser.helpText());
#else
	std::cout << parser.helpText().toStdString() << std::endl;
#endif
}

static void displayVer(const QCommandLineParser &parser)
{
	Q_UNUSED(parser)

	QString verString = QString("%1 %2").arg(qApp->applicationDisplayName()).arg(qApp->applicationVersion());

#ifdef Q_OS_WIN
	QMessageBox::information(nullptr, qApp->applicationDisplayName(), verString);
#else
	std::cout << verString.toStdString() << std::endl;
#endif
}

int main(int argc, char *argv[])
{
	qsrand(QTime::currentTime().msec());

	QApplication app(argc, argv);
	app.setApplicationName("btsync-qt");
	app.setApplicationVersion("1.0");

	QCommandLineParser parser;
	parser.setApplicationDescription("Qt based btsync ui");

	QCommandLineOption helpOption = parser.addHelpOption();
	QCommandLineOption verOption = parser.addVersionOption();

	QCommandLineOption btsyncPathOption(QStringList() << "b" << "btpath", "Path to btsync binary", "btsyncpath");
	parser.addOption(btsyncPathOption);

	QCommandLineOption apikeyOption(QStringList() << "a" << "apikey", "BTSync developer api key", "apikey");
	parser.addOption(apikeyOption);

	QCommandLineOption systrayOption(QStringList() << "t" << "tray", "Minimize to systray on startup");
	parser.addOption(systrayOption);

	QCommandLineOption clientOption(QStringList() << "c" << "client", "Enable client mode, don't spawn btsync");
	parser.addOption(clientOption);

	QCommandLineOption hostOption(QStringList() << "H" << "host", "Client mode: btsync host", "host", "127.0.0.1");
	parser.addOption(hostOption);

	QCommandLineOption portOption(QStringList() << "p" << "port", "Client mode: btsync port", "port", "8080");
	parser.addOption(portOption);

	QCommandLineOption userOption(QStringList() << "u" << "user", "Client mode: btsync username", "user");
	parser.addOption(userOption);

	QCommandLineOption passOption(QStringList() << "P" << "pass", "Client mode: btsync password", "pass");
	parser.addOption(passOption);

	if(!parser.parse(app.arguments()))
	{
#ifdef Q_OS_WIN
		QMessageBox::critical(nullptr, qApp->applicationDisplayName(), parser.errorText());
#else
		std::cerr << parser.errorText().toStdString() << std::endl;
#endif
		return -1;
	}

	if(parser.isSet(helpOption))
	{
		displayHelp(parser);
		return 0;
	}

	if(parser.isSet(verOption))
	{
		displayVer(parser);
		return 0;
	}

	if(parser.isSet(btsyncPathOption))
	{
		QString path = parser.value(btsyncPathOption);

		QFileInfo finfo(path);

		if(!finfo.exists() || !finfo.isExecutable())
		{
			QString errString =  QObject::tr("ERROR: \"%1\" not found or not executable").arg(path);
#ifdef Q_OS_WIN
			QMessageBox::critical(nullptr, qApp->applicationDisplayName(), errString);
#else
			std::cerr << errString.toStdString() << std::endl;
#endif
			return -1;
		}

		BtsGlobal::setBtsyncExecutablePath(parser.value(btsyncPathOption));
	}
	else
	{
		QDir::setCurrent(app.applicationDirPath());
	}

	if(parser.isSet(apikeyOption))
	{
		QString apikey = parser.value(apikeyOption);

		try
		{
			BtsGlobal::setApiKey(apikey);
		}
		catch(BtsException&)
		{
			QString errString = QObject::tr("Supplied APIKey \"%1\" is invalid!").arg(apikey);
#ifdef Q_OS_WIN
			QMessageBox::critical(nullptr, qApp->applicationDisplayName(), errString);
#else
			std::cerr << errString.toStdString() << std::endl;
#endif
			return -1;
		}
	}
	else
	{
		BtsGlobal::setApiKey(builtinApiKey);
	}

#ifdef HAVE_SIGNALHANDLER
	SignalHandler sigHandler;
	QObject::connect(&sigHandler, SIGNAL(sigQuitApp()), &app, SLOT(quit()));
#endif

	std::unique_ptr<MainWin> win;

	if(parser.isSet(clientOption))
	{
		QString host = parser.value(hostOption);
		int port = parser.value(portOption).toInt();

		BtsRemoteClient *client = new BtsRemoteClient(host, port);

		if(parser.isSet(userOption))
			client->setUserName(parser.value(userOption));

		if(parser.isSet(passOption))
			client->setPassword(parser.value(passOption));

		win = std::unique_ptr<MainWin>(new MainWin(client));
	}
	else
	{
		win = std::unique_ptr<MainWin>(new MainWin());
	}

	if(!parser.isSet(systrayOption))
		win->show();

	return app.exec();
}
