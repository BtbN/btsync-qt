#include <QSocketNotifier>

#include <sys/types.h>
#include <sys/socket.h>
#include <signal.h>
#include <unistd.h>

#include "sighandler.h"


int SignalHandler::sighupFd[2] = { -1, -1 };
int SignalHandler::sigtermFd[2] = { -1, -1 };
int SignalHandler::sigintFd[2] = { -1, -1 };

void SignalHandler::hupSigHandler(int unused)
{
	Q_UNUSED(unused)

	if(sighupFd[0] < 0)
		return;

	char a = 1;
	::write(sighupFd[0], &a, sizeof(a));
}

void SignalHandler::termSigHandler(int unused)
{
	Q_UNUSED(unused)

	if(sigtermFd[0] < 0)
		return;

	char a = 1;
	::write(sigtermFd[0], &a, sizeof(a));
}

void SignalHandler::intSigHandler(int unused)
{
	Q_UNUSED(unused)

	if(sigintFd[0] < 0)
		return;

	char a = 1;
	::write(sigintFd[0], &a, sizeof(a));
}

SignalHandler::SignalHandler(QObject *parent)
	:QObject(parent)
{
	if(sigintFd[1] < 0 && !setup())
	{
		qCritical("Failed setting up signal handler");
		return;
	}

	snHup = new QSocketNotifier(sighupFd[1], QSocketNotifier::Read, this);
	snTerm = new QSocketNotifier(sigtermFd[1], QSocketNotifier::Read, this);
	snInt = new QSocketNotifier(sigintFd[1], QSocketNotifier::Read, this);

	connect(snHup, SIGNAL(activated(int)), this, SIGNAL(sigHup()));
	connect(snHup, SIGNAL(activated(int)), this, SIGNAL(sigQuitApp()));

	connect(snTerm, SIGNAL(activated(int)), this, SIGNAL(sigTerm()));
	connect(snTerm, SIGNAL(activated(int)), this, SIGNAL(sigQuitApp()));

	connect(snInt, SIGNAL(activated(int)), this, SIGNAL(sigInt()));
	connect(snInt, SIGNAL(activated(int)), this, SIGNAL(sigQuitApp()));
}

SignalHandler::~SignalHandler()
{}

bool SignalHandler::setup()
{
	struct sigaction hupAct, termAct, intAct;

	if(::socketpair(AF_UNIX, SOCK_STREAM, 0, sighupFd))
		goto error;

	if(::socketpair(AF_UNIX, SOCK_STREAM, 0, sigtermFd))
		goto error;

	if(::socketpair(AF_UNIX, SOCK_STREAM, 0, sigintFd))
		goto error;

	hupAct.sa_handler = &SignalHandler::hupSigHandler;
	::sigemptyset(&hupAct.sa_mask);
	hupAct.sa_flags = SA_RESTART;

	if(::sigaction(SIGHUP, &hupAct, 0))
		goto error;

	termAct.sa_handler = &SignalHandler::termSigHandler;
	::sigemptyset(&termAct.sa_mask);
	termAct.sa_flags = SA_RESTART;

	if(::sigaction(SIGTERM, &termAct, 0))
		goto error;

	intAct.sa_handler = &SignalHandler::intSigHandler;
	::sigemptyset(&intAct.sa_mask);
	intAct.sa_flags = SA_RESTART;

	if(::sigaction(SIGINT, &intAct, 0))
		goto error;

	return true;

error:

	if(sighupFd[0] >= 0)
		::close(sighupFd[0]);

	if(sighupFd[1] >= 0)
		::close(sighupFd[1]);

	if(sigtermFd[0] >= 0)
		::close(sigtermFd[0]);

	if(sigtermFd[1] >= 0)
		::close(sigtermFd[1]);

	if(sigintFd[0] >= 0)
		::close(sigintFd[0]);

	if(sigintFd[1] >= 0)
		::close(sigintFd[1]);

	sighupFd[0] = -1;
	sighupFd[1] = -1;
	sigtermFd[0] = -1;
	sigtermFd[1] = -1;
	sigintFd[0] = -1;
	sigintFd[1] = -1;

	return false;
}
