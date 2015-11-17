/*
 * (C) Copyright 1996-2015 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/* #define CHECK_DEAD_LOCKS */

#include "eckit/thread/AutoLocker.h"
#include "eckit/thread/Mutex.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

#ifdef CHECK_DEAD_LOCKS

typedef std::map<void*,pthread_t,std::less<void*> > GotMap;
typedef std::map<pthread_t,void*,std::less<pthread_t> > WantMap;
static WantMap*   wantMap = 0;
static GotMap*   gotMap = 0;
static Mutex* local_mutex = 0;
typedef std::set<pthread_t,std::less<pthread_t> > Set;

static pthread_once_t once = PTHREAD_ONCE_INIT;

static void lock()
{
	 local_mutex->lock();
}

static void unlock()
{
	local_mutex->unlock();
}

static void init(void)
{
	local_mutex   = new Mutex;
	wantMap = new WantMap;
	gotMap  = new GotMap;
	pthread_atfork(lock,unlock,unlock);
}

void AutoLocker::want(void* resource)
{
	pthread_once(&once,init);
	local_mutex->lock();

	cerr << "AutoLocker " << pthread_self() << " want " << resource << std::endl;

	GotMap::iterator i = gotMap->find(resource);

	if( i != gotMap->end())
	{
		if((*i).second != pthread_self())
		{
			(*wantMap)[pthread_self()] = resource;
			analyse(resource);
		}
	}

	local_mutex->unlock();
}

void AutoLocker::got(void* resource)
{
	local_mutex->lock();
	cerr << "AutoLocker " << pthread_self() << " got " << resource << std::endl;
	(*gotMap)[resource] = pthread_self();
	wantMap->erase(pthread_self());
	local_mutex->unlock();
}

void AutoLocker::release(void* resource)
{
	local_mutex->lock();
	cerr << "AutoLocker " << pthread_self() << " release " << resource << std::endl;
	gotMap->erase(resource);
	local_mutex->unlock();
}

static void visit(pthread_t p, Set& s,void *resource)
{
	if(s.find(p) != s.end())
	{
		std::cerr << "Deadlock detected"  << std::endl;
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

#endif

//-----------------------------------------------------------------------------

} // namespace eckit
