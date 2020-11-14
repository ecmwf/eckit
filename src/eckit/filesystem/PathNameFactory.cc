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

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

PathNameFactory& PathNameFactory::instance() {
    static PathNameFactory theInstance;
    return theInstance;
}

PathNameFactory::PathNameFactory() {}

void PathNameFactory::enregister(const std::string& name, const PathNameBuilderBase* builder) {

    std::lock_guard<std::mutex> lock_(m_);

    if (builders_.find(name) != builders_.end()) {
        std::ostringstream ss;
        ss << "Cannot register PathNameBuilder '" << name << "'. A builder with this name already exists";
        throw SeriousBug(ss.str(), Here());
    }

    names_.push_back(name);
    builders_.emplace(name, builder);
}

void PathNameFactory::deregister(const PathNameBuilderBase* builder) {

    std::lock_guard<std::mutex> lock_(m_);

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
BasePathName* PathNameFactory::build(const std::string& path, bool tildeIsUserHome) const {

    std::string type = "local";

    // Restrict search for identifier to the start of the path
    size_t pos = path.rfind("://", 10);
    if (pos != std::string::npos) {
        type = path.substr(0, pos);
    }

    return build(type, path, tildeIsUserHome);
}

BasePathName* PathNameFactory::build(const std::string& type, const std::string& path, bool tildeIsUserHome) const {

    std::lock_guard<std::mutex> lock_(m_);

    auto it = builders_.find(type);
    if (it == builders_.end()) {
        std::ostringstream ss;
        ss << "PathNameBuilder '" << type << "' not found";
        throw SeriousBug(ss.str(), Here());
    }

    return it->second->make(path, tildeIsUserHome);
}

PathNameBuilderBase::PathNameBuilderBase(const std::string& name) {
    PathNameFactory::instance().enregister(name, this);
}

PathNameBuilderBase::~PathNameBuilderBase() {
    PathNameFactory::instance().deregister(this);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
