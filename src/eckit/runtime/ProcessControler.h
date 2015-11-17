/*
 * (C) Copyright 1996-2015 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File ProcessControler.h
// Baudouin Raoult - ECMWF May 96

#ifndef eckit_ProcessControler_h
#define eckit_ProcessControler_h

#include "eckit/container/ClassExtent.h"
#include "eckit/runtime/Task.h"


//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class ProcessControler : public Task, 
                         public ClassExtent<ProcessControler> {
public:

	struct result { bool found_; pid_t pid_; int status_; };

// -- Contructors

	ProcessControler(bool forget = false);

// -- Destructor

	~ProcessControler();

// -- Methods

	virtual void run() = 0;

// -- Overridden methods

	// From Task

	virtual void start();
	virtual void stop();
	virtual void kill();
	virtual void wait();
	virtual bool active();

	// Class-methods

	static bool isRunning(pid_t);
    

private:

// -- Members

	pid_t pid_;
	bool  child_;
	int   status_;
	bool forget_;

// -- Methods

	void end(result&);

// -- Class methods

	static void* waitChild(void*);
	static void sigChild(int);
	static void printStatus(pid_t, int);

	friend class ChildReaper;

};

//-----------------------------------------------------------------------------

} // namespace eckit


#endif
