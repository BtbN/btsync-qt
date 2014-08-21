#pragma once

#include <QObject>
#include <QString>
#include <QVector>
#include <QList>
#include <QHash>

class QJsonDocument;
class QNetworkReply;

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

struct LIBBTS_EXPORT BtsGetFolderPeersResult
{
	QString id;
	QString connection;
	QString name;
	qint64 synced;
	qint64 download;
	qint64 upload;
};

enum BtsGetFilesResultType
{
	Unknown,
	File,
	EmptyFile,
	Folder
};

struct LIBBTS_EXPORT BtsGetFilesResult
{
	BtsGetFilesResultType type;
	QString typeString;

	QString name;
	QString state;

	int total_pieces;
	int have_pieces;
	qint64 size;
	bool download;
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

	private:
	bool checkForError(QNetworkReply *reply);
	bool checkForError(const QJsonDocument &doc);

	public slots:
	void getFolders(const QString &secret = QString());
	void addFolder(const QString &dir, bool selective_sync = false, const QString &secret = QString());
	void removeFolder(const QString &secret);
	void getFiles(const QString &secret, const QString &path = QString());
	void setFilePrefs(const QString &secret, const QString &path, bool download);
	void getFolderPeers(const QString &secret);
	void getSecrets(bool encryption = false, const QString &secret = QString());
	void getSecrets(const QString &secret);
	void getFolderPrefs(const QString &secret);
	void setFolderPrefs(const QString &secret, const QVariantHash &prefs);
	void getFolderHosts(const QString &secret);
	void setFolderHosts(const QString &secret, const QStringList &hosts);
	void getPreferences();
	void setPreferences(const QVariantHash &prefs);
	void getOsName();
	void getVersion();
	void getSpeed();
	void shutdown();

	signals:
	void error(const QString &errorString);
	void getFoldersResult(const QVector<BtsGetFoldersResult> &result);
	void addFolderResult();
	void removeFolderResult();
	void getFilesResult(const QVector<BtsGetFilesResult> &result);
	void setFilePrefsResult(const QVector<BtsGetFilesResult> &result);
	void getFolderPeersResult(const QVector<BtsGetFolderPeersResult> &result);
	void getSecretsResult(const QString &readWrite, const QString &readOnly, const QString &encryption);
	void getFolderPrefsResult(const QVariantHash &prefs);
	void setFolderPrefsResult(const QVariantHash &prefs);
	void getFolderHostsResult(const QStringList &hosts);
	void setFolderHostsResult(const QStringList &hosts);
	void getPreferencesResult();
	void setPreferencesResult();
	void getOsNameResult();
	void getVersionResult();
	void getSpeedResult();
	void shutdownResult();

	private:
	BtsApi_private *p;
};
