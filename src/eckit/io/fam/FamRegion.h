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

    /// Bare region number, as carried by the *region* descriptor.
    /// This region's objects report a different id — theirs packs in the memory server serving them — so this must
    /// never be used to address an object. @see FamDescriptor
    fam::index_t index() const;

    fam::size_t size() const;

    fam::perm_t permissions() const;

    std::string name() const;

    FamProperty property() const;

    void setObjectLevelPermissions() const;

    void setRegionLevelPermissions() const;

    // object methods

    /// Wraps an already-allocated object at @p descriptor, reusing this region's session.
    /// The descriptor addresses the object on its own memory server, which need not match index().
    /// @note this avoids invoking fam, as in lookupObject.
    FamObject proxyObject(const FamDescriptor& descriptor) const;

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

    void print(std::ostream& out) const;

    friend std::ostream& operator<<(std::ostream& out, const FamRegion& region);

private:  // members

    std::shared_ptr<FamSession> session_;
    std::shared_ptr<FamRegionDescriptor> region_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
