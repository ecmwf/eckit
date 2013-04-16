/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <signal.h>
#include <sys/time.h>

#include "eckit/runtime/Context.h"
#include "eckit/runtime/Monitor.h"
#include "eclib/ProcessControler.h"
#include "eckit/runtime/TaskInfo.h"
#include "eclib/Timer.h"
#include "eclib/SignalHandler.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

TaskInfo::TaskInfo()
{
	ASSERT(busy_ == false);

	eckit::zero(*this);

	pid_    = getpid();
	thread_ = pthread_self();
	pos_    = 0;
	start_  = ::time(0);
	strncpy(name_,Context::instance().runName().c_str(),sizeof(name_)-1);
	strcpy(kind_,name_);
	strcpy(application_,name_);
	strcpy(status_,"Starting");
	show_ = true; 
	start(0,0);
	busy_   = true;
	stoppable_ = true;
	parent_  = -1;
	state_ = ' ';
}


TaskInfo::~TaskInfo()
{
	busy_ = false;
}

void TaskInfo::kind(const string& s)
{
	touch();
	strncpy(kind_,s.c_str(),sizeof(kind_)-1);
}

void TaskInfo::name(const string& s)
{
	touch();
	strncpy(name_,s.c_str(),sizeof(name_)-1);
}

void TaskInfo::status(const string& s)
{
	touch();
	strncpy(status_,s.c_str(),sizeof(status_)-1);
}

void TaskInfo::message(const string& s)
{
	touch();
	zero(message_);
	strncpy(message_,s.c_str(),sizeof(message_)-1);
}

void TaskInfo::progressName(const string& s)
{
	touch();
	strncpy(progress_.name_,s.c_str(),sizeof(progress_.name_)-1);
}

void TaskInfo::out(char* from,char *to)
{
	touch();
	for(char *p = from; p != to ; p++)
		buffer_[(pos_++)%size_] = *p;
}

unsigned long TaskInfo::text(char *buf,unsigned long max,unsigned long& pos) const
{
	unsigned long len = pos_ - pos;
	if(len > size_) len = size_;
	if(len > max  ) len = max;

	unsigned long p = pos_ - len + size_;
	unsigned long n = len;

	while(n-->0) *buf++ = buffer_[(p++)%size_];

	pos = pos_;
	
	return len;
}

bool TaskInfo::busy(bool check)
{
	if(!busy_) 
		return false;

	time_t now = ::time(0);

	// After 2 minutes, force the check
	if( (now - check_) > 120)
		check = true;

	check_ = now;

	if(!check) 
		return busy_;

	// Check first

	if(!ProcessControler::isRunning(pid_))
	{
		this->TaskInfo::~TaskInfo();
		return false;
	}

	return true;
}

void TaskInfo::start(unsigned long long min,unsigned long long max)
{
	progress_.rate_     = progress_.speed_ = 0;
	progress_.min_      = min;
	progress_.max_      = max;
	progress_.val_      = min;
	gettimeofday(&progress_.start_,0);
	gettimeofday(&progress_.last_,0);

	touch();
}

void TaskInfo::progress(unsigned long long val)
{
	timeval now;
	gettimeofday(&now,0);

	timeval diff = now - progress_.last_;

    progress_.rate_ = (val - progress_.val_) / 
		((double)diff.tv_sec + ((double)diff.tv_usec / 1000000.));

	diff = now - progress_.start_;

    progress_.speed_ = (val - progress_.min_) / 
		((double)diff.tv_sec + ((double)diff.tv_usec / 1000000.));

	progress_.val_ = val;

	gettimeofday(&progress_.last_,0);
	touch();
}

void TaskInfo::done()
{
	start(0,0);	
}

void TaskInfo::touch()
{ 
	checkAbort();

	check_ = last_ = ::time(0);
	busy_  = true;
	
	SignalHandler::checkInterrupt();

	if(stop_ && stoppable_)
	{
		stopped_ = true;
		stop_ = false;
		exit(1);
		throw Stop("ThreadControler stopped by request");
		::kill(getpid(),SIGTERM);
	}
}

void TaskInfo::checkAbort()
{
	if(abort_)
	{
		abort_   = false;
		throw Abort("ThreadControler aborted by request");
	}
}

void TaskInfo::parent(long p)
{
	parent_ = p;
	depth_  = 0;
	if(p != -1) depth_ = Monitor::info()[p].depth() + 1;
}

//-----------------------------------------------------------------------------

} // namespace eckit

