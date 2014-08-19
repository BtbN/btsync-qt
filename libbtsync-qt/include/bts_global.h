#pragma once

#include <QString>
#include <exception>

#include "bts_export_helper.h"

class LIBBTS_EXPORT BtsException : public std::exception
{
	public:
	BtsException(const QString &msg):msg(msg),constMsg(msg.toUtf8()) {}

	QString getMessage() const
	{
		return msg;
	}

	const char *what() const
	{
		return constMsg.constData();
	}

	private:
	QString msg;
	QByteArray constMsg;
};

class LIBBTS_EXPORT BtsApiKey
{
	public:
	BtsApiKey(const QString &key = QString());
	BtsApiKey(const char *key);

	BtsApiKey& operator=(const QString &key);
	BtsApiKey& operator=(const char *key);

	operator QString() const { return key; }

	inline bool isValid() { return !key.isEmpty(); }

	private:
	void setKey(const QString &newkey);

	private:
	QString key;
};

class LIBBTS_EXPORT BtsGlobal
{
	public:
	static void setApiKey(const BtsApiKey &key);
	static BtsApiKey getApiKey();
};
