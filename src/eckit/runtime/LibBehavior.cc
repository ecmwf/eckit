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

template< typename TYPE, int L >
struct CallBackAlloc {
    CallBackAlloc( CallbackLogger::callback c, void* ctxt ) : c_(c),ctxt_(ctxt) {}
    TYPE* operator() () 
    { 
        CallbackLogger* lg = new CallbackLogger();
        lg->register_callback( c_, L, ctxt_ );
        return new TYPE( lg ); 
    }
    CallbackLogger::callback c_;
    void* ctxt_;
};

//-----------------------------------------------------------------------------

LibBehavior::LibBehavior() : c_(0),ctxt_(0)
{
}

LibBehavior::~LibBehavior()
{
}

void LibBehavior::register_logger_callback( CallbackLogger::callback c, void* ctxt )
{    
    ASSERT(c);
    c_ = c;
    ctxt_ = ctxt;
}

LogStream& LibBehavior::infoStream()
{
    if(!c_) throw SeriousBug("Forgot to register callback before logging?");
    typedef CallBackAlloc<InfoStream,(int)'I'> Alloc;
    static Alloc a (c_,ctxt_);
    static ThreadSingleton<InfoStream,Alloc> x( a );
    return x.instance();
}

LogStream& LibBehavior::warnStream()
{
    if(!c_) throw SeriousBug("Forgot to register callback before logging?");
    typedef CallBackAlloc<WarnStream,(int)'W'> Alloc;
    static Alloc a (c_,ctxt_);
    static ThreadSingleton<WarnStream,Alloc> x( a );
    return x.instance();
}

LogStream& LibBehavior::errorStream()
{
    if(!c_) throw SeriousBug("Forgot to register callback before logging?");
    typedef CallBackAlloc<ErrorStream,(int)'E'> Alloc;
    static Alloc a (c_,ctxt_);
    static ThreadSingleton<ErrorStream,Alloc> x( a );
    return x.instance();
}

LogStream& LibBehavior::debugStream()
{
    if(!c_) throw SeriousBug("Forgot to register callback before logging?");
    typedef CallBackAlloc<DebugStream,(int)'D'> Alloc;
    static Alloc a (c_,ctxt_);
    static ThreadSingleton<DebugStream,Alloc> x( a );
    return x.instance();
}

//-----------------------------------------------------------------------------

} // namespace eckit

