#include <QRegExp>

#include "bts_global.h"


BtsApiKey::BtsApiKey(const QString &key)
{
	setKey(key);
}

BtsApiKey::BtsApiKey(const char *key)
{
	setKey(key);
}

BtsApiKey &BtsApiKey::operator=(const QString &key)
{
	setKey(key);
	return *this;
}

BtsApiKey &BtsApiKey::operator=(const char *key)
{
	setKey(key);
	return *this;
}

void BtsApiKey::setKey(const QString &newkey)
{
	QRegExp keyRegExp("^[a-zA-Z0-9]{135}$", Qt::CaseInsensitive, QRegExp::RegExp2);

	if(!keyRegExp.exactMatch(newkey))
	{
		throw BtsException("Tried to set an invalid BTSync API key.");
	}

	key = newkey;
}

static BtsApiKey globalApiKey;

void BtsGlobal::setApiKey(const BtsApiKey &key)
{
	globalApiKey = key;
}

BtsApiKey BtsGlobal::getApiKey()
{
	if(!globalApiKey.isValid())
	{
		throw BtsException("No global BTSync API key has been set yet");
	}

	return globalApiKey;
}
