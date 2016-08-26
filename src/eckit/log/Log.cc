/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/config/LibEcKit.h"
#include "eckit/log/Log.h"
#include "eckit/log/Channel.h"
#include "eckit/log/ChannelBuffer.h"
#include "eckit/log/MultiChannel.h"
#include "eckit/log/MonitorChannel.h"
#include "eckit/log/UserChannel.h"
#include "eckit/log/StatusTarget.h"
#include "eckit/log/MessageTarget.h"

#include "eckit/runtime/Context.h"

#include "eckit/exception/Exceptions.h"

#include "eckit/thread/ThreadSingleton.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

#ifndef _GNU_SOURCE

static void handle_strerror_r(std::ostream& s, int e, char es[], int hs )
{
    if ( hs == 0 )
        s << " (" << es << ") " ;
    else
        s << " (errno = " << e << ") ";
}

#else // GNU implementation is not XSI compliant and returns char* instead of int

static void handle_strerror_r(std::ostream& s, int e, char[], char* p )
{
    if (p) {
        s << " (" << p << ")";
    }
    else {
        s << " (errno = " << e << ") ";
    }
}

#endif

//----------------------------------------------------------------------------------------------------------------------

const Logger& Log::defaultLogger()
{
    return LibEcKit::instance();
}

void Log::registerChannel(const std::string& key, Channel* channel)
{
    Context::instance().registerChannel(key, channel);
}

void Log::removeChannel(const std::string& key)
{
    Context::instance().removeChannel(key);
}

Channel& Log::channel(const std::string& key)
{
    return Context::instance().channel(key);
}

Channel& Log::debug(const Logger& logger)
{
    return logger.debugChannel();
}

struct CreateStatusChannel {
    Channel* operator()() {
        Channel* channel = new Channel();
        channel->setLogTarget(new StatusTarget());
        return channel;
    }
};

std::ostream& Log::status()
{
    static ThreadSingleton<Channel, CreateStatusChannel> x;
    return x.instance();
}

struct CreateMessageChannel {
    Channel* operator()() {
        Channel* channel = new Channel();
        channel->setLogTarget(new MessageTarget());
        return channel;
    }
};

std::ostream& Log::message()
{
    static ThreadSingleton<Channel, CreateMessageChannel> x;
    return x.instance();
}

Channel& Log::info()
{
    return Context::instance().infoChannel();
}

Channel& Log::error()
{
    return Context::instance().errorChannel();
}

std::ostream& Log::panic()
{
    try {
        return Log::error();
    }
    catch (std::exception&) {
        return  std::cerr;
    }
}

Channel& Log::warning()
{
    return Context::instance().warnChannel();
}


UserChannel& Log::user()
{
    static ThreadSingleton<UserChannel> x;
    return x.instance();
}

std::ostream& Log::userInfo()
{
    UserChannel& u = user();
    u.msgType(UserChannel::INFO);
    return u;
}

std::ostream& Log::userError()
{
    UserChannel& u = user();
    u.msgType(UserChannel::ERROR);
    return u;
}

std::ostream& Log::userWarning()
{
    UserChannel& u = user();
    u.msgType(UserChannel::WARN);
    return u;
}

void Log::notifyClient(const std::string& msg)
{
    UserChannel& u = user();
    UserMsg* um = u.userMsg();
    if (um) um->notifyClient(msg);
}

//----------------------------------------------------------------------------------------------------------------------

std::ostream& Log::syserr(std::ostream& s)
{
    int e = errno;
    char estr [256];
    handle_strerror_r(s, e, estr, strerror_r( e, estr, sizeof(estr) ) );
    return s;
}

static int xindex = std::ios::xalloc();

int format(std::ostream& s)
{
    return s.iword(xindex);
}

std::ostream& setformat(std::ostream& s, int n)
{
    s.iword(xindex) = n;
    return s;
}

template class ThreadSingleton<UserChannel>;

//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit
