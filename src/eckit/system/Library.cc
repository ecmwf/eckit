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
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Mutex.h"
#include "eckit/utils/Translator.h"

namespace eckit {
namespace system {

//----------------------------------------------------------------------------------------------------------------------

/// Registry for all libraries
///
class LibraryRegistry {

typedef std::map<std::string, Library*> LibraryMap;

public:  // methods

    /// Builds the registry on demand, needed for correct static initialization
    /// because factories can be initialized first
    static LibraryRegistry& instance() {
        static LibraryRegistry reg;
        return reg;
    }

    /// Registers an entry to the registry
    /// @pre Cannot exist yet
    /// @param obj pointer must be valid
    void enregister(const std::string& name, Library* obj) {
        AutoLock<Mutex> lockme(mutex_);
        ASSERT(obj);
        ASSERT(map_.find(name) == map_.end());
        map_[name] = obj;
    }

    /// Removes an entry from the registry
    /// @pre Must exist
    void deregister(const std::string& name) {
        AutoLock<Mutex> lockme(mutex_);
        ASSERT(map_.find(name) != map_.end());
        map_.erase(name);
    }

    /// List entries in library
    std::vector<std::string> list() const {
        AutoLock<Mutex> lockme(mutex_);
        std::vector<std::string> result;
        for (LibraryMap::const_iterator j = map_.begin(); j != map_.end(); ++j) {
            result.push_back(j->first);
        }
        return result;
    }

    /// Check entry exists in registry
    bool exists(const std::string& name) const {
        AutoLock<Mutex> lockme(mutex_);
        LibraryMap::const_iterator j = map_.find(name);
        return (j != map_.end());
    }

    /// Prints the entries in registry
    void print(std::ostream& out, const char* separator) const {
        AutoLock<Mutex> lockme(mutex_);
        std::vector<std::string> l = LibraryRegistry::instance().list();
        const char* sep               = "";
        for (auto j : l) {
            out << sep << j;
            sep = separator;
        }
    }

    /// Lookup entry in the registry
    Library& lookup(const std::string& name) const {
        AutoLock<Mutex> lockme(mutex_);

        LibraryMap::const_iterator j = map_.find(name);

        if (j == map_.end()) {
            eckit::Log::error() << "No Library found with name '" << name << "'" << std::endl;
            eckit::Log::error() << "Registered libraries are:";
            print(eckit::Log::error(), "\n");
            throw eckit::SeriousBug(std::string("No Library found with name ") + name);
        }

        ASSERT(j->second);
        return *(j->second);
    }

private:  // members
    LibraryMap map_;
    mutable Mutex mutex_;
};

//----------------------------------------------------------------------------------------------------------------------

std::vector<std::string> Library::list() {
    return LibraryRegistry::instance().list();
}

void Library::list(std::ostream& out) {
    return LibraryRegistry::instance().print(out, ", ");
}

bool Library::exists(const std::string& name) {
    return LibraryRegistry::instance().exists(name);
}

const Library& Library::lookup(const std::string& name) {
    return LibraryRegistry::instance().lookup(name);
}

Library::Library(const std::string& name) : name_(name), prefix_(name), debug_(false) {

    LibraryRegistry::instance().enregister(name, this);

    std::transform(prefix_.begin(), prefix_.end(), prefix_.begin(), ::toupper);

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

Channel& Library::debugChannel() const {
    AutoLock<Mutex> lock(mutex_);

    if (debugChannel_) {
        return *debugChannel_;
    }

    std::string s = prefix_ + "_DEBUG";

    if (debug_) {
        debugChannel_.reset(new Channel(new PrefixTarget(s)));
    }
    else {
        debugChannel_.reset(new Channel());
    }

    return *debugChannel_;
}

const Configuration& Library::configuration() const {
    AutoLock<Mutex> lock(mutex_);

    if (configuration_)
        return *configuration_;

    std::string s = "$" + prefix_ + "_CONFIG_PATH";
    std::string p = "~" + name_ + "/etc/" + name_ + "/config.yaml";

    eckit::PathName cfgpath = eckit::Resource<eckit::PathName>(s.c_str(), p.c_str());

    Log::debug() << "Parsing Lib " << name_ << " config file " << cfgpath << std::endl;

    eckit::Configuration* cfg =
        cfgpath.exists() ? new eckit::YAMLConfiguration(cfgpath) : new eckit::YAMLConfiguration(std::string(""));

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

        if (tmp.exists())
            return tmp;

        if (path == root)
            break;

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

}  // namespace system
}  // namespace eckit
