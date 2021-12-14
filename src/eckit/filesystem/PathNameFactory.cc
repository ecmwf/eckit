/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/filesystem/PathNameFactory.h"

#include "eckit/filesystem/BasePathNameT.h"
#include "eckit/filesystem/LocalPathName.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/StaticMutex.h"
#include "eckit/config/LibEcKit.h"
#include "eckit/filesystem/LocalPathName.h"

namespace eckit {

static PathNameBuilder<LocalPathName> localBuilder("local");

//----------------------------------------------------------------------------------------------------------------------

class PathNameFactoryImpl : private eckit::NonCopyable {
public:  // methods
    static PathNameFactoryImpl& instance();

    void enregister(const std::string& name, const PathNameBuilderBase* builder);
    void deregister(const PathNameBuilderBase* builder);

    BasePathName* build(const std::string& type, const std::string& path, bool tildeIsUserHome = false);

private:                        // methods
    StaticMutex static_mutex_;  //< must be StaticMutex, so it is registered to be clean on fork
    std::vector<std::string> names_;
    std::map<std::string, const PathNameBuilderBase*> builders_;
};

//----------------------------------------------------------------------------------------------------------------------

PathNameFactoryImpl& PathNameFactoryImpl::instance() {
    static PathNameFactoryImpl theInstance;
    return theInstance;
}

void PathNameFactoryImpl::enregister(const std::string& name, const PathNameBuilderBase* builder) {

    AutoLock<StaticMutex> lock(static_mutex_);

    if (builders_.find(name) != builders_.end()) {
        std::ostringstream ss;
        ss << "Cannot register PathNameBuilder '" << name << "'. A builder with this name already exists";
        throw SeriousBug(ss.str(), Here());
    }

    names_.push_back(name);
    builders_.emplace(name, builder);
}

void PathNameFactoryImpl::deregister(const PathNameBuilderBase* builder) {

    if(LibEcKit::instance().dontDeregisterFactories()) return;

    AutoLock<StaticMutex> lock(static_mutex_);

    for (auto it = builders_.begin(); it != builders_.end(); ++it) {
        if (it->second == builder) {
            auto it2 = std::find(names_.begin(), names_.end(), it->first);
            ASSERT(it2 != names_.end());
            names_.erase(it2);
            builders_.erase(it);
            return;
        }
    }

    std::ostringstream ss;
    ss << "Cannot deregister PathNameBuilder " << (void*)builder << ". Not found";
    throw SeriousBug(ss.str(), Here());
}

BasePathName* PathNameFactoryImpl::build(const std::string& type, const std::string& path, bool tildeIsUserHome) {

    // allow creation of local paths even after de-registration
    // useful for out-of-order destructors and _at_exit() exceptions
    if (type == "local") {
        return localBuilder.make(path, tildeIsUserHome);
    }

    AutoLock<StaticMutex> lock(static_mutex_);

    auto it = builders_.find(type);
    if (it == builders_.end()) {
        std::ostringstream ss;
        ss << "PathNameBuilder '" << type << "' not found";
        throw SeriousBug(ss.str(), Here());
    }

    return it->second->make(path, tildeIsUserHome);
}

//----------------------------------------------------------------------------------------------------------------------

PathNameBuilderBase::PathNameBuilderBase(const std::string& name) {
    PathNameFactoryImpl::instance().enregister(name, this);
}

PathNameBuilderBase::~PathNameBuilderBase() {
    PathNameFactoryImpl::instance().deregister(this);
}

//----------------------------------------------------------------------------------------------------------------------

BasePathName* PathNameFactory::build(const std::string& path, bool tildeIsUserHome) {
    std::string type = "local";

    // Restrict search for identifier to the start of the path
    size_t pos = path.rfind("://", 10);
    if (pos != std::string::npos) {
        type = path.substr(0, pos);
    }

    return PathNameFactoryImpl::instance().build(type, path, tildeIsUserHome);
}

BasePathName* PathNameFactory::build(const std::string& type, const std::string& path, bool tildeIsUserHome) {
    return PathNameFactoryImpl::instance().build(type, path, tildeIsUserHome);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
