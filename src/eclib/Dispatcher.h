/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File Dispatcher.h
// Baudouin Raoult - ECMWF Jul 96

#ifndef eckit_Dispatcher_h
#define eckit_Dispatcher_h

#include "eclib/machine.h"

#include "eclib/config/Configurable.h"
#include "eckit/thread/Mutex.h"
#include "eckit/thread/MutexCond.h"
#include "eclib/Resource.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class JSON;

template<class Request>
struct DefaultHandler {
	void pick(list<Request*>&,vector<Request*>&);
	void idle() {}
static void print(ostream&,const Request&);
static void json(JSON&,const Request&);
};

template<class Request>
struct DequeuePicker {
	virtual void pick(list<Request*>&) = 0;
};

//=======================================
// A 'T' must have a default constructor a operator()(R*)
//

template<class Traits>
class Dispatcher : public Configurable  {
public:

	typedef typename Traits::Request   Request;
	typedef typename Traits::Handler   Handler;

// -- Contructors

	Dispatcher(const string& name = Traits::name() );

// -- Destructor

	~Dispatcher();

// -- Operators

	// Dispatcher takes ownership of Request

	void push(Request*);
	void push(const vector<Request*>&);
	bool next(Handler&,vector<Request*>&,Mutex&);
	void dequeue(DequeuePicker<Request>&);
	void sleep();
	void sleep(int);
	void awake();
	int  size();
	void running(long);
	long running();
    void json(JSON&) const;

	// From Configurable

	virtual string name() const  { return name_; }

protected:

// -- Members

	list<Request*>    queue_;
	Resource<long>    maxTasks_;
	long              count_;
	long              next_;
	MutexCond         ready_;
	MutexCond         wait_;
	string            name_;
	bool           	  grow_;
	long              running_;

	Mutex             lock_;

private:

// No copy allowed

	Dispatcher(const Dispatcher<Traits>&);
	Dispatcher<Traits>& operator=(const Dispatcher<Traits>&);

	// -- Methods

	void print(ostream&) const;
	void changeThreadCount(int delta);
	void _push(Request*);

	// From Configurable

	virtual string kind() const  { return "Dispatcher"; }
	virtual void reconfigure();

    friend ostream& operator<<(ostream& s,const Dispatcher<Traits>& p)
		{ p.print(s); return s; }

};

//-----------------------------------------------------------------------------

} // namespace eckit

#include "eclib/Dispatcher.cc"

#endif
