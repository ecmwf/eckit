/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Tiago Quintino
/// @date   November 2020

#include <algorithm>
#include <cctype>
#include <map>

#include <dlfcn.h> // for dlopen

#include "eckit/system/LibraryManager.h"

#include "eckit/config/Resource.h"
#include "eckit/config/YAMLConfiguration.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/LocalPathName.h"
#include "eckit/log/Log.h"
#include "eckit/log/OStreamTarget.h"
#include "eckit/log/PrefixTarget.h"
#include "eckit/os/System.h"
#include "eckit/system/SystemInfo.h"
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
        const char* sep            = "";
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

    void load(const std::string& name) {
        AutoLock<Mutex> lockme(mutex_);

        static std::vector<std::string> libPaths(Resource<std::vector<std::string>>(
            "dynamicLibraryPath;$DYNAMIC_LIBRARY_PATH", {"~/lib64", "~/lib", "~eckit/lib64", "~eckit/lib"}));

        if (!exists(name)) {

            // Get the (system specific) library name for the given library instance

            std::string libraryName = SystemInfo::instance().dynamicLibraryName(name);

            // Try the various paths in the way

            for (const eckit::PathName& dir : libPaths) {

                eckit::PathName p = dir / libraryName;
                if (p.exists()) {
                    void* plib = ::dlopen(p.localPath(), RTLD_NOW | RTLD_GLOBAL);
                    if (plib == nullptr) {
                        std::ostringstream ss;
                        ss << "dlopen(" << p << ", ...)";
                        throw FailedSystemCall(ss.str().c_str(), Here(), errno);
                    }

                    // If the library still doesn't exist after a successful call of dlopen, then
                    // we have managed to load something other than a (self-registering) eckit library
                    if (!exists(name)) {
                        std::ostringstream ss;
                        ss << "Shared library " << p << " loaded but Library " << name << " not registered";
                        throw UnexpectedState(ss.str(), Here());
                    }

                    return;
                }
            }

            // Not found!!!
            std::ostringstream ss;
            ss << "Library " << name << " not found";
            throw SeriousBug(ss.str(), Here());
        }
    }

private:  // members
    LibraryMap map_;
    mutable Mutex mutex_;
};

//----------------------------------------------------------------------------------------------------------------------

void LibraryManager::enregister(const std::string& name, Library* obj) { 
    LibraryRegistry::instance().enregister(name, obj);
}

std::vector<std::string> LibraryManager::list() {
    return LibraryRegistry::instance().list();
}

void LibraryManager::list(std::ostream& out) {
    return LibraryRegistry::instance().print(out, ", ");
}

bool LibraryManager::exists(const std::string& name) {
    return LibraryRegistry::instance().exists(name);
}

const Library& LibraryManager::lookup(const std::string& name) {
    return LibraryRegistry::instance().lookup(name);
}

/* const Library& */ void LibraryManager::load(const std::string& name) {
    // return 
    LibraryRegistry::instance().load(name);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace system
}  // namespace eckit
