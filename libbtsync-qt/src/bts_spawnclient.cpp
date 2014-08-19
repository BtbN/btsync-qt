#include <QTemporaryFile>
#include <QStringList>
#include <QProcess>
#include <QPointer>
#include <QRegExp>
#include <QUuid>

#include "bts_spawnclient.h"
#include "bts_global.h"


struct BtsSpawnClient_private
{
	QString host;
	int port;
	QString username;
	QString password;

	int cur_port;
	QString cur_username;
	QString cur_password;

	QTemporaryFile configFile;
	QPointer<QProcess> clientProc;
};

BtsSpawnClient::BtsSpawnClient(QObject *parent)
	:BtsClient(parent)
{
	p = new BtsSpawnClient_private();

	p->cur_port = 0;
	p->host = "127.0.0.1";
	randomize();

	p->clientProc = nullptr;
}

BtsSpawnClient::~BtsSpawnClient()
{
	delete p;
}

QString BtsSpawnClient::getHost()
{
	return "127.0.0.1";
}

int BtsSpawnClient::getPort()
{
	return p->cur_port;
}

QString BtsSpawnClient::getUserName()
{
	return p->cur_username;
}

QString BtsSpawnClient::getPassword()
{
	return p->cur_password;
}

bool BtsSpawnClient::isClientReady()
{
	if(!p->clientProc)
		return false;

	return p->clientProc->state() == QProcess::Running;
}

void BtsSpawnClient::exitClient()
{
	exitClient(false);
}

void BtsSpawnClient::exitClient(bool force)
{
	if(!p->clientProc)
		return;

	p->clientProc->terminate();

	if(force && !p->clientProc->waitForFinished(5000))
	{
		p->clientProc->kill();
	}
}

#ifdef Q_OS_WIN
# define ARG_PREFIX "/"
#else
# define ARG_PREFIX "--"
#endif

void BtsSpawnClient::startClient()
{
	if(isClientReady())
		return;

	p->configFile.open();
	//TODO: Write the actual config!
	p->configFile.close();

	p->clientProc = new QProcess(this);
	connect(p->clientProc, SIGNAL(finished(int)), this, SLOT(procFinished()));
	connect(p->clientProc, SIGNAL(finished(int)), p->clientProc, SLOT(deleteLater()));

	p->clientProc->setProgram(BtsGlobal::getBtsyncExecutablePath());
	p->clientProc->setArguments(QStringList() << ARG_PREFIX "config" << p->configFile.fileName());

	p->clientProc->start();
}

void BtsSpawnClient::restartClient()
{
	exitClient();
	startClient();
}

void BtsSpawnClient::setPort(int port)
{
	p->port = port;
}

void BtsSpawnClient::setHost(QString host)
{
	p->host = host;
}

void BtsSpawnClient::setUsername(QString username)
{
	p->username = username;
}

void BtsSpawnClient::setPassword(QString password)
{
	p->password = password;
}

void BtsSpawnClient::randomizePort()
{
	p->port = (qrand() % 64000) + 1025;
}

void BtsSpawnClient::randomizeCredentials()
{
	QRegExp removeExp("[\\{\\}\\-]");

	p->username = QUuid::createUuid().toString().remove(removeExp);
	p->password = QUuid::createUuid().toString().remove(removeExp);
}

void BtsSpawnClient::randomize()
{
	randomizePort();
	randomizeCredentials();
}

void BtsSpawnClient::procFinished()
{

}
