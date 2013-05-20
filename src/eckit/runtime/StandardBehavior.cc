/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <stdlib.h>

#include "eckit/runtime/StandardBehavior.h"
#include "eckit/log/LogBuffer.h"
#include "eckit/log/StdLogger.h"
#include "eckit/thread/ThreadSingleton.h"

using namespace std;

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

StandardBehavior::StandardBehavior()
{
}

StandardBehavior::~StandardBehavior()
{
}

//-----------------------------------------------------------------------------

template< typename TYPE >
struct OutAlloc {
    OutAlloc() {}
    TYPE* operator() () { return new TYPE( new StdLogger( std::cout ) ); }
};

template< typename TYPE >
struct ErrAlloc {
    ErrAlloc() {}
    TYPE* operator() () { return new TYPE( new StdLogger( std::cerr ) ); }
};

LogStream& StandardBehavior::infoStream()
{
    static ThreadSingleton<InfoStream,OutAlloc<InfoStream> > x;
    return x.instance();
}

LogStream& StandardBehavior::warnStream()
{
    static ThreadSingleton<WarnStream,ErrAlloc<WarnStream> > x;
    return x.instance();
}

LogStream& StandardBehavior::errorStream()
{
    static ThreadSingleton<ErrorStream,ErrAlloc<ErrorStream> > x;
    return x.instance();
}

LogStream& StandardBehavior::debugStream()
{
    static ThreadSingleton<DebugStream,OutAlloc<DebugStream> > x;
    return x.instance();
}

//-----------------------------------------------------------------------------

} // namespace eckit

