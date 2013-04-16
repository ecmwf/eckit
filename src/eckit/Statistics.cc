/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/thread/AutoLock.h"
#include "eckit/runtime/Context.h"
#include "eckit/os/Semaphore.h"
#include "eckit/Statistics.h"
#include "eckit/log/TimeStamp.h"


//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------


#ifdef COME_BACK

static Semaphore *sem  = 0;
static PathName  *stats  = 0;

static pthread_once_t once = PTHREAD_ONCE_INIT;

static void init(void)
{
	stats = new PathName("~/stat/statistics");
	sem  = new Semaphore(*stats);
}
#endif

void Statistics::readFile(const MarsID& id,const PathName& path,
	const Length& length)
{
#ifdef COME_BACK

	static const char* here = __FUNCTION__;

	pthread_once(&once,init);

	try {
		AutoLock<Semaphore> lock(sem);

		string fs  = path.fileSystem();
		Length size = path.size();
		long age    = ::time(0) - path.lastAccess();

		ofstream out(stats->c_str(),ios::app);
		out << TimeStamp("%Y%m%d:%H%M%S") 
			<< ",request=" << id
			<< ",application=" << Context::instance().runName()
			<< ",action=read_file"
			<< ",filesystem=" << fs
			<< ",path="       << path
			<< ",length="     << length      
			<< ",size="       << size
			<< ",age="        << age
			<< endl;
	} 
	catch(exception& e)
	{
	   Log::error() << "** " << e.what() << " Caught in " << here <<  endl;
	   Log::error() << "** Exception is ignored" << endl;
	}
#endif
}

void Statistics::createFile(const MarsID& id,const PathName& path,
	const Length& length,const string& source)
{
#ifdef COME_BACK

	static const char* here = __FUNCTION__;

	pthread_once(&once,init);

	try {
		AutoLock<Semaphore> lock(sem);

		string fs   = path.fileSystem();

		ofstream out(stats->c_str(),ios::app);
		out << TimeStamp("%Y%m%d:%H%M%S") 
			<< ",request=" << id 
			<< ",application=" << Context::instance().runName()
			<< ",action=create_file"
			<< ",filesystem=" << fs
			<< ",path="       << path
			<< ",length="     << length 
			<< ",source="     << source
			<< endl;

	} 
	catch(exception& e)
	{
	   Log::error() << "** " << e.what() << " Caught in " << here <<  endl;
	   Log::error() << "** Exception is ignored" << endl;
	}
#endif
}

void Statistics::deleteFile(const PathName& path)
{
#ifdef COME_BACK         


	static const char* here = __FUNCTION__;

	pthread_once(&once,init);

	try {
		AutoLock<Semaphore> lock(sem);

		string fs  = path.fileSystem();
		Length size = path.size();
		long age    = ::time(0) - path.lastAccess();

		ofstream out(stats->c_str(),ios::app);
		out << TimeStamp("%Y%m%d:%H%M%S") 
			<< ",application=" << Context::instance().runName()
			<< ",action=delete_file"
			<< ",filesystem=" << fs
			<< ",path="       << path
			<< ",size="       << size
			<< ",age="        << age
			<< endl;
	} 
	catch(exception& e)
	{
	   Log::error() << "** " << e.what() << " Caught in " << here <<  endl;
	   Log::error() << "** Exception is ignored" << endl;
	}
#endif
}

//-----------------------------------------------------------------------------

} // namespace eckit

