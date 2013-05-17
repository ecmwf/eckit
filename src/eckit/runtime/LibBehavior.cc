/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <cstdlib>
#include <cassert>

#include "eckit/log/LogBuffer.h"
#include "eckit/runtime/LibBehavior.h"
#include "eckit/thread/ThreadSingleton.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

LibBehavior::LibBehavior() :
    info_ ( new CallbackLogger() ),
    debug_( new CallbackLogger() ),
    warn_ ( new CallbackLogger() ),
    error_( new CallbackLogger() )
{
}

LibBehavior::~LibBehavior()
{
    delete info_;
    delete debug_;
    delete warn_;
    delete error_;
}

void LibBehavior::register_logger_callback( CallbackLogger::callback c, void* ctxt )
{    
    info_->register_callback (c, (int)'I', ctxt);
    debug_->register_callback(c, (int)'D', ctxt);
    warn_->register_callback (c, (int)'W', ctxt);
    error_->register_callback(c, (int)'E', ctxt);
}

LogStream& LibBehavior::infoStream()
{
    typedef NewAlloc1<InfoStream,Logger*> Alloc;
    static Alloc a (info_);
    static ThreadSingleton<InfoStream,Alloc> x( a );
    return x.instance();
}

LogStream& LibBehavior::warnStream()
{
    typedef NewAlloc1<WarnStream,Logger*> Alloc;
    static Alloc a (warn_);
    static ThreadSingleton<WarnStream,Alloc> x( a );
    return x.instance();
}

LogStream& LibBehavior::errorStream()
{
    typedef NewAlloc1<ErrorStream,Logger*> Alloc;
    static Alloc a (error_);
    static ThreadSingleton<ErrorStream,Alloc> x( a );
    return x.instance();
}

LogStream& LibBehavior::debugStream()
{
    typedef NewAlloc1<DebugStream,Logger*> Alloc;
    static Alloc a (debug_);
    static ThreadSingleton<DebugStream,Alloc> x( a );
    return x.instance();
}

//-----------------------------------------------------------------------------

} // namespace eckit

