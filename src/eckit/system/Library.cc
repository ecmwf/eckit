/*
 * (C) Copyright 1996-2016 ECMWF.
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
#include "eckit/log/Channel.h"
#include "eckit/log/ChannelBuffer.h"
#include "eckit/log/MultiChannel.h"
#include "eckit/os/System.h"
#include "eckit/utils/Translator.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Mutex.h"

namespace eckit {
namespace system {

//----------------------------------------------------------------------------------------------------------------------

typedef std::map<std::string, Library*> LibraryMap;

static pthread_once_t once  = PTHREAD_ONCE_INIT;
static eckit::Mutex* local_mutex = 0;
static LibraryMap* m = 0;

static void init() {
    local_mutex = new eckit::Mutex();
    m = new LibraryMap();
}

//----------------------------------------------------------------------------------------------------------------------

std::vector<std::string> Library::list() {

    std::vector<std::string> result;

    pthread_once(&once, init);

    eckit::AutoLock<eckit::Mutex> lock(local_mutex);

    for (LibraryMap::const_iterator j = m->begin() ; j != m->end() ; ++j) {
        result.push_back(j->first);
    }
    return result;
}

void Library::list(std::ostream& out) {

    pthread_once(&once, init);

    eckit::AutoLock<eckit::Mutex> lock(local_mutex);

    const char* sep = "";
    for (LibraryMap::const_iterator j = m->begin() ; j != m->end() ; ++j) {
        out << sep << (*j).first;
        sep = ", ";
    }
}

bool Library::exists(const std::string& name) {

    pthread_once(&once, init);

    eckit::AutoLock<eckit::Mutex> lock(local_mutex);
    LibraryMap::const_iterator j = m->find(name);

    return (j != m->end());
}

const Library& Library::lookup(const std::string& name) {

    pthread_once(&once, init);

    eckit::AutoLock<eckit::Mutex> lock(local_mutex);
    LibraryMap::const_iterator j = m->find(name);

    // eckit::Log::info() << "Looking for Library '" << name << "'" << std::endl;

    if (j == m->end()) {
        eckit::Log::error() << "No Library found with name '" << name << "'" << std::endl;
        eckit::Log::error() << "Registered libraries are:" << std::endl;
        for (j = m->begin() ; j != m->end() ; ++j)
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

    ASSERT(m->find(name) == m->end());
    (*m)[name] = this;

    std::transform(prefix_.begin(), prefix_.end(), prefix_.begin(), ::toupper);

    std::string s = prefix_ + "_DEBUG";
    const char* e = ::getenv(s.c_str());
    if (e) {
        debug_ = eckit::Translator<std::string, bool>()(e);
    }
}

Library::~Library() {
}

std::string Library::name() const {
    return name_;
}

LocalPathName Library::path() const {
    return LocalPathName(libraryPath()).dirName().dirName();
}

std::string Library::libraryPath() const {
    if (libraryPath_.empty()) {
        std::string p = eckit::System::addrToPath(addr());
        libraryPath_ = LocalPathName(p).realName();
    }
    return libraryPath_;
}

bool Library::debug() const
{
    return debug_;
}

Channel& Library::debugChannel() const
{
    eckit::AutoLock<Mutex> lock(mutex_);

    if (debugChannel_) { return *debugChannel_; }

    std::string s = prefix_ + "_DEBUG";

    if (debug_) {
        debugChannel_.reset(new Channel( new ChannelBuffer( std::cout )));
    }
    else {

        debugChannel_.reset(new MultiChannel());
    }
    return *debugChannel_;
}

std::string Library::expandPath(const std::string& p) const {

    std::string s = "~" + name_;

    std::string result = path() + "/" + p.substr(s.size());

    return result;
}

void Library::print(std::ostream &out) const {
    out << "Library("
        << "name=" << name_
        << "path=" << libraryPath()
        << ")";
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace system
} // namespace eckit

