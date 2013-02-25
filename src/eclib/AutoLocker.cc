/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/* #define CHECK_DEAD_LOCKS */

#include "eclib/AutoLocker.h"
#include "eclib/Mutex.h"

//-----------------------------------------------------------------------------

namespace eclib {

//-----------------------------------------------------------------------------

#ifdef CHECK_DEAD_LOCKS

typedef map<void*,pthread_t,less<void*> > GotMap;
typedef map<pthread_t,void*,less<pthread_t> > WantMap;
static WantMap*   wantMap = 0;
static GotMap*   gotMap = 0;
static Mutex* mutex = 0;
typedef set<pthread_t,less<pthread_t> > Set;

static pthread_once_t once = PTHREAD_ONCE_INIT;

static void lock()
{
	 mutex->lock();
}

static void unlock()
{
	mutex->unlock();
}

static void init(void)
{
	mutex   = new Mutex;
	wantMap = new WantMap;
	gotMap  = new GotMap;
	pthread_atfork(lock,unlock,unlock);
}

void AutoLocker::want(void* resource)
{
	pthread_once(&once,init);
	mutex->lock();

	cerr << "AutoLocker " << pthread_self() << " want " << resource << endl;

	GotMap::iterator i = gotMap->find(resource);

	if( i != gotMap->end())
	{
		if((*i).second != pthread_self())
		{
			(*wantMap)[pthread_self()] = resource;
			analyse(resource);
		}
	}

	mutex->unlock();
}

void AutoLocker::got(void* resource)
{
	mutex->lock();
	cerr << "AutoLocker " << pthread_self() << " got " << resource << endl;
	(*gotMap)[resource] = pthread_self();
	wantMap->erase(pthread_self());
	mutex->unlock();
}

void AutoLocker::release(void* resource)
{
	mutex->lock();
	cerr << "AutoLocker " << pthread_self() << " release " << resource << endl;
	gotMap->erase(resource);
	mutex->unlock();
}

static void visit(pthread_t p, Set& s,void *resource)
{
	if(s.find(p) != s.end())
	{
		cerr << "Deadlock detected"  << endl;
		::pause();

	}
	
	s.insert(p);
	
	GotMap::iterator i = gotMap->find(resource);
	pthread_t q = (*i).second; // The one with the resource

	WantMap::iterator j = wantMap->find(q);
	if(j != wantMap->end())
		visit(q,s,(*j).second);

	s.erase(p);
}

void AutoLocker::analyse(void *resource)
{
	Set set;
	visit(pthread_self(),set,resource);
}

#else

void AutoLocker::want(void* resource)
{
}

void AutoLocker::got(void* resource)
{
}

void AutoLocker::release(void* resource)
{
}

void AutoLocker::analyse(void* resource)
{
}

//-----------------------------------------------------------------------------

} // namespace eclib

#endif
