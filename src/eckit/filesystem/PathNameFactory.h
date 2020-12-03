/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Simon Smart
/// @date March 2020

#ifndef eckit_filesystem_PathNameFactory_h
#define eckit_filesystem_PathNameFactory_h

#include "eckit/filesystem/BasePathNameT.h"
#include "eckit/memory/NonCopyable.h"

#include <map>
#include <mutex>
#include <string>
#include <vector>

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class PathNameBuilderBase;
class BasePathName;

class PathNameFactory : private eckit::NonCopyable {
public:  // methods
    static PathNameFactory& instance();

    void enregister(const std::string& name, const PathNameBuilderBase* builder);
    void deregister(const PathNameBuilderBase* builder);

    BasePathName* build(const std::string& path, bool tildeIsUserHome = false) const;
    BasePathName* build(const std::string& type, const std::string& path, bool tildeIsUserHome = false) const;

private:  // methods
    PathNameFactory();

    mutable std::mutex m_;
    std::vector<std::string> names_;
    std::map<std::string, const PathNameBuilderBase*> builders_;
};


class PathNameBuilderBase {
public:
    PathNameBuilderBase(const std::string& name);
    virtual ~PathNameBuilderBase();
    virtual BasePathName* make(const std::string& path, bool tildeIsUserHome) const = 0;
};

template <typename T>
class PathNameBuilder : public PathNameBuilderBase {
public:
    using PathNameBuilderBase::PathNameBuilderBase;
    BasePathName* make(const std::string& path, bool tildeIsUserHome) const override {
        return new BasePathNameT<T>(path, tildeIsUserHome);
    }
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
