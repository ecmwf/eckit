/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <signal.h>
#include <sys/wait.h>

// WARNING: on linux, threads are different processes,
// so a thread cannot call waitpid on a process forked by an other
// thread....

#if 0

#ifndef linux
#include <sys/proc.h>
#endif


#ifdef IBM
#pragma priority(-100)
#include <procinfo.h>
//extern "C" int getprocs(struct procsinfo*,int,struct fdsinfo*,int,pid_t*,int);

#else
#include <sys/procfs.h>
#endif
#endif

#include "eclib/machine.h"

#include "eclib/Log.h"
#include "eclib/Monitor.h"
#include "eclib/ProcessControler.h"
#include "eclib/Thread.h"
#include "eclib/ThreadControler.h"

static pthread_once_t once = PTHREAD_ONCE_INIT;

ProcessControler::ProcessControler(bool forget):
	ClassExtent<ProcessControler>(this),
	pid_(-1),
	child_(false),
	forget_(forget)
{
}

ProcessControler::~ProcessControler()
{
	if(!forget_ && active()) 
		Log::warning() << "~ProcessControler called while process still active" << endl;
}

void ProcessControler::printStatus(pid_t pid,int status)
{
	Log::info() << "-------- End of " << pid;
	
	if(WIFEXITED(status))
		Log::info() << " exited ";

	if(WEXITSTATUS(status))
		Log::info() << " status " << WEXITSTATUS(status) << ' ';

	if(WIFSIGNALED(status))
		Log::info() << " with signal " << WTERMSIG(status); 

	Log::info() << endl;
}

void ProcessControler::end(result& r)
{
	if(r.pid_ == pid_)
	{
		r.found_ = true;
		status_  = r.status_;
		pid_     = -pid_;
	}
}

// This thread should receive the signals

class ChildReaper : public Thread {
	virtual void run();
};

void ChildReaper::run()
{

	sigset_t newmask, oldmask;
	sigemptyset(&newmask); 
	sigaddset(&newmask, SIGCHLD);
	pthread_sigmask(SIG_UNBLOCK,&newmask,&oldmask);

	Monitor::name("reaper");

	for(;;)
	{
		ProcessControler::result r;
		r.found_ = false;
		Log::status() << "Waiting" << endl;

        Monitor::show(false);
		r.pid_ = ::waitpid(-1, &r.status_, 0);
        Monitor::show(true);

		if(r.pid_ == -1) {
			// Todo: use mutex cond....
            if(errno != ECHILD) 
                Log::error() << "Wait pid " << Log::syserr << endl;
			::sleep(5);
		}
		else
		{
			Log::status() << "End of " << r.pid_ << endl;
			ProcessControler::printStatus(r.pid_,r.status_);
			ProcessControler::callAll(&ProcessControler::end,r);
		}
	}

}

static void init()
{
	ThreadControler ctl(new ChildReaper());
	ctl.start();

/*
	// For now, ignore the sig childs...
	signal(SIGCHLD, SIG_IGN);
*/
}

void ProcessControler::start()
{
	static const char *here = __FUNCTION__;

	pthread_once(&once,init);

	sigset_t newmask, oldmask;
	sigemptyset(&newmask);
	sigaddset(&newmask, SIGCHLD);
	pthread_sigmask(SIG_BLOCK,&newmask,&oldmask);

	switch(pid_ = ::fork())
	{
		case 0:

			pid_ = ::getpid();
			child_ = true;

			//Log::info() << "Starting new process with a pid of " << pid_ << endl;

			try {
				run();
			}
			catch(exception& e){
				Log::error() << "** " << e.what() << " Caught in " 
					<< here <<  endl;
				Log::error() << "** Exception is terminate process " 
							 << pid_ << endl;
			}


			::exit(0);
			break;

		case -1:
			Log::error() << "Cannot fork " << Log::syserr << endl;
			throw FailedSystemCall("fork");
			break;

		default:
			break;

	}

	sigemptyset(&newmask); 
	sigaddset(&newmask, SIGCHLD);
	pthread_sigmask(SIG_UNBLOCK,&newmask,&oldmask);

}

void ProcessControler::stop()
{
	if(!active()) return;

	Log::info() << "ProcessControler::stop " << child_ << '-' << pid_ << endl;
	if(child_)
		::exit(0);
	else
		::kill(pid_,SIGTERM);

	//pid_ = -1;
}

void ProcessControler::kill()
{
	stop(); //For the time being....
}

void ProcessControler::wait()
{
	int status;

	if(!active())
		return;

	Log::info() << "ProcessControler::wait " << pid_ << " " << child_ << endl;
	if(pid_ != -1 && !child_)
	{
		pid_t pid = ::waitpid(pid_,&status,0);

		if(pid != pid_)
		{
			Log::error() << "Wait pid returns " << errno << ' ' <<pid << endl;
			Log::error() << Log::syserr << endl;
		}
		else
		{

			printStatus(pid,status);

			pid_    = -1;
			status_ = status;
		}
	}
}

bool ProcessControler::active()
{
	if(pid_ < 0 && pid_ != -1)
	{
		printStatus(-pid_,status_);
		pid_ = -1;
	}
	return pid_ != -1;
}


bool ProcessControler::isRunning(pid_t pid)
{
#ifdef IBM
	pid_t p = ::getsid(pid);
	if(p == -1)
	{
		if(errno == EPERM)
			return true;

		if(errno == ESRCH)
			return false;

		throw FailedSystemCall("getsid");
	}
	return true;
#else

	char buf[1024];
	sprintf(buf,"/proc/%d",pid);
	return access(buf,F_OK) == 0;

#endif

}
