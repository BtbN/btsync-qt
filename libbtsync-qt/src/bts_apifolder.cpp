#include <QPointer>

#include "bts_api.h"
#include "bts_global.h"
#include "bts_apifolder.h"


struct BtsApiFolder_private
{
	QPointer<BtsApi> api;
	QString watchedSecret;
};


BtsApiFolder::BtsApiFolder(QObject *parent)
	:QObject(parent)
{
	p = new BtsApiFolder_private();
}

BtsApiFolder::BtsApiFolder(BtsApi *api, QObject *parent)
	:QObject(parent)
{
	p = new BtsApiFolder_private();
	setApi(api);
}

BtsApiFolder::BtsApiFolder(const QString &secret, QObject *parent)
	:QObject(parent)
{
	p = new BtsApiFolder_private();
	p->watchedSecret = secret;
}

BtsApiFolder::BtsApiFolder(BtsApi *api, const QString &secret, QObject *parent)
	:QObject(parent)
{
	p = new BtsApiFolder_private();
	p->watchedSecret = secret;
	setApi(api);
}

BtsApiFolder::~BtsApiFolder()
{
	delete p;
}

void BtsApiFolder::setApi(BtsApi *api)
{
	if(!p->api.isNull())
	{
		p->api->disconnect(this);
	}

	p->api = api;

	if(!p->api.isNull())
	{
		connect(api, &BtsApi::removeFolderResult, this, &BtsApiFolder::parseRemoveFolderResult);
		connect(api, &BtsApi::getFilesResult, this, &BtsApiFolder::parseGetFilesResult);
		connect(api, &BtsApi::setFilePrefsResult, this, &BtsApiFolder::parseSetFilePrefsResult);
		connect(api, &BtsApi::getFolderPeersResult, this, &BtsApiFolder::parseGetFolderPeersResult);
		connect(api, &BtsApi::getFolderPrefsResult, this, &BtsApiFolder::parseGetFolderPrefsResult);
		connect(api, &BtsApi::setFolderPrefsResult, this, &BtsApiFolder::parseSetFolderPrefsResult);
		connect(api, &BtsApi::getFolderHostsResult, this, &BtsApiFolder::parseGetFolderHostsResult);
		connect(api, &BtsApi::setFolderHostsResult, this, &BtsApiFolder::parseSetFolderHostsResult);
		connect(api, &BtsApi::getSecretsResult, this, &BtsApiFolder::parseGetSecretsResult);
	}
}

BtsApi *BtsApiFolder::getApi()
{
	return p->api;
}

void BtsApiFolder::setWatchedSecret(const QString &secret)
{
	p->watchedSecret = secret;
}

QString BtsApiFolder::getWatchedSecret()
{
	return p->watchedSecret;
}

static void assertApi(BtsApiFolder_private *p)
{
	if(p->api.isNull())
		throw BtsException("tried to perform folder action on null BtsApi");

	if(p->watchedSecret.isEmpty())
		throw BtsException("tried to perform folder action without a secret");
}

void BtsApiFolder::removeFolder()
{
	assertApi(p);

	p->api->removeFolder(p->watchedSecret);
}

void BtsApiFolder::getFiles(const QString &path)
{
	assertApi(p);

	p->api->getFiles(p->watchedSecret, path);
}

void BtsApiFolder::setFilePrefs(const QString &path, bool download)
{
	assertApi(p);

	p->api->setFilePrefs(p->watchedSecret, path, download);
}

void BtsApiFolder::getFolderPeers()
{
	assertApi(p);

	p->api->getFolderPeers(p->watchedSecret);
}

void BtsApiFolder::getFolderPrefs()
{
	assertApi(p);

	p->api->getFolderPrefs(p->watchedSecret);
}

void BtsApiFolder::setFolderPrefs(const QVariantHash &prefs)
{
	assertApi(p);

	p->api->setFolderPrefs(p->watchedSecret, prefs);
}

void BtsApiFolder::getFolderHosts()
{
	assertApi(p);

	p->api->getFolderHosts(p->watchedSecret);
}

void BtsApiFolder::setFolderHosts(const QStringList &hosts)
{
	assertApi(p);

	p->api->setFolderHosts(p->watchedSecret, hosts);
}

void BtsApiFolder::getSecrets(bool encryption)
{
	assertApi(p);

	p->api->getSecrets(encryption, p->watchedSecret);
}

void BtsApiFolder::parseRemoveFolderResult(const QString &secret)
{
	if(secret != p->watchedSecret)
		return;
}

void BtsApiFolder::parseGetFilesResult(const QVector<BtsGetFilesResult> &result, const QString &secret)
{
	if(secret != p->watchedSecret)
		return;

	emit getFilesResult(result, secret);
}

void BtsApiFolder::parseSetFilePrefsResult(const QVector<BtsGetFilesResult> &result, const QString &secret)
{
	if(secret != p->watchedSecret)
		return;

	emit setFilePrefsResult(result, secret);
}

void BtsApiFolder::parseGetFolderPeersResult(const QVector<BtsGetFolderPeersResult> &result, const QString &secret)
{
	if(secret != p->watchedSecret)
		return;

	emit getFolderPeersResult(result, secret);
}

void BtsApiFolder::parseGetFolderPrefsResult(const QVariantHash &prefs, const QString &secret)
{
	if(secret != p->watchedSecret)
		return;

	emit getFolderPrefsResult(prefs, secret);
}

void BtsApiFolder::parseSetFolderPrefsResult(const QVariantHash &prefs, const QString &secret)
{
	if(secret != p->watchedSecret)
		return;

	emit setFolderPrefsResult(prefs, secret);
}

void BtsApiFolder::parseGetFolderHostsResult(const QStringList &hosts, const QString &secret)
{
	if(secret != p->watchedSecret)
		return;

	emit getFolderHostsResult(hosts, secret);
}

void BtsApiFolder::parseSetFolderHostsResult(const QStringList &hosts, const QString &secret)
{
	if(secret != p->watchedSecret)
		return;

	emit setFolderHostsResult(hosts, secret);
}

void BtsApiFolder::parseGetSecretsResult(const QString &readWrite, const QString &readOnly, const QString &encryption)
{
	if(readWrite != p->watchedSecret)
		return;

	emit getSecretsResult(readWrite, readOnly, encryption);
}
