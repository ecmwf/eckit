/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/*
 * This software was developed as part of the Horizon Europe programme funded project OpenCUBE
 * (Grant agreement: 101092984) horizon-opencube.eu
 */

/// @file   FamRegion.h
/// @author Metin Cakircali
/// @date   Mar 2024

#pragma once

#include "eckit/io/fam/FamObject.h"
#include "eckit/io/fam/FamProperty.h"

#include <memory>
#include <string>

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class FamRegion {
public:  // types
    using UPtr = std::unique_ptr<FamRegion>;
    using SPtr = std::shared_ptr<FamRegion>;

public:  // methods
    FamRegion(FamSessionDetail& session, std::unique_ptr<FamRegionDescriptor> region);

    ~FamRegion();

    auto clone() const -> UPtr;

    void destroy() const;

    auto exists() const -> bool;

    // properties

    auto index() const -> fam::index_t;

    auto size() const -> fam::size_t;

    auto permissions() const -> fam::perm_t;

    auto name() const -> std::string;

    auto property() const -> FamProperty;

    // object methods

    [[nodiscard]]
    auto proxyObject(fam::index_t offset) const -> FamObject;

    [[nodiscard]]
    auto lookupObject(const std::string& objectName) const -> FamObject;

    [[nodiscard]]
    auto allocateObject(fam::size_t        objectSize,
                        fam::perm_t        objectPerm,
                        const std::string& objectName = "",
                        bool               overwrite  = false) const -> FamObject;

    /// IMPOTANT: this uses the region's permissions for the object
    [[nodiscard]]
    auto allocateObject(fam::size_t        objectSize,
                        const std::string& objectName = "",
                        bool               overwrite  = false) const -> FamObject {
        return allocateObject(objectSize, permissions(), objectName, overwrite);
    }

    void deallocateObject(const std::string& objectName) const;

private:  // methods
    void print(std::ostream& out) const;

    friend std::ostream& operator<<(std::ostream& out, const FamRegion& region);

private:  // members
    std::shared_ptr<FamSessionDetail>    session_;
    std::shared_ptr<FamRegionDescriptor> region_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
