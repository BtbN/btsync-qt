#include <QTimer>

#include "bts_remoteclient.h"


struct BtsRemoteClient_private
{
	QString host;
	int port;
	QString username;
	QString password;
};

BtsRemoteClient::BtsRemoteClient(QObject *parent)
	:BtsClient(parent)
{
	p = new BtsRemoteClient_private();

	p->port = -1;
}

BtsRemoteClient::BtsRemoteClient(QString host, int port, QObject *parent)
	:BtsClient(parent)
{
	p = new BtsRemoteClient_private();

	p->port = -1;

	setHost(host);
	setPort(port);
}

BtsRemoteClient::BtsRemoteClient(QString host, int port, QString username, QString password, QObject *parent)
	:BtsClient(parent)
{
	p = new BtsRemoteClient_private();

	p->port = -1;

	setUserName(username);
	setPassword(password);
	setHost(host);
	setPort(port);
}

BtsRemoteClient::~BtsRemoteClient()
{
	delete p;
}

void BtsRemoteClient::notify()
{
	QTimer::singleShot(0, this, SIGNAL(clientStarted()));
}

QString BtsRemoteClient::getHost()
{
	return p->host;
}

void BtsRemoteClient::setHost(const QString &host)
{
	p->host = host;

	if(isClientReady())
		notify();
}

int BtsRemoteClient::getPort()
{
	return p->port;
}

void BtsRemoteClient::setPort(int port)
{
	p->port = port;

	if(isClientReady())
		notify();
}

QString BtsRemoteClient::getUserName()
{
	return p->username;
}

void BtsRemoteClient::setUserName(const QString &username)
{
	p->username = username;

	if(isClientReady())
		notify();
}

QString BtsRemoteClient::getPassword()
{
	return p->password;
}

void BtsRemoteClient::setPassword(const QString &password)
{
	p->password = password;

	if(isClientReady())
		notify();
}

bool BtsRemoteClient::isClientReady()
{
	return !getHost().isEmpty() && getPort() > 0;
}
