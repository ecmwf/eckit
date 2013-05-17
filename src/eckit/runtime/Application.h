/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File Application.h
// Baudouin Raoult - ECMWF May 96

#ifndef eckit_Application_h
#define eckit_Application_h

#include <stdlib.h>
#include <exception>

#include "eckit/bases/Loader.h"
#include "eckit/config/Configurable.h"
#include "eckit/config/Resource.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/log/Log.h"
#include "eckit/os/BackTrace.h"
#include "eckit/os/Semaphore.h"
#include "eckit/runtime/Application.h"
#include "eckit/runtime/Context.h"
#include "eckit/runtime/Monitor.h"
#include "eckit/runtime/StandardBehavior.h"
#include "eckit/runtime/Task.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class Stream;

//-----------------------------------------------------------------------------

struct DefaultLogging
{
    void setup() {}
};

//-----------------------------------------------------------------------------

struct DefaultLocations
{
    void setup() {}
};

//-----------------------------------------------------------------------------

struct NoMonitor
{
    void start() {}
    void stop() {}
};

struct DoMonitor
{
    void start() 
    {
        try // activate monitor
        {
            Monitor::active( true );
            Monitor::instance().startup();
            Context::instance().self( Monitor::instance().self() );
        }
        catch(exception& e) {
            Log::error() << "** " << e.what() << " Caught in " << Here() <<  std::endl;
            Log::error() << "** Exception is re-thrown" << std::endl;
            throw;
        }
    }
    
    void stop() {  Monitor::instance().shutdown(); }
    
};

//-----------------------------------------------------------------------------

struct NoSignalRegist
{ void regist() {}
    void unregist() {}
};

//-----------------------------------------------------------------------------

struct StdSignalRegist
{
    StdSignalRegist();
    ~StdSignalRegist();
    
    static void catch_terminate();
    static void catch_unexpected();    
    static void catch_new_handler();
    
    bool regist_;
    
    std::new_handler nh;
    std::terminate_handler  th;
    std::unexpected_handler uh;

    void regist() 
    {
        if(regist_) return;
        nh = ::set_new_handler( StdSignalRegist::catch_new_handler );
        th = ::set_terminate( StdSignalRegist::catch_terminate );
        uh = ::set_unexpected( StdSignalRegist::catch_unexpected );
        regist_ = true;
    }
    
    void unregist() 
    {
        if(!regist_) return;
        if(nh) ::set_new_handler( nh );
        if(th) ::set_terminate( th );
        if(uh) ::set_unexpected( uh );
        regist_ = false;
    }
    
};

//-----------------------------------------------------------------------------

template < class LG = DefaultLogging,   ///< policy for logging
           class MN = NoMonitor,        ///< policy for monitoring 
           class LC = DefaultLocations, ///< policy for locations (home,etc...)
           class SH = NoSignalRegist    ///< policy for signal handling
           >
class Application : public Task, 
                    public Configurable {
public:

// -- Contructors

	Application(int argc,char **argv);

// -- Destructor

	virtual ~Application();
    
    static Application<LG,MN,LC,SH>& instance();
    
// -- Methods
	
	virtual void run() = 0; 
	
    /// call this function once to make this application unique on this host
    void unique();

// -- Overridden methods
	
	// From Task

	virtual void start();
	virtual void stop();
	virtual void kill();
	virtual void wait()   {}
	virtual bool active() { return true; }

	string        appName()   { return name_;    }
	bool          running()   { return running_;    }

    time_t        uptime();

	void          terminate();

protected: // methods
	
	// From Configurable

	virtual void   reconfigure();
	virtual string name() const   { return name_; }

private: // members
    
    LG log_policy;
    MN monitor_policy;
    LC location_policy;
    SH signal_policy;
    
    string name_;
	bool   running_;
    
    static Application<LG,MN,LC,SH>* instance_;

	/// overriden from Configurable
	virtual string kind() const  { return "Application"; }

	friend class AppCommand;    ///< in order to call reconfigure
	friend class ResourceBase;
};

//-----------------------------------------------------------------------------

typedef Application<DefaultLogging,NoMonitor,DefaultLocations,NoSignalRegist> DefaultApplication;

//-----------------------------------------------------------------------------

template< class LG, class MN, class LC, class SH >
Application<LG,MN,LC,SH>::Application(int argc,char **argv)
{    
    location_policy.setup(); // location policy first -- sets home()
    
    name_ = PathName(argv[0]).baseName(false);        

    Context& ctxt = Context::instance();
    ctxt.setup( argc, argv );
    ctxt.runName( name_ );
    ctxt.debug( Resource<int>(this,"debug;$DEBUG;-debug",0) );
    
    log_policy.setup();

    monitor_policy.start();
    
	::srand(::getpid() + ::time(0));

    // ensure is uniqe instance of this application
    {
        if(instance_)
            throw SeriousBug("An instance of application already exists");
        instance_ = this;
    }
            
    signal_policy.regist();
	
	Loader::callAll(&Loader::execute);
}

template< class LG, class MN, class LC, class SH >
Application<LG,MN,LC,SH>::~Application()
{
    signal_policy.unregist();
    monitor_policy.stop();
    instance_ = 0;
}

template< class LG, class MN, class LC, class SH >
Application<LG,MN,LC,SH>& Application<LG,MN,LC,SH>::instance()
{
    PANIC(instance_ == 0);
    return *instance_;
}

template< class LG, class MN, class LC, class SH >
void Application<LG,MN,LC,SH>::reconfigure()
{
    Log::info() << "Application::reconfigure" << endl;
    
    int debug = Resource<int>(this,"debug;$DEBUG;-debug",0);
    
    Context::instance().debug( debug );

    Context::instance().reconfigure();  // forward to context
}

template< class LG, class MN, class LC, class SH >
void Application<LG,MN,LC,SH>::start()
{
	int status = 0;

    string displayName = Resource<string>("-name",name_);
    
    Context::instance().displayName( displayName );
    
	Monitor::instance().name( displayName );

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

template< class LG, class MN, class LC, class SH >
void Application<LG,MN,LC,SH>::stop()
{
	::exit(0); // or: throw Stop();
}

template< class LG, class MN, class LC, class SH >
void Application<LG,MN,LC,SH>::kill()
{
	::exit(1);
}

template< class LG, class MN, class LC, class SH >
void Application<LG,MN,LC,SH>::terminate()
{	
	stop();
}

template< class LG, class MN, class LC, class SH >
void Application<LG,MN,LC,SH>::unique()
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

template< class LG, class MN, class LC, class SH >
time_t Application<LG,MN,LC,SH>::uptime()
{
    long taskId = Context::instance().self();
    
	Monitor::TaskArray& info = Monitor::instance().tasks();
	time_t uptime = info[taskId].start();
	time_t now    = ::time(0);

	return now - uptime;
}

//-----------------------------------------------------------------------------

} // namespace eckit

#endif
