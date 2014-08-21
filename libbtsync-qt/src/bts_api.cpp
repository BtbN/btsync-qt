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
#include <QRegExp>
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
	{
		if(qp.second.isNull())
			urlQuery.addQueryItem(qp.first, "");
		else
			urlQuery.addQueryItem(qp.first, qp.second);
	}

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

		QJsonArray arr = doc.array();
		QVector<BtsGetFoldersResult> res;
		res.reserve(arr.size());

		for(const QJsonValue &val: arr)
		{
			QJsonObject folderObj = val.toObject();

			if(folderObj.isEmpty())
			{
				emit error("Got an unexpected get_folders reply format");
				return;
			}

			BtsGetFoldersResult resObj;

			resObj.dir = folderObj.value("dir").toString();
			resObj.secret = folderObj.value("secret").toString();
			resObj.size = folderObj.value("size").toVariant().toLongLong();
			resObj.type = folderObj.value("type").toString();
			resObj.files = folderObj.value("files").toVariant().toLongLong();
			resObj.error = folderObj.value("error").toInt();
			resObj.indexing = folderObj.value("indexing").toInt();

			res << resObj;
		}

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

void BtsApi::addFolder(const QString &dir, const QString &secret)
{
	addFolder(dir, false, secret);
}

void BtsApi::removeFolder(const QString &secret)
{
	QueryList ql;

	ql << QueryPair("secret", secret);

	QUrl apiUrl = getApiUrl(p, "remove_folder", ql);

	QNetworkReply *reply = p->nam->get(QNetworkRequest(apiUrl));

	connect(reply, &QNetworkReply::finished, [this, reply, secret]()
	{
		if(checkForError(reply))
			return;

		QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());

		if(checkForError(doc))
			return;

		emit removeFolderResult(secret);
	});
}

static void parseGetFilesResult(QJsonObject &fileObj, BtsGetFilesResult &resObj)
{
	resObj.typeString = fileObj.value("type").toString();

	if(resObj.typeString == "file")
		resObj.type = BtsGetFilesResultType::File;
	else if(resObj.typeString == "folder")
		resObj.type = BtsGetFilesResultType::Folder;
	else if(resObj.typeString == "empty")
		resObj.type = BtsGetFilesResultType::EmptyFile;
	else
		resObj.type = BtsGetFilesResultType::Unknown;

	resObj.name = fileObj.value("name").toString();
	resObj.state = fileObj.value("state").toString();

	if(resObj.type == BtsGetFilesResultType::File)
	{
		resObj.size = fileObj.value("size").toVariant().toLongLong();
		resObj.total_pieces = fileObj.value("total_pieces").toInt();
		resObj.have_pieces = fileObj.value("have_pieces").toInt();
		resObj.download = fileObj.value("download").toInt(1) ? true : false;
	}
	else
	{
		resObj.size = 0;
		resObj.total_pieces = 0;
		resObj.have_pieces = 0;
		resObj.download = true;
	}
}

void BtsApi::getFiles(const QString &secret, const QString &path)
{
	QueryList ql;

	ql << QueryPair("secret", secret);

	if(!path.isEmpty())
		ql << QueryPair("path", path);

	QUrl apiUrl = getApiUrl(p, "get_files", ql);

	QNetworkReply *reply = p->nam->get(QNetworkRequest(apiUrl));

	connect(reply, &QNetworkReply::finished, [this, reply, secret]()
	{
		if(checkForError(reply))
			return;

		QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());

		if(checkForError(doc))
			return;

		QJsonArray arr = doc.array();
		QVector<BtsGetFilesResult> res;
		res.reserve(arr.size());

		for(const QJsonValue &val: arr)
		{
			QJsonObject fileObj = val.toObject();

			if(fileObj.isEmpty())
			{
				emit error("Got an unexpected get_files reply format");
				return;
			}

			BtsGetFilesResult resObj;

			parseGetFilesResult(fileObj, resObj);

			res << resObj;
		}

		emit getFilesResult(res, secret);
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

	connect(reply, &QNetworkReply::finished, [this, reply, secret]()
	{
		if(checkForError(reply))
			return;

		QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());

		if(checkForError(doc))
			return;

		QJsonArray arr = doc.array();
		QVector<BtsGetFilesResult> res;
		res.reserve(arr.size());

		for(const QJsonValue &val: arr)
		{
			QJsonObject fileObj = val.toObject();

			if(fileObj.isEmpty())
			{
				emit error("Got an unexpected set_file_prefs reply format");
				return;
			}

			BtsGetFilesResult resObj;

			parseGetFilesResult(fileObj, resObj);

			res << resObj;
		}

		emit setFilePrefsResult(res, secret);
	});
}

