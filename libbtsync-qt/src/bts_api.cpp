#include <QtDebug>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

#include <QUrlQuery>
#include <QPointer>
#include <QList>
#include <QPair>
#include <QUrl>

#include "bts_global.h"
#include "bts_client.h"
#include "bts_api.h"


typedef QPair<QString, QString> QueryPair;
typedef QList<QueryPair> QueryList;

struct BtsApi_private
{
	QPointer<BtsClient> client;
	QNetworkAccessManager *nam;
};


BtsApi::BtsApi(QObject *parent)
	:QObject(parent)
{
	p = new BtsApi_private();

	init();
}

BtsApi::BtsApi(BtsClient *client, QObject *parent)
	:QObject(parent)
{
	p = new BtsApi_private();
	p->client = client;

	init();
}

void BtsApi::init()
{
	p->nam = new QNetworkAccessManager(this);
}

BtsApi::~BtsApi()
{
	delete p;
}

void BtsApi::setClient(BtsClient *client)
{
	p->client = client;
}

BtsClient *BtsApi::getClient()
{
	return p->client;
}

static void assertClient(BtsApi_private *p)
{
	if(p->client.isNull())
		throw BtsException("Tried to make an api call without setting a client");
}

static QUrl getApiUrl(BtsApi_private *p, const QString &method, const QueryList &querylist = QueryList())
{
	assertClient(p);

	QUrl res;

	res.setScheme("http");

	res.setHost(p->client->getHost());
	res.setPort(p->client->getPort());

	if(!p->client->getUserName().isEmpty())
		res.setUserName(p->client->getUserName());

	if(!p->client->getPassword().isEmpty())
		res.setPassword(p->client->getPassword());

	res.setPath("/api");

	QUrlQuery urlQuery;
	urlQuery.addQueryItem("method", method);

	for(const QueryPair &qp: querylist)
		urlQuery.addQueryItem(qp.first, qp.second);

	res.setQuery(urlQuery);

	return res;
}

void BtsApi::getFolders(const QString &secret)
{
	QueryList ql;

	if(!secret.isEmpty())
		ql << QueryPair("secret", secret);

	QUrl apiUrl = getApiUrl(p, "get_folders", ql);

	QNetworkReply *reply = p->nam->get(QNetworkRequest(apiUrl));

	connect(reply, &QNetworkReply::finished, [this, reply]()
	{
		if(reply->error() != QNetworkReply::NoError)
		{
			emit error(QString("Network request to btsync api failed: %1").arg(reply->errorString()));
			return;
		}

		qDebug() << QString::fromUtf8(reply->readAll());

		BtsGetFoldersResult res;

		emit getFoldersResult(res);

		reply->deleteLater();
	});
}
