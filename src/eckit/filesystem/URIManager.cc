/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/filesystem/URIManager.h"
#include "eckit/config/LibEcKit.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/filesystem/URI.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/StaticMutex.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

typedef std::map<std::string, URIManager*> URIManagerMap;

// Builds the map on demand, needed for correct static initialization because factories can be
// initialized first
struct URIManagerRegistry {
    static URIManagerRegistry& instance() {
        static URIManagerRegistry reg;
        return reg;
    }

    URIManagerMap map_;

    URIManagerMap& map() { return map_; }
};


static StaticMutex local_mutex;

//----------------------------------------------------------------------------------------------------------------------

URIManager::URIManager(const std::string& name) : name_(name) {
    AutoLock<StaticMutex> lock(local_mutex);
    URIManagerMap& m = URIManagerRegistry::instance().map();

    ASSERT(m.find(name) == m.end());
    m[name] = this;
}

URIManager::~URIManager() {
    AutoLock<StaticMutex> lock(local_mutex);
    URIManagerMap& m = URIManagerRegistry::instance().map();

    m.erase(name_);
}

std::string URIManager::asString(const URI& f) const {
    return PathName(f.asRawString()).asString();
}

URIManager& URIManager::lookUp(const std::string& name) {
    AutoLock<StaticMutex> lock(local_mutex);
    URIManagerMap& m = URIManagerRegistry::instance().map();

    std::map<std::string, URIManager*>::const_iterator j = m.find(name);

    Log::debug<LibEcKit>() << "Looking for URIManager [" << name << "]" << std::endl;

    if (j == m.end()) {
        Log::error() << "No URIManager for [" << name << "]" << std::endl;
        Log::error() << "Managers are:" << std::endl;
        for (j = m.begin(); j != m.end(); ++j)
            Log::error() << "   " << *((*j).second) << std::endl;
        throw SeriousBug(std::string("No URIManager called ") + name);
    }

    return *((*j).second);
}


void URIManager::print(std::ostream& s) const {
    s << "URIManager[" << name_ << "]";
}

//----------------------------------------------------------------------------------------------------------------------


class LocalFileManager : public URIManager {
    virtual bool exists(URI& f) { return PathName(f.name()).exists(); }

    virtual DataHandle* newWriteHandle(URI& f) { return PathName(f.name()).fileHandle(); }

    virtual DataHandle* newReadHandle(URI& f) { return PathName(f.name()).fileHandle(); }

    virtual DataHandle* newReadHandle(URI& f, const OffsetList& ol, const LengthList& ll) {
        return PathName(f.name()).partHandle(ol, ll);
    }

    virtual std::string asString(const URI& f) const {
        return f.name();
    }

public:
    LocalFileManager(const std::string& name) : URIManager(name) {}
};


class MarsFSManager : public URIManager {
    virtual bool exists(URI& f) { return PathName(f.scheme() + ":" + f.name()).exists(); }

    virtual DataHandle* newWriteHandle(URI& f) { return PathName(f.scheme() + ":" + f.name()).fileHandle(); }

    virtual DataHandle* newReadHandle(URI& f) { return PathName(f.scheme() + ":" + f.name()).fileHandle(); }

    virtual DataHandle* newReadHandle(URI& f, const OffsetList& ol, const LengthList& ll) {
        return PathName(f.scheme() + ":" + f.name()).partHandle(ol, ll);
    }

public:
    MarsFSManager(const std::string& name) : URIManager(name) {}
};


static LocalFileManager manager_posix("posix");
static LocalFileManager manager_unix("unix");
static LocalFileManager manager_file("file");
static MarsFSManager manager_marsfs("marsfs");

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
