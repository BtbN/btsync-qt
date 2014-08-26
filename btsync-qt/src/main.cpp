#include <QtDebug>

#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QSystemTrayIcon>
#include <QApplication>
#include <QStringList>
#include <QFileInfo>
#include <QTime>

#include <iostream>

#include <bts_global.h>

#if defined(Q_OS_UNIX) || defined(Q_OS_LINUX) || defined(Q_OS_OSX)
#define HAVE_SIGNALHANDLER 1
#include "sighandler.h"
#endif

#include "mainwin.h"

static const char *builtinApiKey = "7VZ7OB5PSMAY6Q6MZH6VXJEAYMJZFA3F7PE2Y2JUUJK7TXZ67JUWIGAPNXXJBS2I3R7M"
                                   "UZQAR7KB3QPONKZ356BNKLZUZLH6ZBTOWAC5XNOJVYJXDIXHVKBNVN3WHKA6RWMKYCA";

static void displayHelp(const QCommandLineParser &parser)
{
	std::cout << parser.helpText().toStdString() << std::endl;
}

static void displayVer(const QCommandLineParser &parser)
{
	Q_UNUSED(parser)

	std::cout << qApp->applicationName().toStdString() << " " << qApp->applicationVersion().toStdString() << std::endl;
}

int main(int argc, char *argv[])
{
	qsrand(QTime::currentTime().msec());

	QApplication app(argc, argv);
	app.setApplicationName("btsync-qt");
	app.setApplicationVersion("0.1");

	QCommandLineParser parser;
	parser.setApplicationDescription("Qt based btsync ui");

	QCommandLineOption helpOption = parser.addHelpOption();
	QCommandLineOption verOption = parser.addVersionOption();

	QCommandLineOption btsyncPathOption(QStringList() << "b" << "btpath", "Path to btsync binary", "btsyncpath");
	parser.addOption(btsyncPathOption);

	QCommandLineOption apikeyOption(QStringList() << "a" << "apikey", "BTSync developer api key", "apikey");
	parser.addOption(apikeyOption);

	QCommandLineOption systrayOption(QStringList() << "s" << "show", "Show main window on startup instead of minimizing to the system tray");

	if(QSystemTrayIcon::isSystemTrayAvailable())
		parser.addOption(systrayOption);

	parser.process(app);

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
			std::cerr << QString("ERROR: \"%1\" not found or nor executable").arg(path).toStdString() << std::endl;
			return -1;
		}

		BtsGlobal::setBtsyncExecutablePath(parser.value(btsyncPathOption));
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
			std::cerr << QString("Supplied APIKey \"%1\" is invalid!").arg(apikey).toStdString() << std::endl;
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

	MainWin win;

	if(!QSystemTrayIcon::isSystemTrayAvailable() || parser.isSet(systrayOption))
		win.show();

	return app.exec();
}
