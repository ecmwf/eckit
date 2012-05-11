/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <pthread.h>
#include <signal.h>

#include "eclib/AutoLock.h"
#include "eclib/AutoLock.h"
#include "eclib/Monitor.h"
#include "eclib/PathName.h"
#include "eclib/Resource.h"
#include "eclib/StrStream.h"
#include "eclib/TaskInfo.h"
#include "eclib/ThreadSingleton.h"

// static Mutex* mutex = new Mutex;

static bool active_ = (getenv("DHSHOME") != 0);


Monitor::Monitor():
	ready_(false),
	check_(false)
{ 
}

void Monitor::init()
{
	if(ready_) return;

	if(!active_) {
		ready_ = true;
		return;
	}

    //  Monitor is being inited twice...
	
	if(check_) ::abort();

	check_ = true;

	TaskArray& a = info();

	AutoLock<TaskArray> lock(a);

	int slot = hash() % a.size();

	bool found = false;

	for(bool check = false; !check && !found ; check = true)
	{
		for(int i = 0 ; i < a.size() && !found ; i++)
		{
			slot_ = (slot+i) % a.size();
			if(!a[slot_].busy(check))
			{
				new(&a[slot_]) TaskInfo();
				found = true;
			}
		}
	}

    if(!found) {
    cout << "No free monitor slots" << endl;
    cerr << "No free monitor slots" << endl;
    }

	// No free monitor slots

	PANIC(!found);

	check_ = false;
	ready_ = true;
}

Monitor::~Monitor() 
{ 
	if(ready_ && active_) {
		TaskArray& a = info();
		AutoLock<TaskArray> lock(a);
		a[slot_].TaskInfo::~TaskInfo();
	}
}

void Monitor::startup()
{
	if(!active_) return;
	// Create the instance	
	Monitor::instance();
}

void Monitor::shutdown()
{
	if(!active_) return;

	TaskArray& a = info();
	AutoLock<TaskArray> lock(a);

	pid_t pid = getpid();

	for(int i = 0; i < a.size() ; i++)
		if(a[i].pid() == pid)
			a[i].TaskInfo::~TaskInfo();

	// should unmap
}

unsigned long Monitor::hash()
{
	return (((unsigned long)pthread_self() << 16) | (unsigned long)getpid());
}

//============================================================================

template class ThreadSingleton<Monitor>;

static ThreadSingleton<Monitor> monitor;

Monitor& Monitor::instance()
{
	Monitor& m = monitor.instance();
	if(!m.ready_) m.init();
	return m;
}

TaskInfo* Monitor::operator->()
{
	if(!active_)
	{
		static TaskInfo info;
		return &info;
	}

	Monitor& m = Monitor::instance(); // insure is initialized

	TaskArray& a = info();
	return &a[slot_];
}


long Monitor::self()
{
	if(!active_) return 0;

	Monitor& m =  monitor.instance();
	if(!m.ready_)  return -1;
	return instance().slot_;	
}

void Monitor::out(char* from,char *to)
{
	Monitor& m =  monitor.instance();
	instance()->out(from,to);
}


static Monitor::TaskArray* mapArray = 0;
static pthread_once_t once = PTHREAD_ONCE_INIT;

static void init(void)
{
    string  monitor = Resource<string>("monitorPath","~/etc/monitor");
	long    size    = Resource<long>("monitorSize",1000);
    mapArray = new Monitor::TaskArray(monitor,size);
}

Monitor::TaskArray& Monitor::info()
{
    pthread_once(&once, ::init);
	return *mapArray;
}

void Monitor::name(const string& s) 
{
	instance()->name(s);
}

void Monitor::kind(const string& s) 
{
	instance()->kind(s);
}

void Monitor::progress(const string& name,
	unsigned long long min,unsigned long long max)
{
	instance()->progressName(name);
	instance()->start(min,max);
}

void Monitor::progress(unsigned long long value)
{
	instance()->progress(value);
}

void Monitor::progress()
{
	instance()->done();
}

char Monitor::state(char c)
{
	char x = instance()->state();
	instance()->state(c);
	return x;
}

void Monitor::status(const string& msg)
{
	Monitor& m =  monitor.instance();
	if(!m.ready_) return;
	instance()->status(msg);
}

string Monitor::status()
{
	Monitor& m =  monitor.instance();
	if(!m.ready_) return string();
	return instance()->status();
}

void Monitor::message(const string& msg)
{
	Monitor& m =  monitor.instance();
	if(!m.ready_) return;
	instance()->message(msg);
}

string Monitor::message()
{
	Monitor& m =  monitor.instance();
	if(!m.ready_) return string();
	return instance()->message();
}

void Monitor::stoppable(bool b)
{
	instance()->stoppable(b);
}

bool Monitor::stopped()
{
	return instance()->stopped();
}

void Monitor::abort()
{
	instance()->abort();
}

void Monitor::checkAbort()
{
	instance()->checkAbort();
}

void Monitor::parent(long p)
{
	instance()->parent(p);
}


string Monitor::statusTree()
{
    StrStream os;
    os << status();
    int n = self();

	TaskArray& p = info();

	for(int j = 0 ; j < p.size(); j++)
		if((p[j].parent() == n) && p[j].show() && p[j].busy(true))
		{
            os << "|" << p[j].status();
		}

    os << StrStream::ends;

    return string(os);
}

void Monitor::start(const string& app)
{
	if(!active_) return;

	TaskArray& p = info();

	for(int j = 0 ; j < p.size(); j++)
		if(p[j].busy(true) && app == p[j].application() && 
			p[j].depth() == 0)
		{
			Log::warning() << app << " is already running with a pid of " 
				<< p[j].pid() << endl;
			Log::warning() << "Please stop it first" << endl;
			return;
		}

	PathName cmd = Resource<PathName>("startCmd","~/admin/starter");

	string s = string(cmd) + " " + app;

	Log::info() << "Executing shell command: " << s << endl;

	SYSCALL(::system(s.c_str()));

}

void Monitor::port(int p)
{
	instance()->port(p);
}

int Monitor::port() 
{
	return instance()->port();
}

void Monitor::host(const string& h)
{
	instance()->host(h);
}

string Monitor::host() 
{
	return instance()->host();
}

void Monitor::marsID(const MarsID& id)
{
	instance()->marsID(id);
}

MarsID Monitor::marsID() 
{
	return instance()->marsID();
}

void Monitor::show(bool on)
{
	instance()->show(on);
}

int Monitor::kill(const string& name, int sig)
{

	if(!active_) return 0;

	Monitor::TaskArray& info = Monitor::info();
	pid_t      me   = ::getpid();
	int        n    = 0;

	// Name. Look for Unix process ID
	for(int i = 0; i<info.size(); ++i)
		if(info[i].busy(true) && info[i].application() == name)
		{
			pid_t pid = info[i].pid();
			if(pid == me)
				Log::info() << pid << ": Suicide avoided ;-)" << endl;
			else
			{
				if(::kill(pid,sig))
					Log::info() << Log::syserr << endl; 
				else
				{
					Log::info() << pid << ": Killed" << endl;
					n++;
				}
			}
		}

	return n;
}
