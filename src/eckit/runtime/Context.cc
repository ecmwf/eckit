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

static Once<Mutex> mutex;

Context::Context() :
    argc_(0),
    argv_(0),
    self_(0)
{
    behavior_.reset( new StandardBehavior() );
}

Context::~Context()
{
}

Context& Context::instance()
{
    AutoLock<Mutex> lock(mutex);
    static Context ctxt;
    return ctxt;
}

void Context::setup( int argc, char **argv )
{   
    if( argc <= 0 || argv == 0 ) 
        throw SeriousBug("Context setup with bad command line arguments");

    argc_ = argc;
    argv_ = argv;

    Log::init(); // should not logging before this line
}

void Context::behavior( ContextBehavior* b )
{
    AutoLock<Mutex> lock(mutex);
    if( !b )
        throw SeriousBug("Context setup with bad context behavior");
    behavior_.reset(b);
}

ContextBehavior& Context::behavior() const
{
    AutoLock<Mutex> lock(mutex);
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
    AutoLock<Mutex> lock(mutex);
    return argc_;
}

string Context::argv(int n) const
{ 
    AutoLock<Mutex> lock(mutex);
    ASSERT( argc_ != 0 && argv_ != 0 ); // check initialized
    ASSERT( n < argc_ && n >= 0);       // check bounds
    return argv_[n]; 
}

void Context::reconfigure()
{
	Log::info() << "Context::reconfigure" << endl;
    behavior_->reconfigure();
}

string Context::home() const
{
    return behavior_->home();
}

long Context::self() const
{
    return behavior_->taskId();    
}

string Context::runName() const
{
    return behavior_->runName();    
}

void Context::runName(const string &name)
{
    behavior_->runName(name);    
}

string Context::displayName() const
{
    return behavior_->displayName();    
}

void Context::displayName(const string &name)
{
    behavior_->displayName(name);    
}

Logger* Context::createInfoLogger()
{
    return behavior_->createInfoLogger();       
}


Logger* Context::createDebugLogger()
{
    return behavior_->createDebugLogger();       
}

Logger* Context::createWarningLogger()
{
    return behavior_->createWarningLogger();       
}

Logger* Context::createErrorLogger()
{
    return behavior_->createErrorLogger();       
}

//-----------------------------------------------------------------------------

} // namespace eckit

