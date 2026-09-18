// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Simon Smart
/// @date March 2020

#ifndef eckit_filesystem_PathNameFactory_h
#define eckit_filesystem_PathNameFactory_h

#include "eckit/filesystem/BasePathNameT.h"

#include <map>
#include <string>
#include <vector>

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class PathNameBuilderBase;
class BasePathName;

class PathNameFactory {
public:  // methods

    static BasePathName* build(const std::string& path, bool tildeIsUserHome = false);
    static BasePathName* build(const std::string& type, const std::string& path, bool tildeIsUserHome = false);
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
