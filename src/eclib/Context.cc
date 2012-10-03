/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eclib/BackTrace.h"
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
	behavior_->setup(argc, argv);
    
    Log::init(); // should not logging before this line
}

int  Context::debug() const
{
    if( is_setup() ) 
        return behavior_->debug();
    else
        return 0;
}

void Context::debug( const int d )
{
    if( is_setup() ) 
        behavior_->debug(d);
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

void Context::checkInit() const
{
    if( !is_setup() )
    {
        std::cerr << "[ERROR] Context used before being created [ERROR] " << std::endl;
        std::cerr << "Backtrace: " << std::endl;
        std::cerr << BackTrace::dump() << std::endl;
        throw InitError( "Context used before being created" );
    }
}

string Context::home() const
{
    checkInit();
    return behavior_->home();
}

long Context::self() const
{
    checkInit();
    return behavior_->taskId();    
}

string Context::runName() const
{
    checkInit();
    return behavior_->runName();    
}

void Context::runName(const string &name)
{
    checkInit();
    behavior_->runName(name);    
}

string Context::displayName() const
{
    checkInit();
    return behavior_->displayName();    
}

void Context::displayName(const string &name)
{
    checkInit();
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
