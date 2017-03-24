/*
 * (C) Copyright 1996-2017 ECMWF.
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

#include <map>
#include <algorithm>
#include <cctype>

#include "eckit/system/Library.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/log/Log.h"
#include "eckit/log/OStreamTarget.h"
#include "eckit/log/PrefixTarget.h"
#include "eckit/os/System.h"
#include "eckit/utils/Translator.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Mutex.h"
#include "eckit/filesystem/LocalPathName.h"

namespace eckit {
namespace system {

//----------------------------------------------------------------------------------------------------------------------

typedef std::map<std::string, Library*> LibraryMap;

struct LibraryRegistry {

    static LibraryRegistry& instance() {
        static LibraryRegistry reg;
        return reg;
    }

    LibraryMap map_;

    LibraryMap& map() { return map_; }
};


static pthread_once_t once  = PTHREAD_ONCE_INIT;
static eckit::Mutex* local_mutex = 0;

static void init() {
    local_mutex = new eckit::Mutex();
}

//----------------------------------------------------------------------------------------------------------------------

std::vector<std::string> Library::list() {

    std::vector<std::string> result;

    pthread_once(&once, init);
    eckit::AutoLock<eckit::Mutex> lock(local_mutex);

    LibraryMap& m = LibraryRegistry::instance().map();

    for (LibraryMap::const_iterator j = m.begin() ; j != m.end() ; ++j) {
        result.push_back(j->first);
    }
    return result;
}

void Library::list(std::ostream& out) {

    pthread_once(&once, init);
    eckit::AutoLock<eckit::Mutex> lock(local_mutex);

    LibraryMap& m = LibraryRegistry::instance().map();

    const char* sep = "";
    for (LibraryMap::const_iterator j = m.begin() ; j != m.end() ; ++j) {
        out << sep << (*j).first;
        sep = ", ";
    }
}

bool Library::exists(const std::string& name) {

    pthread_once(&once, init);
    eckit::AutoLock<eckit::Mutex> lock(local_mutex);

    LibraryMap& m = LibraryRegistry::instance().map();

    LibraryMap::const_iterator j = m.find(name);

    return (j != m.end());
}

const Library& Library::lookup(const std::string& name) {

    pthread_once(&once, init);
    eckit::AutoLock<eckit::Mutex> lock(local_mutex);

    LibraryMap& m = LibraryRegistry::instance().map();

    LibraryMap::const_iterator j = m.find(name);

    // eckit::Log::info() << "Looking for Library '" << name << "'" << std::endl;

    if (j == m.end()) {
        eckit::Log::error() << "No Library found with name '" << name << "'" << std::endl;
        eckit::Log::error() << "Registered libraries are:" << std::endl;
        for (j = m.begin() ; j != m.end() ; ++j)
            eckit::Log::error() << "   " << (*j).first << std::endl;
        throw eckit::SeriousBug(std::string("No Library found with name ") + name);
    }

    ASSERT(j->second);

    return *(j->second);
}

Library::Library(const std::string& name) :
    name_(name),
    prefix_(name),
    debug_(false) {

    pthread_once(&once, init);
    eckit::AutoLock<eckit::Mutex> lock(local_mutex);

    LibraryMap& m = LibraryRegistry::instance().map();

    ASSERT(m.find(name) == m.end());
    m[name] = this;

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

Library::~Library() {
}

const std::string& Library::name() const {
    return name_;
}

std::string Library::prefixDirectory() const {
    eckit::AutoLock<Mutex> lock(mutex_);

    if (prefixDirectory_.empty()) {
        prefixDirectory_ = LocalPathName(libraryPath()).dirName().dirName().realName();
    }

    return prefixDirectory_;
}

std::string Library::home() const
{
    eckit::AutoLock<Mutex> lock(mutex_);

    std::string libhome = prefix_ + "_HOME";
    char* home = ::getenv(libhome.c_str());
    if(home) { return home; }

    return home_; // may return empty string (meaning not set)
}

void Library::libraryHome(const std::string& home)
{
    eckit::AutoLock<Mutex> lock(mutex_);
    home_ = home;
}

std::string Library::libraryPath() const {
    eckit::AutoLock<Mutex> lock(mutex_);

    if (libraryPath_.empty()) {
        std::string p = eckit::System::addrToPath(addr());
        libraryPath_ = LocalPathName(p).realName();
    }
    return libraryPath_;
}

Channel& Library::debugChannel() const
{
    eckit::AutoLock<Mutex> lock(mutex_);

    if (debugChannel_) { return *debugChannel_; }

    std::string s = prefix_ + "_DEBUG";

    if (debug_) {
        debugChannel_.reset(new Channel(new PrefixTarget(s)));
    }
    else {
        debugChannel_.reset(new Channel());
    }

    return *debugChannel_;
}

std::string Library::expandPath(const std::string& p) const {

    std::string s = "~" + name_;

    ASSERT(p.substr(0, s.size()) == s);
    ASSERT(p.size() == s.size() || p[s.size()] == '/');

    // 1. if HOME is set for this library, either via env variable LIBNAME_HOME exists
    //    or set in code expand ~lib/ to its content

    const std::string h = home();
    if(!h.empty()) {
        std::string result = h + "/" + p.substr(s.size());
        return result;
    }

    // 2. try to walk up the path and check for paths that exist

    const std::string extra = "/" + p.substr(s.size());

    eckit::LocalPathName path = prefixDirectory();
    eckit::LocalPathName root("/");

    while(true) {

        LocalPathName tmp = path + extra;

        if(tmp.exists()) return tmp;

        if(path == root) break;

        path = path.dirName();
    }

    // 3. as a last resort expand with prefix directory although we know the path doesn't exist

    return prefixDirectory() + extra;
}

void Library::print(std::ostream &out) const {
    out << "Library("
        << "name=" << name_
        << ", path=" << libraryPath()
        << ", prefix=" << prefixDirectory()
        << ")";
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace system
} // namespace eckit

