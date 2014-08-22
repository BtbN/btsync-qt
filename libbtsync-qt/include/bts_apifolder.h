#pragma once

#include <QObject>

#include "bts_export_helper.h"

class BtsApi;

struct BtsApiFolder_private;

class LIBBTS_EXPORT BtsApiFolder : public QObject
{
	Q_OBJECT

	BtsApiFolder& operator=(const BtsApiFolder&) = delete;
	BtsApiFolder(const BtsApiFolder&) = delete;

	public:
	BtsApiFolder(QObject *parent = 0);
	BtsApiFolder(BtsApi *api, QObject *parent = 0);
	BtsApiFolder(const QString &secret, QObject *parent = 0);
	BtsApiFolder(BtsApi *api, const QString &secret, QObject *parent = 0);
	~BtsApiFolder();

	void setApi(BtsApi *api);
	BtsApi *getApi();

	void setWatchedSecret(const QString &secret);
	QString getWatchedSecret();

	public slots:
	void getFolders();
	void removeFolder();
	void getFiles(const QString &path = QString());
	void setFilePrefs(const QString &path, bool download);
	void getFolderPeers();
	void getFolderPrefs();
	void setFolderPrefs(const QVariantHash &prefs);
	void getFolderHosts();
	void setFolderHosts(const QStringList &hosts);
	void getSecrets(bool encryption = false);

	signals:
	void getFoldersResult(const QVector<BtsGetFoldersResult> &result, const QString &secret);
	void removeFolderResult(const QString &secret);
	void getFilesResult(const QVector<BtsGetFilesResult> &result, const QString &secret);
	void setFilePrefsResult(const QVector<BtsGetFilesResult> &result, const QString &secret);
	void getFolderPeersResult(const QVector<BtsGetFolderPeersResult> &result, const QString &secret);
	void getFolderPrefsResult(const QVariantHash &prefs, const QString &secret);
	void setFolderPrefsResult(const QVariantHash &prefs, const QString &secret);
	void getFolderHostsResult(const QStringList &hosts, const QString &secret);
	void setFolderHostsResult(const QStringList &hosts, const QString &secret);
	void getSecretsResult(const QString &readWrite, const QString &readOnly, const QString &encryption);

	private slots:
	void parseGetFoldersResult(const QVector<BtsGetFoldersResult> &result, const QString &secret);
	void parseRemoveFolderResult(const QString &secret);
	void parseGetFilesResult(const QVector<BtsGetFilesResult> &result, const QString &secret);
	void parseSetFilePrefsResult(const QVector<BtsGetFilesResult> &result, const QString &secret);
	void parseGetFolderPeersResult(const QVector<BtsGetFolderPeersResult> &result, const QString &secret);
	void parseGetFolderPrefsResult(const QVariantHash &prefs, const QString &secret);
	void parseSetFolderPrefsResult(const QVariantHash &prefs, const QString &secret);
	void parseGetFolderHostsResult(const QStringList &hosts, const QString &secret);
	void parseSetFolderHostsResult(const QStringList &hosts, const QString &secret);
	void parseGetSecretsResult(const QString &readWrite, const QString &readOnly, const QString &encryption);

	private:
	BtsApiFolder_private *p;
};
