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

#include "eckit/thread/AutoLock.h"

#include "eckit/log/Channel.h"
#include "eckit/log/UserChannel.h"
#include "eckit/log/StatusTarget.h"
#include "eckit/log/MessageTarget.h"
#include "eckit/log/OStreamTarget.h"
#include "eckit/log/PrefixTarget.h"
#include "eckit/utils/Translator.h"
#include "eckit/log/FileTarget.h"

#include "eckit/runtime/Main.h"

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

#if 0 // if we bring this back, then we need to implemenmt the mutex with direct pthread_mutex_t

typedef std::map<std::string, Channel*> ChannelRegister;
static Once<ChannelRegister> channelsRegister; // needs Once<> to have a lock/unlock semantic

void Log::registerChannel(const std::string& key, Channel* channel) {
    pthread_once(&once, init);
    AutoLock<Mutex> lock(channelsRegister);

    ASSERT(channel);
    ChannelRegister::iterator itr = channelsRegister->find(key);
    if (itr == channelsRegister->end()) {
        channelsRegister->insert(std::make_pair(key, channel));
    }
    else {
        std::ostringstream msg;
        msg << "Channel with name " << key << " already registered";
        throw SeriousBug(msg.str());
    }
}

void Log::removeChannel(const std::string& key) {
    pthread_once(&once, init);
    AutoLock<Mutex> lock(*local_mutex);

    ChannelRegister::iterator itr = channelsRegister->find(key);
    if (itr != channelsRegister->end()) {
        delete itr->second;
        channelsRegister->erase(itr);
    }
}

Channel& Log::channel(const std::string& key) {
    pthread_once(&once, init);
    AutoLock<Mutex> lock(*local_mutex);

    ChannelRegister::iterator itr = channelsRegister->find(key);
    ASSERT(itr != channelsRegister->end());
    return *(itr->second);
}

#endif

struct CreateStatusChannel {
    Channel* operator()() {
        return new Channel(new StatusTarget());
    }
};

std::ostream& Log::status()
{
    static ThreadSingleton<Channel, CreateStatusChannel> x;
    return x.instance();
}

struct CreateMessageChannel {
    Channel* operator()() {
        return new Channel(new MessageTarget());
    }
};

std::ostream& Log::message()
{
    static ThreadSingleton<Channel, CreateMessageChannel> x;
    return x.instance();
}



struct CreateLogChannel {

    virtual Channel* createChannel() = 0;

    Channel* operator()() {
        try {
            return createChannel();
        }
        catch (std::exception& e) {
            std::cerr << "Exception caught when creating channel: " << e.what() << std::endl;
            return new Channel(new OStreamTarget(std::cout));
        }
    }
};

struct CreateInfoChannel : public CreateLogChannel {
    virtual Channel* createChannel() { return new Channel(Main::instance().createInfoLogTarget()); }
};

Channel& Log::info()
{
    if (!Main::ready()) {
        static Channel empty(new PrefixTarget("PRE-MAIN-INFO", new OStreamTarget(std::cout)));
        return empty;
    }
    static ThreadSingleton<Channel, CreateInfoChannel> x;
    return x.instance();
}

struct CreateErrorChannel : public CreateLogChannel {
    virtual Channel* createChannel() { return new Channel(Main::instance().createErrorLogTarget()); }
};

Channel& Log::error()
{
    if (!Main::ready()) {
        static Channel empty(new PrefixTarget("PRE-MAIN-ERROR", new OStreamTarget(std::cout)));
        return empty;
    }
    static ThreadSingleton<Channel, CreateErrorChannel> x;
    return x.instance();
}

struct CreateWarningChannel : public CreateLogChannel {
    virtual Channel* createChannel() { return new Channel(Main::instance().createWarningLogTarget()); }
};

Channel& Log::warning()
{
    if (!Main::ready()) {
        static Channel empty(new PrefixTarget("PRE-MAIN-WARNING", new OStreamTarget(std::cout)));
        return empty;
    }
    static ThreadSingleton<Channel, CreateWarningChannel> x;
    return x.instance();
}

struct CreateDebugChannel : public CreateLogChannel {
    virtual Channel* createChannel() {
        return new Channel(Main::instance().createDebugLogTarget());
    }
};

Channel& Log::debug()
{
    if (!Main::ready()) {


        const char* e = getenv("DEBUG");

        if (e && bool(Translator<std::string, bool>()(e))) {
            static Channel empty(new PrefixTarget("PRE-MAIN-DEBUG", new OStreamTarget(std::cout)));
            return empty;
        }
        else {
            static Channel empty;
            return empty;
        }
    }

    if (!Main::instance().debug_) {
        static Channel empty;
        return empty;
    }


    static ThreadSingleton<Channel, CreateDebugChannel> x;
    return x.instance();
}


std::ostream& Log::panic()
{
    try {
        return Log::error();
    }
    catch (std::exception&) {
        return std::cerr;
    }
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

void Log::setStream(std::ostream& out) {
    info().setStream(out);
    warning().setStream(out);
    error().setStream(out);
    if (debug()) {
        debug().setStream(out);
    }
}
void Log::addStream(std::ostream& out) {
    info().addStream(out);
    warning().addStream(out);
    error().addStream(out);
    if (debug()) {
        debug().addStream(out);
    }
}

void Log::setFile(const std::string& path) {
    LogTarget* file = new FileTarget(path);

    info().setTarget(file);
    warning().setTarget(file);
    error().setTarget(file);
    if (debug()) {
        debug().setTarget(file);
    }
}
void Log::addFile(const std::string& path) {
    LogTarget* file = new FileTarget(path);

    info().addTarget(file);
    warning().addTarget(file);
    error().addTarget(file);
    if (debug()) {
        debug().addTarget(file);
    }
}

void Log::setCallback(channel_callback_t cb, void* data) {
    info().setCallback(cb, data);
    warning().setCallback(cb, data);
    error().setCallback(cb, data);
    if (debug()) {
        debug().setCallback(cb, data);
    }
}

void Log::addCallback(channel_callback_t cb, void* data) {
    info().addCallback(cb, data);
    warning().addCallback(cb, data);
    error().addCallback(cb, data);
    if (debug()) {
        debug().addCallback(cb, data);
    }
}

void Log::reset() {
    info().reset();
    warning().reset();
    error().reset();
    debug().reset();
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
