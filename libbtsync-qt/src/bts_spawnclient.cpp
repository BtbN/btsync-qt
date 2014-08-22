#include <QtDebug>

#include <QTemporaryFile>
#include <QStandardPaths>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStringList>
#include <QProcess>
#include <QPointer>
#include <QRegExp>
#include <QTimer>
#include <QUuid>
#include <QDir>

#include "bts_spawnclient.h"
#include "bts_global.h"
#include "bts_api.h"


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
	QPointer<QTimer> forceKillTimer;

	bool autorestart;
};

BtsSpawnClient::BtsSpawnClient(QObject *parent)
	:BtsClient(parent)
{
	p = new BtsSpawnClient_private();

	p->autorestart = false;
	p->cur_port = 0;
	p->host = "127.0.0.1";
	randomize();

	p->clientProc = nullptr;

	p->forceKillTimer = new QTimer(this);
	p->forceKillTimer->setSingleShot(true);
	p->forceKillTimer->setInterval(2500);
	connect(p->forceKillTimer, SIGNAL(timeout()), this, SLOT(killClient()));
}

BtsSpawnClient::~BtsSpawnClient()
{
	if(p->clientProc)
	{
		BtsApi api(this);

		api.shutdown();
		p->clientProc->waitForFinished(5000);

		p->clientProc->terminate();

		if(!p->clientProc->waitForFinished(5000))
		{
			p->clientProc->kill();
			p->clientProc->waitForFinished(500);
		}

		emit clientExited();
	}

	delete p;
}

void BtsSpawnClient::setAutorestart(bool autorestart)
{
	p->autorestart = autorestart;
}

bool BtsSpawnClient::isAutorestart()
{
	return p->autorestart;
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

void BtsSpawnClient::killClient()
{
	exitClient(true);
}

void BtsSpawnClient::exitClient(bool force)
{
	if(!p->clientProc)
		return;

	if(force)
	{
		p->clientProc->kill();
		p->forceKillTimer->stop();
	}
	else
	{
		p->clientProc->terminate();
		p->forceKillTimer->start();
	}
}

void BtsSpawnClient::startClient()
{
	if(isClientReady())
		return;

	QDir dataPath = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
	dataPath.mkpath("sync_storage");

	if(!dataPath.cd("sync_storage"))
	{
		qDebug() << "Failed creating sync storage dir!";
		return;
	}

	QJsonObject configObject;
	configObject.insert("storage_path", dataPath.absolutePath());
	configObject.insert("use_gui", false);

	QJsonObject webuiObject;
	webuiObject.insert("listen", QString("%1:%2").arg(p->host).arg(p->port));
	webuiObject.insert("login", p->username);
	webuiObject.insert("password", p->password);
	webuiObject.insert("api_key", BtsGlobal::getApiKey().toString());

	configObject.insert("webui", webuiObject);

	p->configFile.open();
	QJsonDocument saveDoc(configObject);
	p->configFile.write(saveDoc.toJson());
	p->configFile.close();

	p->configFile.setPermissions(QFile::ReadOwner | QFile::WriteOwner);

	p->cur_port = p->port;
	p->cur_password = p->password;
	p->cur_username = p->username;

	p->clientProc = new QProcess(this);
	connect(p->clientProc, SIGNAL(finished(int)), this, SLOT(procFinished(int)));
	connect(p->clientProc, SIGNAL(finished(int)), p->clientProc, SLOT(deleteLater()));
	connect(p->clientProc, SIGNAL(started()), this, SLOT(procStarted()));

#ifdef Q_OS_WIN
# define ARG_PREFIX "/"
#else
# define ARG_PREFIX "--"
#endif

	p->clientProc->setProgram(BtsGlobal::getBtsyncExecutablePath());
	p->clientProc->setArguments(QStringList()
	                            << ARG_PREFIX "nodaemon"
	                            << ARG_PREFIX "config" << p->configFile.fileName());

	p->clientProc->setProcessChannelMode(QProcess::ForwardedChannels);

	p->clientProc->start();
}

void BtsSpawnClient::restartClient()
{
	if(!p->clientProc)
	{
		startClient();
	}
	else
	{
		connect(p->clientProc.data(), SIGNAL(finished(int)), this, SLOT(startClient()));
		exitClient();
	}
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

void BtsSpawnClient::procStarted()
{
	QTimer::singleShot(2000, this, SIGNAL(clientStarted()));
	QTimer::singleShot(10000, this, SLOT(removeConfig()));
}

void BtsSpawnClient::procFinished(int exitCode)
{
	p->forceKillTimer->stop();
	emit clientExited();

	qDebug() << "btsync finished with code" << exitCode;

	if(exitCode != 0 || p->autorestart)
	{
		QTimer::singleShot(1000, this, SLOT(startClient()));
	}
}

void BtsSpawnClient::removeConfig()
{
	p->configFile.remove();
}
