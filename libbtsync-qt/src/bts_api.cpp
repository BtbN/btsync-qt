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
	connect(p->nam, &QNetworkAccessManager::finished, [](QNetworkReply *reply)
	{
		reply->deleteLater();
	});
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

bool BtsApi::checkForError(QNetworkReply *reply)
{
	if(reply->error() != QNetworkReply::NoError)
	{
		emit error(QString("Network request to BTSync failed: %1").arg(reply->errorString()));
		return true;
	}

	return false;
}

bool BtsApi::checkForError(const QJsonDocument &doc)
{
	qDebug() << doc;

	QJsonObject obj = doc.object();

	int errorCode = obj.value("error").toInt(0);

	if(errorCode == 0)
		return false;

	QString errorString = obj.value("message").toString("no error message");

	emit error(QString("BTSync error %1: %2").arg(errorCode).arg(errorString));

	return true;
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
		if(checkForError(reply))
			return;

		QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());

		if(checkForError(doc))
			return;

		QVector<BtsGetFoldersResult> res;

		emit getFoldersResult(res);
	});
}

void BtsApi::addFolder(const QString &dir, bool selective_sync, const QString &secret)
{
	QueryList ql;

	ql << QueryPair("dir", dir);

	if(!secret.isEmpty())
		ql << QueryPair("secret", secret);

	if(selective_sync)
		ql << QueryPair("selective_sync", "1");

	QUrl apiUrl = getApiUrl(p, "add_folder", ql);

	QNetworkReply *reply = p->nam->get(QNetworkRequest(apiUrl));

	connect(reply, &QNetworkReply::finished, [this, reply]()
	{
		if(checkForError(reply))
			return;

		QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());

		if(checkForError(doc))
			return;

		emit addFolderResult();
	});
}

void BtsApi::removeFolder(const QString &secret)
{
	QueryList ql;

	ql << QueryPair("secret", secret);

	QUrl apiUrl = getApiUrl(p, "remove_folder", ql);

	QNetworkReply *reply = p->nam->get(QNetworkRequest(apiUrl));

	connect(reply, &QNetworkReply::finished, [this, reply]()
	{
		if(checkForError(reply))
			return;

		QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());

		if(checkForError(doc))
			return;

		emit removeFolderResult();
	});
}

void BtsApi::getFiles(const QString &secret, const QString &path)
{
	QueryList ql;

	ql << QueryPair("secret", secret);

	if(!path.isEmpty())
		ql << QueryPair("path", path);

	QUrl apiUrl = getApiUrl(p, "get_files", ql);

	QNetworkReply *reply = p->nam->get(QNetworkRequest(apiUrl));

	connect(reply, &QNetworkReply::finished, [this, reply]()
	{
		if(checkForError(reply))
			return;

		QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());

		if(checkForError(doc))
			return;

		//TODO
		emit getFilesResult();
	});
}

void BtsApi::setFilePrefs(const QString &secret, const QString &path, bool download)
{
	QueryList ql;

	ql << QueryPair("secret", secret)
	   << QueryPair("path", path)
	   << QueryPair("download", download ? "1" : "0");

	QUrl apiUrl = getApiUrl(p, "set_file_prefs", ql);

	QNetworkReply *reply = p->nam->get(QNetworkRequest(apiUrl));

	connect(reply, &QNetworkReply::finished, [this, reply]()
	{
		if(checkForError(reply))
			return;

		QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());

		if(checkForError(doc))
			return;

		//TODO
		emit setFilePrefsResult();
	});
}

void BtsApi::getFolderPeers(const QString &secret)
{
	QueryList ql;

	ql << QueryPair("secret", secret);

	QUrl apiUrl = getApiUrl(p, "get_folder_peers", ql);

	QNetworkReply *reply = p->nam->get(QNetworkRequest(apiUrl));

	connect(reply, &QNetworkReply::finished, [this, reply]()
	{
		if(checkForError(reply))
			return;

		QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());

		if(checkForError(doc))
			return;

		//TODO
		emit getFolderPeersResult();
	});
}

void BtsApi::getSecrets(const QString &secret)
{
	QueryList ql;

	if(!secret.isEmpty())
		ql << QueryPair("secret", secret);

	QUrl apiUrl = getApiUrl(p, "get_secrets", ql);

	QNetworkReply *reply = p->nam->get(QNetworkRequest(apiUrl));

	connect(reply, &QNetworkReply::finished, [this, reply]()
	{
		if(checkForError(reply))
			return;

		QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());

		if(checkForError(doc))
			return;

		//TODO
		emit getSecretsResult("", "", "");
	});
}

