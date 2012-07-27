/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <cstdlib>
#include <cassert>

#include "eclib/LibBehavior.h"

using namespace std;

LibBehavior::LibBehavior() :
    info_ (0),
    debug_(0),
    warn_ (0),
    error_(0)
{
}

LibBehavior::~LibBehavior()
{
}

void LibBehavior::register_logger_callback( CallbackLogger::callback* c, void* ctxt )
{
    assert(info_);
    assert(debug_);
    assert(warn_);
    assert(error_);
    
    info_->register_callback (c, (int)'I', ctxt);
    debug_->register_callback(c, (int)'D', ctxt);
    warn_->register_callback (c, (int)'W', ctxt);
    error_->register_callback(c, (int)'E', ctxt);
}


string LibBehavior::home() const
{
//    "/usr/local/etc/" + Context::instance().appName()
    return string( getenv( "HOME" ) );
}

Logger* LibBehavior::createInfoLogger()
{    
    info_ = new CallbackLogger();
    return info_;
}

Logger* LibBehavior::createDebugLogger()
{    
    debug_ = new CallbackLogger();
    return debug_;
}

Logger* LibBehavior::createWarningLogger()
{   
    warn_ = new CallbackLogger();
    return warn_;
}

Logger* LibBehavior::createErrorLogger()
{    
    error_ = new CallbackLogger();
    return error_;
}



