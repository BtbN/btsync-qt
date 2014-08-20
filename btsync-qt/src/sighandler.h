#pragma once

#include <QObject>
class QSocketNotifier;

class SignalHandler : public QObject
{
	Q_OBJECT

	static void hupSigHandler(int unused);
	static void termSigHandler(int unused);
	static void intSigHandler(int unused);

	public:
	SignalHandler(QObject *parent = 0);
	~SignalHandler();

	static bool setup();

	signals:
	void sigHup();
	void sigTerm();
	void sigInt();
	void sigQuitApp();

	private:
	static int sighupFd[2];
	static int sigtermFd[2];
	static int sigintFd[2];

	QSocketNotifier *snHup;
	QSocketNotifier *snTerm;
	QSocketNotifier *snInt;
};
