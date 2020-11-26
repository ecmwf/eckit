/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <unistd.h>
#include <cstring>

#include "eckit/config/LibEcKit.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/log/Channel.h"
#include "eckit/log/FileTarget.h"
#include "eckit/log/Log.h"
#include "eckit/log/MessageTarget.h"
#include "eckit/log/OStreamTarget.h"
#include "eckit/log/PrefixTarget.h"
#include "eckit/log/StatusTarget.h"
#include "eckit/log/UserChannel.h"
#include "eckit/runtime/Main.h"
#include "eckit/system/Library.h"
#include "eckit/system/LibraryManager.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/ThreadSingleton.h"
#include "eckit/utils/Translator.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

#if defined(_GNU_SOURCE)

/* To use with GNU libc strerror_r */

static void handle_strerror_r(std::ostream& s, int e, char[], char* p) {
    if (p) {
        s << " (" << p << ")";
    }
    else {
        s << " (errno = " << e << ") ";
    }
}

#elif (_POSIX_C_SOURCE >= 200112L || _XOPEN_SOURCE >= 600 || _POSIX_VERSION >= 200112L || _XOPEN_VERSION >= 600)

/* To use with XSI-compliant strerror_r
 *
 * Linux defines _POSIX_C_SOURCE and _XOPEN_SOURCE
 * BSD defines _POSIX_VERSION and _XOPEN_VERSION
 * glibc defines _GNU_SOURCE and implements a non-XSI compliant strerror_r
 */

static void handle_strerror_r(std::ostream& s, int e, char es[], int hs) {
    if (hs == 0) {
        s << " (" << es << ") ";
    }
    else {
        s << " (errno = " << e << ") ";
    }
}

#else /* we don't know what to do */

#warning "eckit doesn't recognise stderror_r since this isn't a GNU libc or a supported UNIX"

/* This uses the deprecated sys_errlist[] error arrays */

static void handle_strerror_r(std::ostream& s, int e, ...) {
    if (e < sys_nerr)
        s << " (" << sys_errlist[e] << ") ";
    else
        s << " (errno = " << e << ") ";
}

#endif

//----------------------------------------------------------------------------------------------------------------------

struct CreateStatusChannel {
    Channel* operator()() { return new Channel(new StatusTarget()); }
};

std::ostream& Log::status() {
    static ThreadSingleton<Channel, CreateStatusChannel> x;
    return x.instance();
}

struct CreateMessageChannel {
    Channel* operator()() { return new Channel(new MessageTarget()); }
};

