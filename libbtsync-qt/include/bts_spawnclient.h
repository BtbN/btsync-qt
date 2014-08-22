#pragma once

#include <QObject>

#include "bts_client.h"

struct BtsSpawnClient_private;

class LIBBTS_EXPORT BtsSpawnClient : public BtsClient
{
	Q_OBJECT

	BtsSpawnClient& operator=(const BtsSpawnClient&) = delete;
	BtsSpawnClient(const BtsSpawnClient&) = delete;

	public:
	BtsSpawnClient(QObject *parent = 0);
	~BtsSpawnClient();

	QString getHost();
	int getPort();
	QString getUserName();
	QString getPassword();

	bool isAutorestart();
	bool isClientReady();

	public slots:
	void exitClient();
	void killClient();
	void exitClient(bool force);
	void startClient();
	void restartClient();

	void setAutorestart(bool autorestart);
	void setPort(int port);
	void setHost(QString host);
	void setUsername(QString username);
	void setPassword(QString password);

	void randomizePort();
	void randomizeCredentials();
	void randomize();

	private slots:
	void procStarted();
	void procFinished(int exitCode);
	void removeConfig();

	private:
	BtsSpawnClient_private *p;
};
