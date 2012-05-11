/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <stdlib.h>

#include "eclib/Application.h"
#include "eclib/DHSBehavior.h"
#include "eclib/Log.h"
#include "eclib/Monitor.h"
#include "eclib/PathName.h"
#include "eclib/Semaphore.h"
#include "eclib/Loader.h"

string  Application::name_  = "unknown";
Application* Application::instance_ = 0;

bool    Application::running_  = false;

static char* memoryReserve;

static void end(const char* msg)
{
    static bool in_end = false;

    if(in_end) {
        fprintf(stderr,"terminate called twice\n");
        _exit(1);
    }

    in_end = true;

	delete[] memoryReserve; memoryReserve = 0;
	Log::monitor(Log::Other,9998) << msg << endl;

	try {
		throw;
	}
	catch(exception& e)
	{
        Log::panic() << "!!!!!!!!!!!!!!!!!! " ;
		Log::panic() << msg <<  " with the exception:" << endl;
		Log::panic() << e.what() << endl;
	//	Exception::exceptionStack(Log::panic());
	}
    _exit(1);
	//Panic(msg);
}

static void catch_terminate()
{
	end("Terminate was called");
}

static void catch_unexpected()
{
	end("Unexpected was called");
}

static void catch_new_handler()
{
	delete[] memoryReserve; memoryReserve = 0;
	throw OutOfMemory();
}

static bool abortExit = true;

static void catch_exit()
{
	if(abortExit)
	{
		Log::monitor(Log::Ostore,9999) << "Exit called, check ostore" << endl;
		Panic("Exit was called");
	}
}

Application::Application(int argc,char **argv)
{
	name_ = PathName(argv[0]).baseName(false);
    
    Context::instance().setup( argc, argv, new DHSBehavior() );
    
	// mallopt(M_DISCLAIM,0);
	::srand(::getpid() + ::time(0));

	memoryReserve = new char[20*1024];

	if(instance_)
		throw SeriousBug("An instance of application already exists");

    instance_ = this;
    
    // TODO: maybe this should also be part of the ContextBehavior
    
	set_new_handler(catch_new_handler);
	set_terminate(catch_terminate);
	set_unexpected(catch_unexpected);
	
	Loader::callAll(&Loader::execute);
}

Application::~Application()
{
	instance_ = 0;
	abortExit = false;
}

void Application::reconfigure()
{
    Log::info() << "Application::reconfigure" << endl;
    
    // forward to context
    Context::instance().reconfigure(); 
}

Application& Application::instance()
{
	PANIC(instance_ == 0);
	return *instance_;
}

void Application::start()
{
	int status = 0;

	Monitor::name(Resource<string>("-name",name_));

	Log::info() << "** Start of " << name() << " ** pid is " << getpid() << endl;
	
	try {
		Log::status() << "Running" << endl;
        running_ = true;
		run();
        running_ = false;
	}
	catch(exception& e){
		status = 1;
		Log::error() << "** " << e.what() << " Caught in "  << Here() <<  endl;
		Log::error() << "** Exception is terminates " << name() << endl;
	}


    Log::info() << "** End of " << name() << " (" << Context::instance().argv(0) << ")  **"  << endl;
	
	::exit(status);
}

void Application::stop()
{
	::exit(0); // or: throw Stop();
}

void Application::kill()
{
	::exit(1);
}

void Application::terminate()
{
	PANIC(instance_ == 0);
	abortExit = false;	
	instance_->stop();
}

void Application::unique()
{
	PathName  lockFile("~/locks/" + name());

	if(!lockFile.exists())
		lockFile.touch();	

	Semaphore* sem = new Semaphore(lockFile);

	if(sem->test())
	{
		ifstream os(lockFile.localPath());
		string s;
		os >> s;
		throw SeriousBug("Application " + name() + " is already running as pid " + s);
	}

	sem->lock(); // OS should reset the semaphore

	ofstream os(lockFile.localPath());
	os << getpid();
}

time_t Application::uptime()
{
	Monitor::TaskArray& info = Monitor::info();
	time_t uptime = info[Context::instance().self()].start();
	time_t now    = ::time(0);

	return now - uptime;
}
