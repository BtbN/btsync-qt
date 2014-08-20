#pragma once

#include <QObject>

#include "bts_client.h"

struct BtsSpawnClient_private;

class LIBBTS_EXPORT BtsSpawnClient : public BtsClient
{
	Q_OBJECT

	public:
	BtsSpawnClient(QObject *parent = 0);
	~BtsSpawnClient();

	QString getHost();
	int getPort();
	QString getUserName();
	QString getPassword();

	bool isClientReady();

	public slots:
	void exitClient();
	void killClient();
	void exitClient(bool force);
	void startClient();
	void restartClient();

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

	private:
	BtsSpawnClient_private *p;
};
