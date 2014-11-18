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
#include "eckit/log/Channel.h"
#include "eckit/log/ChannelBuffer.h"
#include "eckit/thread/ThreadSingleton.h"

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
    TYPE* operator() () { return new TYPE( new ChannelBuffer( std::cout ) ); }
};

template< typename TYPE >
struct ErrAlloc {
    ErrAlloc() {}
    TYPE* operator() () { return new TYPE( new ChannelBuffer( std::cerr ) ); }
};

Channel& StandardBehavior::infoChannel()
{
    static ThreadSingleton<Channel,OutAlloc<Channel> > x;
    return x.instance();
}

Channel& StandardBehavior::warnChannel()
{
    static ThreadSingleton<Channel,OutAlloc<Channel> > x;
    return x.instance();
}

Channel& StandardBehavior::errorChannel()
{
    static ThreadSingleton<Channel,ErrAlloc<Channel> > x;
    return x.instance();
}

Channel& StandardBehavior::debugChannel()
{
    static ThreadSingleton<Channel,OutAlloc<Channel> > x;
    return x.instance();
}

//-----------------------------------------------------------------------------

} // namespace eckit

