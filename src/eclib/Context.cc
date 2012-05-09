/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eclib/Context.h"
#include "eclib/ContextBehavior.h"
#include "eclib/Log.h"
#include "eclib/Monitor.h"

Context::Context() :
    behavior_(0),
    argc_(0),
    argv_(0),
    self_(0)
{
}

Context::~Context()
{
}

Context& Context::instance()
{
    static Context theContext;
    return theContext;
}

void Context::setup( int argc, char **argv, ContextBehavior* b )
{   
    if( argc <= 0 || argv == 0 ) 
        throw SeriousBug("Context setup with bad command line arguments");
    if( !b )
        throw SeriousBug("Context setup with bad context behavior");

    argc_ = argc;
    argv_ = argv;

    if( behavior_.get() )
		throw SeriousBug("Context is already setup with a behavior");
    
    behavior_ = auto_ptr<ContextBehavior>( b );
    
    Log::init(); // should not logging before this line
}

int  Context::debug() const
{
    if( is_setup() ) 
        return behavior_->debug();
    else
        return 0;
}

int  Context::argc() const 
{ 
    return argc_; 
}

string Context::argv(int n) const
{ 
    ASSERT( argc_ != 0 && argv_ != 0 ); // check initialized 
    ASSERT( n < argc_ && n >= 0);       // check bounds
    return argv_[n]; 
}

void Context::reconfigure()
{
	Log::info() << "Context::reconfigure" << endl;
    
    // forward to ContextBehavior
    if( is_setup() ) 
        return behavior_->reconfigure();    
}

string Context::home() const
{
    ASSERT( is_setup() );
    return behavior_->home();
}

long Context::self() const
{
    ASSERT( is_setup() );
    return behavior_->taskId();    
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
