/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/log/Log.h"
#include "eckit/os/BackTrace.h"
#include "eckit/runtime/Context.h"
#include "eckit/runtime/ContextBehavior.h"
#include "eckit/runtime/StandardBehavior.h"
#include "eckit/runtime/Monitor.h"
#include "eckit/thread/Mutex.h"
#include "eckit/thread/Once.h"
#include "eckit/thread/ThreadSingleton.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

static Once<Mutex> local_mutex;

Context::Context() :
    argc_(0),
    argv_(0),
    taskID_(0),    
    home_("/"),
    runName_("undef"),
    displayName_()
{
    char * h = getenv( "HOME" );
    if(h)
        home_ = h;

    behavior_.reset( new StandardBehavior() );
}

Context::~Context()
{
}

Context& Context::instance()
{
    AutoLock<Mutex> lock(local_mutex);
    static Context ctxt;
    return ctxt;
}

void Context::setup( int argc, char **argv )
{   
    if( argc <= 0 || argv == 0 ) 
        throw SeriousBug("Context setup with bad command line arguments");

    argc_ = argc;
    argv_ = argv;
}

void Context::behavior( ContextBehavior* b )
{
    AutoLock<Mutex> lock(local_mutex);
    if( !b )
        throw SeriousBug("Context setup with bad context behavior");
    behavior_.reset(b);
}

ContextBehavior& Context::behavior() const
{
    AutoLock<Mutex> lock(local_mutex);
    return *behavior_.get();
}

int  Context::debug() const
{
   return behavior_->debug();
}

void Context::debug( const int d )
{
    behavior_->debug(d);
}

int  Context::argc() const 
{ 
    AutoLock<Mutex> lock(local_mutex);
    return argc_;
}

string Context::argv(int n) const
{ 
    AutoLock<Mutex> lock(local_mutex);
    ASSERT( argc_ != 0 && argv_ != 0 ); // check initialized
    ASSERT( n < argc_ && n >= 0);       // check bounds
    return argv_[n]; 
}

char** Context::argvs() const
{
    return argv_;
}

void Context::reconfigure()
{
	Log::info() << "Context::reconfigure" << std::endl;
    behavior_->reconfigure();
}

string Context::home() const
{
    return home_;
}

void Context::home(const string &h)
{
    AutoLock<Mutex> lock(local_mutex);
    home_ = h;
}

long Context::self() const
{
    return taskID_;
}

void Context::self(long id)
{
    taskID_ = id;
}

string Context::runName() const
{
    return runName_;
}

void Context::runName(const string &name)
{
    runName_ = name;
}

string Context::displayName() const
{
    if(displayName_.empty())
        return runName();
    else
        return displayName_;
}

void Context::displayName(const string &name)
{
    displayName_ = name;
}

Channel& Context::infoChannel()
{
    return behavior_->infoChannel();
}

Channel& Context::warnChannel()
{
    return behavior_->warnChannel();
}

Channel& Context::errorChannel()
{
    return behavior_->errorChannel();
}

Channel& Context::debugChannel()
{
    return behavior_->debugChannel();
}

//-----------------------------------------------------------------------------

} // namespace eckit

