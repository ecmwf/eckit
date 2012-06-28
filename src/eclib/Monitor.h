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

#ifndef Monitor_H
#define Monitor_H

#include "eclib/machine.h"

#include "eclib/AutoLock.h"
#include "eclib/MappedArray.h"
#include "eclib/TaskInfo.h"

class Monitor {
public:

	typedef MappedArray<TaskInfo> TaskArray;

// -- Exceptions
	// None

// -- Contructors

	Monitor();

// -- Destructor

	~Monitor(); // Change to virtual if base class

// -- Convertors
	// None

// -- Operators
	// None

// -- Methods


// -- Overridden methods
	// None

// -- Class members
	// None

// -- Class methods
	// None

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


protected:

// -- Members
	// None

// -- Methods
	
	// void print(ostream&) const; // Change to virtual if base class	

// -- Overridden methods
	// None

// -- Class members
	// None

// -- Class methods
	// None

private:

// No copy allowed

	Monitor(const Monitor&);
	Monitor& operator=(const Monitor&);

// -- Members

	unsigned long slot_;
	bool ready_;
	bool check_;

// -- Methods

	unsigned long hash();
	static Monitor& instance();
	TaskInfo* operator->();

	void init();

// -- Overridden methods
	// None

// -- Class members
	// None


// -- Class methods
	// None

// -- Friends

	//friend ostream& operator<<(ostream& s,const Monitor& p)
	//	{ p.print(s); return s; }

};


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

#endif
