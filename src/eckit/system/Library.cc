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

#include "eckit/system/Library.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/log/Log.h"
#include "eckit/os/System.h"
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

    for(LibraryMap::const_iterator j = m->begin() ; j != m->end() ; ++j) {
        result.push_back(j->first);
    }
    return result;
}

void Library::list(std::ostream& out) {

    pthread_once(&once, init);

    eckit::AutoLock<eckit::Mutex> lock(local_mutex);

    const char* sep = "";
    for(LibraryMap::const_iterator j = m->begin() ; j != m->end() ; ++j) {
        out << sep << (*j).first;
        sep = ", ";
    }
}

const Library& Library::get(const std::string& name) {

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
    name_(name) {

    pthread_once(&once, init);
    eckit::AutoLock<eckit::Mutex> lock(local_mutex);

    ASSERT(m->find(name) == m->end());
    (*m)[name] = this;
}

Library::~Library() {
}

std::string Library::name() const
{
    return name_;
}

std::string Library::location() const
{
    return eckit::System::addrToPath(addr());
}

LocalPathName Library::path() const
{
    std::string loc = location();
    return LocalPathName(loc).realName();
}

void Library::print(std::ostream &out) const {
    out << "Library("
        << "name=" << name_
        << "location=" << this->location()
        << ")";
}

//----------------------------------------------------------------------------------------------------------------------

class LibEcKit : public eckit::system::Library {
public:
    LibEcKit() : Library("eckit") {}
protected:
    const void* addr() const { return this; }
};

static LibEcKit libeckit;

//----------------------------------------------------------------------------------------------------------------------

} // namespace system
} // namespace eckit

