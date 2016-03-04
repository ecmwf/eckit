/*
 * (C) Copyright 1996-2016 ECMWF.
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

namespace {
    static eckit::Mutex* local_mutex = 0;
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    typedef std::map<std::string,Channel*> LogMap;
    static LogMap* logMap = 0;

    static void init() {
        local_mutex = new eckit::Mutex();
        logMap = new LogMap();
    }
}

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

void StandardBehavior::registerChannel(const std::string& key, Channel* channel)
{
    pthread_once(&once, init);
    eckit::AutoLock<eckit::Mutex> lock(local_mutex);

    if(logMap->find(key) == logMap->end())
    {
        logMap->insert(std::make_pair<std::string,Channel*>(key, channel));
    } else {
        throw BadParameter( "Channel '" + key + "' is already registered ", Here());
    }
}

void StandardBehavior::removeChannel(const std::string& key)
{
    pthread_once(&once, init);
    eckit::AutoLock<eckit::Mutex> lock(local_mutex);

    if(logMap->find(key) == logMap->end())
    {
        //channel is not registered
        throw BadParameter( "Channel '" + key + "' does not exist ", Here());
    } else {
        logMap->erase (key);
    }
}

Channel& StandardBehavior::channel(const std::string& key)
{
    pthread_once(&once, init);
    eckit::AutoLock<eckit::Mutex> lock(local_mutex);

    if       ( key == "Error" )  { return errorChannel(); }
    else if  ( key == "Warn"  )  { return warnChannel(); }
    else if  ( key == "Info"  )  { return infoChannel(); }
    else if  ( key == "Debug" )  { return debugChannel(); }

    if(logMap->find(key) == logMap->end()) {
        //requested channel not found
        throw BadParameter( "Channel '" + key + "' not found ", Here());
    } else {
        return  *((*logMap)[key]);
    }
}

//-----------------------------------------------------------------------------

} // namespace eckit