void BtsApi::getFolderPeers(const QString &secret)
{
	QueryList ql;

	ql << QueryPair("secret", secret);

	QUrl apiUrl = getApiUrl(p, "get_folder_peers", ql);

	QNetworkReply *reply = p->nam->get(QNetworkRequest(apiUrl));

	connect(reply, &QNetworkReply::finished, [this, reply, secret]()
	{
		if(checkForError(reply))
			return;

		QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());

		if(checkForError(doc))
			return;

		QJsonArray arr = doc.array();
		QVector<BtsGetFolderPeersResult> res;
		res.reserve(arr.size());

		for(const QJsonValue &val: arr)
		{
			QJsonObject peerObj = val.toObject();

			if(peerObj.isEmpty())
			{
				emit error("Got an unexpected get_folder_peers reply format");
				return;
			}

			BtsGetFolderPeersResult resObj;

			resObj.id = peerObj.value("id").toString();
			resObj.connection = peerObj.value("connection").toString();
			resObj.name = peerObj.value("name").toString();
			resObj.synced = peerObj.value("synced").toVariant().toLongLong();
			resObj.download = peerObj.value("download").toVariant().toLongLong();
			resObj.upload = peerObj.value("upload").toVariant().toLongLong();

			res << resObj;
		}

		emit getFolderPeersResult(res, secret);
	});
}

void BtsApi::getSecrets(bool encryption, const QString &secret)
{
	QueryList ql;

	if(!secret.isEmpty())
		ql << QueryPair("secret", secret);

	if(encryption)
		ql << QueryPair("type", "encryption");

	QUrl apiUrl = getApiUrl(p, "get_secrets", ql);

	QNetworkReply *reply = p->nam->get(QNetworkRequest(apiUrl));

	connect(reply, &QNetworkReply::finished, [this, reply]()
	{
		if(checkForError(reply))
			return;

		QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());

		if(checkForError(doc))
			return;

		QJsonObject obj = doc.object();

		QString rw = obj.value("read_write").toString();
		QString ro = obj.value("read_only").toString();
		QString ec = obj.value("encryption").toString();

		emit getSecretsResult(rw, ro, ec);
	});
}

void BtsApi::getSecrets(const QString &secret)
{
	getSecrets(false, secret);
}

void BtsApi::getFolderPrefs(const QString &secret)
{
	QueryList ql;

	ql << QueryPair("secret", secret);

	QUrl apiUrl = getApiUrl(p, "get_folder_prefs", ql);

	QNetworkReply *reply = p->nam->get(QNetworkRequest(apiUrl));

	connect(reply, &QNetworkReply::finished, [this, reply, secret]()
	{
		if(checkForError(reply))
			return;

		QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());

		if(checkForError(doc))
			return;

		QJsonObject obj = doc.object();
		QVariantHash res;

		for(auto it = obj.constBegin(); it != obj.constEnd(); ++it)
			res[it.key()] = it.value();

		emit getFolderPrefsResult(res, secret);
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

	connect(reply, &QNetworkReply::finished, [this, reply, secret]()
	{
		if(checkForError(reply))
			return;

		QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());

		if(checkForError(doc))
			return;

		QJsonObject obj = doc.object();
		QVariantHash res;

		for(auto it = obj.constBegin(); it != obj.constEnd(); ++it)
			res[it.key()] = it.value();

		emit setFolderPrefsResult(res, secret);
	});
}

void BtsApi::getFolderHosts(const QString &secret)
{
	QueryList ql;

	ql << QueryPair("secret", secret);

	QUrl apiUrl = getApiUrl(p, "get_folder_hosts", ql);

	QNetworkReply *reply = p->nam->get(QNetworkRequest(apiUrl));

	connect(reply, &QNetworkReply::finished, [this, reply, secret]()
	{
		if(checkForError(reply))
			return;

		QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());

		if(checkForError(doc))
			return;

		QJsonObject obj = doc.object();
		QJsonArray arr = obj.value("hosts").toArray();
		QStringList res;

		for(const QJsonValue &val: arr)
			res << val.toString();

		emit getFolderHostsResult(res, secret);
	});
}

void BtsApi::setFolderHosts(const QString &secret, const QStringList &hosts)
{
	QueryList ql;

	ql << QueryPair("secret", secret)
	   << QueryPair("hosts", hosts.join(','));

	QUrl apiUrl = getApiUrl(p, "set_folder_hosts", ql);

	QString queryString = apiUrl.query(QUrl::FullyEncoded);

	qDebug() << apiUrl;

	QNetworkReply *reply = p->nam->get(QNetworkRequest(apiUrl));

	connect(reply, &QNetworkReply::finished, [this, reply, secret]()
	{
		if(checkForError(reply))
			return;

		QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());

		if(checkForError(doc))
			return;

		QJsonObject obj = doc.object();
		QJsonArray arr = obj.value("hosts").toArray();
		QStringList res;

		for(const QJsonValue &val: arr)
			res << val.toString();

		emit setFolderHostsResult(res, secret);
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

		QJsonObject obj = doc.object();
		QVariantHash res;

		for(auto it = obj.constBegin(); it != obj.constEnd(); ++it)
			res[it.key()] = it.value();

		emit getPreferencesResult(res);
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

		QJsonObject obj = doc.object();
		QVariantHash res;

		for(auto it = obj.constBegin(); it != obj.constEnd(); ++it)
			res[it.key()] = it.value();

		emit setPreferencesResult(res);
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

		QJsonObject obj = doc.object();

		emit getOsNameResult(obj.value("os").toString());
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

		QJsonObject obj = doc.object();

		emit getVersionResult(obj.value("os").toString());
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

		QJsonObject obj = doc.object();

		qint64 down = obj.value("download").toVariant().toLongLong();
		qint64 up = obj.value("upload").toVariant().toLongLong();

		emit getSpeedResult(down, up);
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
