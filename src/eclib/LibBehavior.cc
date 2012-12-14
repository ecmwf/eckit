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
    info_ ( new CallbackLogger() ),
    debug_( new CallbackLogger() ),
    warn_ ( new CallbackLogger() ),
    error_( new CallbackLogger() )
{
    home_ = getenv( "HOME" );
}

LibBehavior::~LibBehavior()
{
    // we will leak the callback loggers since 
    // they should only be destroyed on program exit since we might still output
    // within some destructor
}

void LibBehavior::register_logger_callback( CallbackLogger::callback c, void* ctxt )
{    
    info_->register_callback (c, (int)'I', ctxt);
    debug_->register_callback(c, (int)'D', ctxt);
    warn_->register_callback (c, (int)'W', ctxt);
    error_->register_callback(c, (int)'E', ctxt);
}

Logger* LibBehavior::createInfoLogger()
{    
    return info_;
}

Logger* LibBehavior::createDebugLogger()
{    
    return debug_;
}

Logger* LibBehavior::createWarningLogger()
{   
    return warn_;
}

Logger* LibBehavior::createErrorLogger()
{    
    return error_;
}