std::ostream& Log::message() {
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

struct CreateMetricsChannel : public CreateLogChannel {
    virtual Channel* createChannel() { return new Channel(Main::instance().createMetricsLogTarget()); }
};

Channel& Log::metrics() {
    if (!Main::ready()) {
        static Channel empty(new PrefixTarget("PRE-MAIN-METRICS", new OStreamTarget(std::cout)));
        return empty;
    }
    static ThreadSingleton<Channel, CreateMetricsChannel> x;
    return x.instance();
}


struct CreateInfoChannel : public CreateLogChannel {
    virtual Channel* createChannel() { return new Channel(Main::instance().createInfoLogTarget()); }
};

Channel& Log::info() {
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

Channel& Log::error() {
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

Channel& Log::warning() {
    if (!Main::ready()) {
        static Channel empty(new PrefixTarget("PRE-MAIN-WARNING", new OStreamTarget(std::cout)));
        return empty;
    }
    static ThreadSingleton<Channel, CreateWarningChannel> x;
    return x.instance();
}

struct CreateDebugChannel : public CreateLogChannel {
    virtual Channel* createChannel() { return new Channel(Main::instance().createDebugLogTarget()); }
};

Channel& Log::debug() {
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


std::ostream& Log::panic() {
    try {
        return Log::error();
    }
    catch (std::exception&) {
        return std::cerr;
    }
}


UserChannel& Log::user() {
    static ThreadSingleton<UserChannel> x;
    return x.instance();
}

std::ostream& Log::userInfo() {
    UserChannel& u = user();
    u.msgType(UserChannel::INFO);
    return u;
}

std::ostream& Log::userError() {
    UserChannel& u = user();
    u.msgType(UserChannel::ERROR);
    return u;
}

std::ostream& Log::userWarning() {
    UserChannel& u = user();
    u.msgType(UserChannel::WARN);
    return u;
}

void Log::notifyClient(const std::string& msg) {
    UserChannel& u = user();
    UserMsg* um    = u.userMsg();
    if (um)
        um->notifyClient(msg);
}

//----------------------------------------------------------------------------------------------------------------------

void Log::setStream(std::ostream& out) {
    info().setStream(out);
    warning().setStream(out);
    error().setStream(out);
    if (debug()) {
        debug().setStream(out);
    }
    std::vector<std::string> libs = eckit::system::LibraryManager::list();
    for (std::vector<std::string>::iterator libname = libs.begin(); libname != libs.end(); ++libname) {
        system::Library::lookup(*libname).debugChannel().setStream(out);
    }
}
void Log::addStream(std::ostream& out) {
    info().addStream(out);
    warning().addStream(out);
    error().addStream(out);
    if (debug()) {
        debug().addStream(out);
    }
    std::vector<std::string> libs = eckit::system::LibraryManager::list();
    for (std::vector<std::string>::iterator libname = libs.begin(); libname != libs.end(); ++libname) {
        system::Library::lookup(*libname).debugChannel().addStream(out);
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
    std::vector<std::string> libs = eckit::system::LibraryManager::list();
    for (std::vector<std::string>::iterator libname = libs.begin(); libname != libs.end(); ++libname) {
        system::Library::lookup(*libname).debugChannel().setTarget(file);
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
    std::vector<std::string> libs = eckit::system::LibraryManager::list();
    for (std::vector<std::string>::iterator libname = libs.begin(); libname != libs.end(); ++libname) {
        system::Library::lookup(*libname).debugChannel().addTarget(file);
    }
}

void Log::setCallback(channel_callback_t cb, void* data) {
    info().setCallback(cb, data);
    warning().setCallback(cb, data);
    error().setCallback(cb, data);
    if (debug()) {
        debug().setCallback(cb, data);
    }

    std::vector<std::string> libs = eckit::system::LibraryManager::list();
    for (std::vector<std::string>::iterator libname = libs.begin(); libname != libs.end(); ++libname) {
        if (system::Library::lookup(*libname).debugChannel()) {
            system::Library::lookup(*libname).debugChannel().setCallback(cb, data);
        }
    }
}

void Log::addCallback(channel_callback_t cb, void* data) {
    info().addCallback(cb, data);
    warning().addCallback(cb, data);
    error().addCallback(cb, data);
    if (debug()) {
        debug().addCallback(cb, data);
    }
    std::vector<std::string> libs = eckit::system::LibraryManager::list();
    for (std::vector<std::string>::iterator libname = libs.begin(); libname != libs.end(); ++libname) {
        system::Library::lookup(*libname).debugChannel().addCallback(cb, data);
    }
}

void Log::flush() {
    info().flush();
    warning().flush();
    error().flush();
    debug().flush();
    std::vector<std::string> libs = eckit::system::LibraryManager::list();
    for (std::vector<std::string>::iterator libname = libs.begin(); libname != libs.end(); ++libname) {
        system::Library::lookup(*libname).debugChannel().flush();
    }
}

void Log::reset() {
    info().reset();
    warning().reset();
    error().reset();
    debug().reset();
    std::vector<std::string> libs = eckit::system::LibraryManager::list();
    for (std::vector<std::string>::iterator libname = libs.begin(); libname != libs.end(); ++libname) {
        system::Library::lookup(*libname).debugChannel().reset();
    }
}

void Log::print(std::ostream& os) {
    os << "Log::info() " << info() << std::endl;
    os << "Log::warning() " << warning() << std::endl;
    os << "Log::error() " << error() << std::endl;
    os << "Log::debug() " << debug() << std::endl;
    std::vector<std::string> libs = eckit::system::LibraryManager::list();
    for (std::vector<std::string>::iterator libname = libs.begin(); libname != libs.end(); ++libname) {
        os << *libname << ".debug() " << system::Library::lookup(*libname).debugChannel() << std::endl;
    }
}

//----------------------------------------------------------------------------------------------------------------------

std::ostream& Log::syserr(std::ostream& s) {
    int e = errno;
    char estr[256];
    handle_strerror_r(s, e, estr, strerror_r(e, estr, sizeof(estr)));
    return s;
}

template class ThreadSingleton<UserChannel>;

//----------------------------------------------------------------------------------------------------------------------

static int xindex = std::ios::xalloc();

int format(std::ostream& s) {
    return s.iword(xindex);
}

void format(std::ostream& s, int f) {
    s.iword(xindex) = f;
}

std::ostream& setformat(std::ostream& s, int f) {
    format(s, f);
    return s;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
