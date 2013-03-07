/*
 * (C) Copyright 1996-2012 ECMWF.
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

#include "eclib/Context.h"
#include "eclib/Configurable.h"
#include "eclib/Task.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class Stream;

class Application : public Task, 
                    public Configurable {
public:

// -- Contructors

	Application(int argc,char **argv);

// -- Destructor

	virtual ~Application();

// -- Methods
	
	virtual void run() = 0; 
	void unique();

// -- Overridden methods
	
	// From Task

	virtual void start();
	virtual void stop();
	virtual void kill();
	virtual void wait()   {}
	virtual bool active() { return true; }

// -- Class methods

	static string        appName()   { return name_;    }
	static bool          running()   { return running_;    }

//	static bool         exited()    { return instance_ != 0; }

	static time_t       uptime();

	static void         terminate();

protected:


// -- Overridden methods
	
	// From Configurable

	virtual void   reconfigure();
	virtual string name() const   { return name_; }

// -- Class methods

	static Application& instance();

private:

// -- Members
    
    static string name_;

	static bool   running_;

// -- Overridden methods

	// From Configurable

	virtual string kind() const  { return "Application"; }

// -- Class members

	static Application* instance_;

	friend class AppCommand; // in order to call reconfigure
	friend class ResourceBase;
};

//-----------------------------------------------------------------------------

} // namespace eckit

#endif
