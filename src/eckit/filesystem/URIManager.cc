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
#include "eckit/thread/Mutex.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

typedef std::map<std::string, URIManager*> URIManagerMap;

/// Registry for all URI managers
///
class URIManagerRegistry {
public:  // methods
    /// Builds the registry on demand, needed for correct static initialization
    /// because factories can be initialized first
    static URIManagerRegistry& instance() {
        static URIManagerRegistry reg;
        return reg;
    }

    /// Registers an entry to the registry
    /// @pre Cannot exist yet
    void enregister(const std::string& name, URIManager* obj) {
        AutoLock<Mutex> lockme(mutex_);
        ASSERT(map_.find(name) == map_.end());
        map_[name] = obj;
    }

    /// Removes an entry from the registry
    /// @pre Must exist
    void deregister(const std::string& name) {
        if(LibEcKit::instance().dontDeregisterFactories()) return;
        AutoLock<Mutex> lockme(mutex_);
        ASSERT(map_.find(name) != map_.end());
        map_.erase(name);
    }

    /// List entries in URIManager
    std::vector<std::string> list() const {
        AutoLock<Mutex> lockme(mutex_);
        std::vector<std::string> result;
        for (URIManagerMap::const_iterator j = map_.begin(); j != map_.end(); ++j) {
            result.push_back(j->first);
        }
        return result;
    }

    /// Check entry exists in registry
    bool exists(const std::string& name) const {
        AutoLock<Mutex> lockme(mutex_);
        URIManagerMap::const_iterator j = map_.find(name);
        return (j != map_.end());
    }

    /// Prints the entries in registry
    void print(std::ostream& out, const char* separator) const {
        AutoLock<Mutex> lockme(mutex_);
        std::vector<std::string> l = URIManagerRegistry::instance().list();
        const char* sep            = "";
        for (auto j : l) {
            out << sep << j;
            sep = separator;
        }
    }

    /// Lookup entry in the registry
    URIManager& lookup(const std::string& name) const {
        AutoLock<Mutex> lockme(mutex_);

        URIManagerMap::const_iterator j = map_.find(name);

        if (j == map_.end()) {
            eckit::Log::error() << "No URIManager found with name '" << name << "'" << std::endl;
            eckit::Log::error() << "Registered URIManager's are:";
            print(eckit::Log::error(), "\n");
            throw eckit::SeriousBug(std::string("No URIManager found with name ") + name);
        }

        ASSERT(j->second);
        return *(j->second);
    }

private:  // members
    URIManagerMap map_;
    mutable Mutex mutex_;
};


//----------------------------------------------------------------------------------------------------------------------

URIManager::URIManager(const std::string& name) :
    name_(name) {
    URIManagerRegistry::instance().enregister(name, this);
}

URIManager::~URIManager() {
    URIManagerRegistry::instance().deregister(name_);
}

std::string URIManager::asString(const URI& uri) const {
    return path(uri).asString();
}

PathName URIManager::path(const URI& uri) const {
    // There is no general mechanism
    NOTIMP;
    // return PathName(uri.scheme() + ":" + uri.name());
}

bool URIManager::exists(const std::string& name) {
    return URIManagerRegistry::instance().exists(name);
}

URIManager& URIManager::lookUp(const std::string& name) {
    return URIManagerRegistry::instance().lookup(name);
}

void URIManager::print(std::ostream& s) const {
    s << "URIManager[" << name_ << "]";
}

//----------------------------------------------------------------------------------------------------------------------

class LocalFilePartManager : public URIManager {
    virtual bool query() override { return true; }
    virtual bool fragment() override { return true; }

    virtual bool exists(const URI& uri) override { return path(uri).exists(); }

    virtual DataHandle* newWriteHandle(const URI& uri) override { return path(uri).fileHandle(); }

    virtual DataHandle* newReadHandle(const URI& uri) override { return path(uri).fileHandle(); }

    virtual DataHandle* newReadHandle(const URI& uri, const OffsetList& ol, const LengthList& ll) override {
        return path(uri).partHandle(ol, ll);
    }

    virtual std::string asString(const URI& uri) const override { return uri.name(); }

    PathName path(const URI& uri) const override { return PathName("local", uri.name()); }

public:
    LocalFilePartManager(const std::string& name) :
        URIManager(name) {}
};

//----------------------------------------------------------------------------------------------------------------------

class HttpURIManager : public URIManager {
    virtual bool authority() override { return true; }
    virtual bool query() override { return true; }
    virtual bool fragment() override { return true; }

    virtual bool exists(const URI& uri) override { return PathName(uri.scheme() + ":" + uri.name()).exists(); }

    virtual DataHandle* newWriteHandle(const URI& uri) override {
        return PathName(uri.scheme() + ":" + uri.name()).fileHandle();
    }

    virtual DataHandle* newReadHandle(const URI& uri) override {
        return PathName(uri.scheme() + ":" + uri.name()).fileHandle();
    }

    virtual DataHandle* newReadHandle(const URI& uri, const OffsetList& ol, const LengthList& ll) override {
        return PathName(uri.scheme() + ":" + uri.name()).partHandle(ol, ll);
    }

    virtual std::string asString(const URI& uri) const override {

        std::string auth = uri.authority();
        if (!auth.empty())
            auth = "//" + auth;
        std::string q = uri.query();
        if (!q.empty())
            q = "?" + q;
        std::string f = uri.fragment();
        if (!f.empty())
            f = "#" + f;

        return uri.scheme() + ":" + auth + uri.name() + q + f;
    }

public:
    HttpURIManager(const std::string& name) :
        URIManager(name) {}
};

//----------------------------------------------------------------------------------------------------------------------

static LocalFilePartManager manager_file("file");
static HttpURIManager manager_http("http");
static HttpURIManager manager_https("https");

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
