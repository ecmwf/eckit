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

Library::Library(const std::string& name) : name_(name), prefix_(name), debug_(false) {

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

const std::vector<std::string>& Library::homes() const {
    AutoLock<Mutex> lock(mutex_);

    if (!homes_.empty()) {
        return homes_;
    }

    std::string libhome = prefix_ + "_HOME";
    char* home_env = ::getenv(libhome.c_str());

    // Home is a colon separated list of directories
    if (home_env) {
        std::stringstream ss(home_env);
        std::string item;
        while (std::getline(ss, item, ':')) {
            homes_.push_back(item);
        }
    }
    else {
        // Default homes
        LocalPathName prefix = prefixDirectory();
        homes_ = {prefix, prefix.dirName(), prefix.dirName().dirName(), "/"};
    }

    ASSERT(!homes_.empty());
    return homes_;
}

std::string Library::libraryHome() const {
    const std::vector<std::string>&  h = homes();
    return h[0];
}

void Library::libraryHome(const std::string& home) {
    AutoLock<Mutex> lock(mutex_);
    homes_ = {home};
}

std::string Library::libraryPath() const {
    AutoLock<Mutex> lock(mutex_);

    if (libraryPath_.empty()) {
        std::string p = eckit::System::addrToPath(addr());
        libraryPath_  = LocalPathName(p).realName();
    }
    return libraryPath_;
}


Channel& Library::debugChannel() const {
    static ThreadSingleton<std::map<const Library*, std::unique_ptr<Channel>>> debugChannels;

    auto it = debugChannels.instance().find(this);
    if (it != debugChannels.instance().end()) {
        return *it->second;
    }

    return *debugChannels.instance()
                .emplace(this, debug_ ? std::make_unique<Channel>(new PrefixTarget(prefix_ + "_DEBUG"))  //
                                      : std::make_unique<Channel>())                                     //
                .first->second.get();
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

    //  If HOME is set for this library, either via env variable LIBNAME_HOME
    //  or set in code, expand ~lib/ to its content
    std::string tail = "/" + p.substr(s.size());
    const std::vector<std::string>& h = homes();

    for (const auto& home : h) {
        LocalPathName result = LocalPathName(home + tail);
        if (result.exists()) {
            return result;
        }
    }

    //  As a last resort expand with the first home entry, although we know the path doesn't exist
    return h[0] + tail;
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
