/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date   August 2016

#include <algorithm>
#include <cctype>
#include <map>

#include "eckit/system/Library.h"

#include "eckit/config/Resource.h"
#include "eckit/config/YAMLConfiguration.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/LocalPathName.h"
#include "eckit/log/Log.h"
#include "eckit/log/OStreamTarget.h"
#include "eckit/log/PrefixTarget.h"
#include "eckit/os/System.h"
#include "eckit/system/LibraryManager.h"
#include "eckit/system/SystemInfo.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Mutex.h"
#include "eckit/thread/ThreadSingleton.h"
#include "eckit/utils/Translator.h"

namespace eckit::system {

//----------------------------------------------------------------------------------------------------------------------

Library::Library(const std::string& name) : name_(name), prefix_(name), debug_(false), debugChannelCtl_{std::make_shared<DebugChannelControl>()} {

    LibraryManager::enregister(name, this);

    std::transform(prefix_.begin(), prefix_.end(), prefix_.begin(),
                   [](char c) { return (c == '-') ? '_' : ::toupper(c); });

    std::string s = prefix_ + "_DEBUG";
    const char* e = ::getenv(s.c_str());
    if (e) {
        debug_ = eckit::Translator<std::string, bool>()(e);
    }

    if (!debug_) {
        e = ::getenv("DEBUG");
        if (e) {
            debug_ = eckit::Translator<std::string, bool>()(e);
        }
    }
}

Library::~Library() {}

const std::string& Library::name() const {
    return name_;
}

std::string Library::prefixDirectory() const {
    AutoLock<Mutex> lock(mutex_);

    if (prefixDirectory_.empty()) {
        prefixDirectory_ = LocalPathName(libraryPath()).dirName().dirName().realName();
    }

    return prefixDirectory_;
}

std::string Library::home() const {
    AutoLock<Mutex> lock(mutex_);

    std::string libhome = prefix_ + "_HOME";
    char* home          = ::getenv(libhome.c_str());
    if (home) {
        return home;
    }

    return home_;  // may return empty string (meaning not set)
}

std::string Library::libraryHome() const {
    std::string h = home();
    if (!h.empty()) {
        return h;
    }
    return prefixDirectory();
}

void Library::libraryHome(const std::string& home) {
    AutoLock<Mutex> lock(mutex_);
    home_ = home;
}

std::string Library::libraryPath() const {
    AutoLock<Mutex> lock(mutex_);

    if (libraryPath_.empty()) {
        std::string p = eckit::System::addrToPath(addr());
        libraryPath_  = LocalPathName(p).realName();
    }
    return libraryPath_;
}


Library::ThreadSentinel::ThreadSentinel(std::thread::id id) : id_{id} {}

Library::ThreadSentinel::~ThreadSentinel() {
    for (auto& ctl : libCtl_) {
        AutoLock<Mutex> lock(ctl->mutex);
        ctl->deadThreads.push_back(id_);
    }
}

Channel& Library::debugChannel() const {
    // THIS IS DIRTY
    //
    // Usually we would like to use thread local channels directly and perform a lookup by library
    // However, some downstream libraries are full of other static obejcts that write to debug from their destructors
    // due to poor destruction ordering SEGFAULTS are occuring.
    // Hence we lookup locks for each thread and safe the thread id of dying threads.
    // Then at a later point the map is cleaned up
    // A shared pointer is required to prevent that ~ThreadSentinel access potentially destructed
    // library members
    //
    // In principal we have the same problem with all the other logs, but with the library specific debug channel we experience it
    // very often in ctests on the CI
    //
    // Improvements: Use library specific thread locals by deriving from a specialized header only `LibrarySpecification<>` class that provides
    // the statics and implements `Library` with overriden debugChannel for compatibility
    // Requires downstream changes. Will not solve dependency problem
    thread_local ThreadSentinel threadSentinel{};
    thread_local std::thread::id tid{std::this_thread::get_id()};
    
    {
        // TODO need a shared_lock (does not build on MAC currently), maybe build RW lock with atomics
        AutoLock<Mutex> lock(debugChannelCtl_->mutex);

        auto it = debugChannelCtl_->channels.find(tid);
        if (it != debugChannelCtl_->channels.end()) {
            return *it->second;
        }
    }
    {
        AutoLock<Mutex> lock(debugChannelCtl_->mutex);
        threadSentinel.libCtl_.push_back(debugChannelCtl_);
        
        // Frequent clean ups
        for (const auto& deadTid: debugChannelCtl_->deadThreads) {
            debugChannelCtl_->channels.erase(deadTid);
        }
        debugChannelCtl_->deadThreads.clear();
        
        return *debugChannelCtl_->channels
                    .emplace(tid, debug_ ? std::make_unique<Channel>(new PrefixTarget(prefix_ + "_DEBUG"))  //
                                         : std::make_unique<Channel>())                                     //
                    .first->second.get();
    }
}


const Configuration& Library::configuration() const {
    AutoLock<Mutex> lock(mutex_);

    if (configuration_) {
        return *configuration_;
    }

    std::string s = "$" + prefix_ + "_CONFIG_PATH";
    std::string p = "~" + name_ + "/etc/" + name_ + "/config.yaml";

    eckit::PathName cfgpath = eckit::Resource<eckit::PathName>(s.c_str(), p.c_str());

    Log::debug() << "Parsing Lib " << name_ << " config file " << cfgpath << std::endl;

    eckit::Configuration* cfg
        = cfgpath.exists() ? new eckit::YAMLConfiguration(cfgpath) : new eckit::YAMLConfiguration(std::string(""));

    Log::debug() << "Lib " << name_ << " configuration: " << *cfg << std::endl;

    configuration_.reset(cfg);

    return *configuration_;
}

std::string Library::expandPath(const std::string& p) const {
    std::string s = "~" + name_;

    ASSERT(p.substr(0, s.size()) == s);
    ASSERT(p.size() == s.size() || p[s.size()] == '/');

    // 1. if HOME is set for this library, either via env variable LIBNAME_HOME exists
    //    or set in code expand ~lib/ to its content

    const std::string h = home();
    if (!h.empty()) {
        std::string result = h + "/" + p.substr(s.size());
        return result;
    }

    // 2. try to walk up the path and check for paths that exist

    const std::string extra = "/" + p.substr(s.size());

    eckit::LocalPathName path = prefixDirectory();
    eckit::LocalPathName root("/");

    while (true) {
        LocalPathName tmp = path + extra;

        if (tmp.exists()) {
            return tmp;
        }

        if (path == root) {
            break;
        }

        path = path.dirName();
    }

    // 3. as a last resort expand with prefix directory although we know the path doesn't exist

    return prefixDirectory() + extra;
}

void Library::print(std::ostream& out) const {
    out << "Library("
        << "name=" << name_ << ", path=" << libraryPath() << ", prefix=" << prefixDirectory() << ")";
}

//----------------------------------------------------------------------------------------------------------------------

std::vector<std::string> Library::list() {
    return LibraryManager::list();
}

void Library::list(std::ostream& out) {
    return LibraryManager::list(out);
}

bool Library::exists(const std::string& name) {
    return LibraryManager::exists(name);
}

const Library& Library::lookup(const std::string& name) {
    return LibraryManager::lookup(name);
}

const void* Library::addr() const {
    return this;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::system
