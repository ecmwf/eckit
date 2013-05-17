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

LogStream& StandardBehavior::infoStream()
{
    typedef NewAlloc1<InfoStream,Logger*> Alloc;
    static ThreadSingleton<InfoStream,Alloc> x( Alloc( new StdLogger( std::cout ) ) );
    return x.instance();
}

LogStream& StandardBehavior::warnStream()
{
    typedef NewAlloc1<WarnStream,Logger*> Alloc;
    static ThreadSingleton<WarnStream,Alloc> x( Alloc( new StdLogger( std::cerr ) ) );
    return x.instance();
}

LogStream& StandardBehavior::errorStream()
{
    typedef NewAlloc1<ErrorStream,Logger*> Alloc;
    static ThreadSingleton<ErrorStream,Alloc> x( Alloc( new StdLogger( std::cerr ) ) );
    return x.instance();
}

LogStream& StandardBehavior::debugStream()
{
    typedef NewAlloc1<DebugStream,Logger*> Alloc;
    static ThreadSingleton<DebugStream,Alloc> x( Alloc( new StdLogger( std::cout ) ) );
    return x.instance();
}

//-----------------------------------------------------------------------------

} // namespace eckit

