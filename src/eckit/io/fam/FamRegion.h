// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/*
 * This software was developed as part of the Horizon Europe programme funded project OpenCUBE
 * (Grant agreement: 101092984) horizon-opencube.eu
 */

/// @file   FamRegion.h
/// @author Metin Cakircali
/// @date   Mar 2024

#pragma once

#include <iosfwd>
#include <memory>
#include <string>

#include "eckit/io/fam/FamObject.h"
#include "eckit/io/fam/FamProperty.h"
#include "eckit/io/fam/FamTypes.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class FamRegion {

public:  // methods

    FamRegion(FamSession& session, FamRegionDescriptor* region);

    void destroy() const;

    bool exists() const;

    // properties

    fam::index_t index() const;

    /// Region id as carried by this region's data items, which OpenFAM may report
    /// differently from the region descriptor's own id; falls back to index().
    fam::index_t objectIndex() const;

    /// Seed objectIndex() when it is known from a previously stored descriptor.
    void useObjectIndex(fam::index_t index) const;

    fam::size_t size() const;

    fam::perm_t permissions() const;

    std::string name() const;

    FamProperty property() const;

    void setObjectLevelPermissions() const;

    void setRegionLevelPermissions() const;

    // object methods

    /// @note this avoids invoking fam, as in lookupObject.
    FamObject proxyObject(fam::index_t offset) const;

    FamObject lookupObject(const std::string& object_name) const;

    FamObject allocateObject(fam::size_t object_size, fam::perm_t object_perm, const std::string& object_name = "",
                             bool overwrite = false) const;

    FamObject allocateObject(fam::size_t object_size, const std::string& object_name = "",
                             bool overwrite = false) const {
        return allocateObject(object_size, permissions(), object_name, overwrite);
    }

    /// Allocate a named object, or look it up if it already exists (idempotent).
    FamObject ensureObject(fam::size_t object_size, const std::string& object_name) const;

    void deallocateObject(const std::string& object_name) const;

private:  // methods

    /// Objects are registered under the id they carry, so remember it for proxying.
    void noteObjectIndex(const FamObject& object) const;

    void print(std::ostream& out) const;

    friend std::ostream& operator<<(std::ostream& out, const FamRegion& region);

private:  // members

    std::shared_ptr<FamSession> session_;
    std::shared_ptr<FamRegionDescriptor> region_;

    // 0 means "not yet observed"
    mutable std::shared_ptr<fam::index_t> objectIndex_{std::make_shared<fam::index_t>(0)};
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
