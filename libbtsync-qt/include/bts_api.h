#pragma once

#include <QObject>
#include <QString>

#include "bts_export_helper.h"

class BtsClient;

struct BtsApi_private;

struct LIBBTS_EXPORT BtsGetFoldersResult
{
	QString dir;
	QString secret;
	qint64 size;
	QString type;
	qint64 files;
	int error;
	int indexing;
};

class LIBBTS_EXPORT BtsApi : public QObject
{
	Q_OBJECT

	void init();

	public:
	BtsApi(QObject *parent = 0);
	BtsApi(BtsClient *client, QObject *parent = 0);
	~BtsApi();

	void setClient(BtsClient *client);
	BtsClient *getClient();

	public slots:
	void getFolders(const QString &secret = QString());

	signals:
	void error(const QString &errorString);
	void getFoldersResult(const BtsGetFoldersResult &result);

	private:
	BtsApi_private *p;
};
