/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File Monitor.h
// Baudouin Raoult - ECMWF Nov 96

#ifndef eckit_Monitor_h
#define eckit_Monitor_h

#include "eclib/machine.h"

#include "eckit/thread/AutoLock.h"
#include "eckit/container/MappedArray.h"
#include "eckit/memory/NonCopyable.h"
#include "eckit/runtime/TaskInfo.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class Monitor : private NonCopyable {
public:

	typedef MappedArray<TaskInfo> TaskArray;

    /// Contructors

	Monitor();

    /// Destructor

	~Monitor(); // Change to virtual if base class

    static bool active();
    static void active( bool a );
    
	static void startup();
	static void shutdown();
	static void out(char*,char*); // called from Log

    static void name(const string&);
	static void kind(const string&);

	static void progress(const string&,unsigned long long,unsigned long long);
	static void progress(unsigned long long);
	static void progress();

	static char state(char);

	static void message(const string&);
	static string message();

	static void status(const string&);
	static string status();
	static string statusTree();
	static void stoppable(bool);
	static bool stopped();

	static TaskArray& info();
	static long       self();
	static void parent(long);

	static void start(const string&);
	static int  kill(const string& , int = 15);

	static void port(int );
	static int  port();

	static void    host(const string& );
	static string  host();

	static MarsID marsID();
	static void marsID(const MarsID&);

	static void show(bool);

	static void cancel(const string&);
	static string cancelMessage();
	static bool  canceled();

	static void abort();
	static void checkAbort();

private: // members

	unsigned long slot_;
	bool ready_;
	bool check_;

private: // methods

	unsigned long hash();
	static Monitor& instance();
	TaskInfo* operator->();

	void init();

};

//-----------------------------------------------------------------------------

// Wrap the setting of Monitor::state

class AutoState {
	char old_;
public:
	AutoState(char c): 
		old_(Monitor::state(c)) {}

	~AutoState()
		{ Monitor::state(old_); }

	void set(char c) 
		{ Monitor::state(c); }
};

//-----------------------------------------------------------------------------

template<class T>
class AutoLockTag {
	AutoState state_;
	AutoLock<T> lock_;
public:
    AutoLockTag(T& t): state_(t.tag()), lock_(t) 
		{ state_.set(t.tag() - 'a' + 'A'); }
    AutoLockTag(T* t): state_(t->tag()), lock_(t) 
		{ state_.set(t->tag() - 'a' + 'A'); }
};

//-----------------------------------------------------------------------------

} // namespace eckit

#endif
