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

#include "eckit/io/fam/FamConfig.h"
#include "eckit/io/fam/FamObject.h"
#include "eckit/io/fam/FamProperty.h"

#include <memory>
#include <string>

namespace eckit {

class FamRegionDetail;

//----------------------------------------------------------------------------------------------------------------------

class FamRegion {
public:  // types
    using UPtr = std::unique_ptr<FamRegion>;
    using SPtr = std::shared_ptr<FamRegion>;

public:  // factory methods
    static auto lookup(const std::string& name, const FamConfig& config) -> UPtr;

    static auto create(const FamProperty& property, const FamConfig& config) -> UPtr;

    static auto ensureCreated(const FamProperty& property, const FamConfig& config) -> UPtr;

public:  // methods
    explicit FamRegion(std::unique_ptr<FamRegionDetail> region) noexcept;

    ~FamRegion();

    void destroy() const;

    auto exists() const -> bool;

    auto name() const -> const char*;

    auto index() const -> std::uint64_t;

    auto size() const -> fam::size_t;

    auto permissions() const -> fam::perm_t;

    auto property() const -> FamProperty { return {size(), permissions(), name()}; }

    // OBJECT factory methods

    auto proxyObject(fam::size_t offset) const -> FamObject::UPtr;

    auto lookupObject(const std::string& name) const -> FamObject::UPtr;

    auto allocateObject(const FamProperty& property) const -> FamObject::UPtr;

    /// @note inherits permissions from region
    auto allocateObject(fam::size_t size, const std::string& name = "") const -> FamObject::UPtr;

    void deallocateObject(const std::string& name) const;

    auto ensureAllocatedObject(const FamProperty& property) const -> FamObject::UPtr;

private:  // methods
    void print(std::ostream& out) const;

    friend std::ostream& operator<<(std::ostream& out, const FamRegion& region);

private:  // members
    std::unique_ptr<FamRegionDetail> impl_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
