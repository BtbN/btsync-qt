#include <QObject>

#include "utils.h"


QString byteCountToString(qint64 bytes, bool showGb, bool isSpeed)
{
	double size = bytes;
	QString sizeString;

	if(isSpeed)
	{
		sizeString = QObject::tr("%L1 Byte/s").arg(size, 0, 'f', 0);

		if(size >= 1000.0)
		{
			size /= 1024.0;
			sizeString = QObject::tr("%L1 kB/s").arg(size, 0, 'f', 1);
		}

		if(size >= 1000.0)
		{
			size /= 1024.0;
			sizeString = QObject::tr("%L1 MB/s").arg(size, 0, 'f', 1);
		}

		if(!showGb)
			return sizeString;

		if(size >= 1000.0)
		{
			size /= 1024.0;
			sizeString = QObject::tr("%L1 GB/s").arg(size, 0, 'f', 1);
		}
	}
	else
	{
		sizeString = QObject::tr("%L1 Byte").arg(size, 0, 'f', 0);

		if(size >= 1000.0)
		{
			size /= 1024.0;
			sizeString = QObject::tr("%L1 kB").arg(size, 0, 'f', 1);
		}

		if(size >= 1000.0)
		{
			size /= 1024.0;
			sizeString = QObject::tr("%L1 MB").arg(size, 0, 'f', 1);
		}

		if(!showGb)
			return sizeString;

		if(size >= 1000.0)
		{
			size /= 1024.0;
			sizeString = QObject::tr("%L1 GB").arg(size, 0, 'f', 1);
		}
	}

	return sizeString;
}