void BtsApi::getFolderPrefs(const QString &secret)
{
	QueryList ql;

	ql << QueryPair("secret", secret);

	QUrl apiUrl = getApiUrl(p, "get_folder_prefs", ql);

	QNetworkReply *reply = p->nam->get(QNetworkRequest(apiUrl));

	connect(reply, &QNetworkReply::finished, [this, reply]()
	{
		if(checkForError(reply))
			return;

		QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());

		if(checkForError(doc))
			return;

		//TODO
		emit getFolderPrefsResult();
	});
}

void BtsApi::setFolderPrefs(const QString &secret, const QVariantHash &prefs)
{
	QueryList ql;

	ql << QueryPair("secret", secret);

	for(auto it = prefs.constBegin(); it != prefs.constEnd(); ++it)
		ql << QueryPair(it.key(), it.value().toString());

	QUrl apiUrl = getApiUrl(p, "set_folder_prefs", ql);

	QNetworkReply *reply = p->nam->get(QNetworkRequest(apiUrl));

	connect(reply, &QNetworkReply::finished, [this, reply]()
	{
		if(checkForError(reply))
			return;

		QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());

		if(checkForError(doc))
			return;

		//TODO
		emit setFolderPrefsResult();
	});
}

void BtsApi::getFolderHosts(const QString &secret)
{
	QueryList ql;

	ql << QueryPair("secret", secret);

	QUrl apiUrl = getApiUrl(p, "get_folder_hosts", ql);

	QNetworkReply *reply = p->nam->get(QNetworkRequest(apiUrl));

	connect(reply, &QNetworkReply::finished, [this, reply]()
	{
		if(checkForError(reply))
			return;

		QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());

		if(checkForError(doc))
			return;

		//TODO
		emit getFolderHostsResult();
	});
}

void BtsApi::setFolderHosts(const QString &secret, const QStringList &hosts)
{
	QueryList ql;

	ql << QueryPair("secret", secret)
	   << QueryPair("hosts", hosts.join(','));

	QUrl apiUrl = getApiUrl(p, "set_folder_hosts", ql);

	QNetworkReply *reply = p->nam->get(QNetworkRequest(apiUrl));

	connect(reply, &QNetworkReply::finished, [this, reply]()
	{
		if(checkForError(reply))
			return;

		QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());

		if(checkForError(doc))
			return;

		//TODO
		emit setFolderHostsResult();
	});
}

void BtsApi::getPreferences()
{
	QUrl apiUrl = getApiUrl(p, "get_prefs");

	QNetworkReply *reply = p->nam->get(QNetworkRequest(apiUrl));

	connect(reply, &QNetworkReply::finished, [this, reply]()
	{
		if(checkForError(reply))
			return;

		QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());

		if(checkForError(doc))
			return;

		//TODO
		emit getPreferencesResult();
	});
}

void BtsApi::setPreferences(const QVariantHash &prefs)
{
	QueryList ql;

	for(auto it = prefs.constBegin(); it != prefs.constEnd(); ++it)
		ql << QueryPair(it.key(), it.value().toString());

	QUrl apiUrl = getApiUrl(p, "set_prefs", ql);

	QNetworkReply *reply = p->nam->get(QNetworkRequest(apiUrl));

	connect(reply, &QNetworkReply::finished, [this, reply]()
	{
		if(checkForError(reply))
			return;

		QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());

		if(checkForError(doc))
			return;

		//TODO
		emit setPreferencesResult();
	});
}

void BtsApi::getOsName()
{
	QUrl apiUrl = getApiUrl(p, "get_os");

	QNetworkReply *reply = p->nam->get(QNetworkRequest(apiUrl));

	connect(reply, &QNetworkReply::finished, [this, reply]()
	{
		if(checkForError(reply))
			return;

		QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());

		if(checkForError(doc))
			return;

		//TODO
		emit getOsNameResult();
	});
}

void BtsApi::getVersion()
{
	QUrl apiUrl = getApiUrl(p, "get_version");

	QNetworkReply *reply = p->nam->get(QNetworkRequest(apiUrl));

	connect(reply, &QNetworkReply::finished, [this, reply]()
	{
		if(checkForError(reply))
			return;

		QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());

		if(checkForError(doc))
			return;

		//TODO
		emit getVersionResult();
	});
}

void BtsApi::getSpeed()
{
	QUrl apiUrl = getApiUrl(p, "get_speed");

	QNetworkReply *reply = p->nam->get(QNetworkRequest(apiUrl));

	connect(reply, &QNetworkReply::finished, [this, reply]()
	{
		if(checkForError(reply))
			return;

		QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());

		if(checkForError(doc))
			return;

		//TODO
		emit getSpeedResult();
	});
}

void BtsApi::shutdown()
{
	QUrl apiUrl = getApiUrl(p, "shutdown");

	QNetworkReply *reply = p->nam->get(QNetworkRequest(apiUrl));

	connect(reply, &QNetworkReply::finished, [this, reply]()
	{
		if(checkForError(reply))
			return;

		QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());

		if(checkForError(doc))
			return;

		emit shutdownResult();
	});
}
