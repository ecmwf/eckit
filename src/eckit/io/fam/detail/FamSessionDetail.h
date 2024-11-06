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

/// @file   FamSessionDetail.h
/// @author Metin Cakircali
/// @date   Mar 2024

#pragma once

#include "eckit/io/fam/FamConfig.h"
#include "eckit/io/fam/FamObject.h"
#include "eckit/io/fam/FamProperty.h"
#include "eckit/io/fam/FamRegion.h"

#include <fam/fam.h>

#include <memory>
#include <string>

namespace eckit {

using FamDescriptorStatus = openfam::Fam_Descriptor_Status;

//----------------------------------------------------------------------------------------------------------------------
// SESSION

class FamSessionDetail: public std::enable_shared_from_this<FamSessionDetail> {
public:  // methods
    FamSessionDetail(const FamConfig& config);

    FamSessionDetail(const FamSessionDetail&)            = delete;
    FamSessionDetail& operator=(const FamSessionDetail&) = delete;
    FamSessionDetail(FamSessionDetail&&)                 = delete;
    FamSessionDetail& operator=(FamSessionDetail&&)      = delete;

    ~FamSessionDetail();

    auto getShared() -> std::shared_ptr<FamSessionDetail> { return shared_from_this(); }

    auto name() const -> std::string { return name_; }

    auto config() -> FamConfig;

    //------------------------------------------------------------------------------------------------------------------
    // REGION

    [[nodiscard]]
    auto lookupRegion(const std::string& regionName) -> FamRegion;

    [[nodiscard]]
    auto createRegion(const fam::size_t  regionSize,
                      const fam::perm_t  regionPerm,
                      const std::string& regionName) -> FamRegion;

    [[nodiscard]]
    auto createRegion(const FamProperty& property) -> FamRegion {
        return createRegion(property.size, property.perm, property.name);
    }

    void resizeRegion(FamRegionDescriptor& region, fam::size_t size);

    void destroyRegion(FamRegionDescriptor& region);

    void destroyRegion(const std::string& regionName);

    [[nodiscard]]
    auto ensureCreateRegion(const fam::size_t  regionSize,
                            const fam::perm_t  regionPerm,
                            const std::string& regionName) -> FamRegion;

    auto stat(FamRegionDescriptor& region) -> FamProperty;

    //------------------------------------------------------------------------------------------------------------------
    // OBJECT

    [[nodiscard]]
    auto proxyObject(std::uint64_t region, std::uint64_t offset) -> FamObject;

    [[nodiscard]]
    auto lookupObject(const std::string& regionName, const std::string& objectName) -> FamObject;

    [[nodiscard]]
    auto allocateObject(FamRegionDescriptor& region,
                        fam::size_t          objectSize,
                        fam::perm_t          objectPerm,
                        const std::string&   objectName = "") -> FamObject;

    [[nodiscard]]
    auto allocateObject(FamRegionDescriptor& region, const FamProperty& property) -> FamObject {
        return allocateObject(region, property.size, property.perm, property.name);
    }

    void deallocateObject(FamObjectDescriptor& object);

    void deallocateObject(const std::string& regionName, const std::string& objectName);

    /// IMPORTANT: This method will deallocate any existing object with the same name
    [[nodiscard]]
    auto ensureAllocateObject(FamRegionDescriptor& region,
                              fam::size_t          objectSize,
                              fam::perm_t          objectPerm,
                              const std::string&   objectName) -> FamObject;

    auto stat(FamObjectDescriptor& object) -> FamProperty;

    void put(FamObjectDescriptor& object, const void* buffer, fam::size_t offset, fam::size_t length);

    void get(FamObjectDescriptor& object, void* buffer, fam::size_t offset, fam::size_t length);

    //------------------------------------------------------------------------------------------------------------------
    // OBJECT - ATOMIC

    template<typename T>
    auto fetch(FamObjectDescriptor& object, fam::size_t offset) -> T;

    template<typename T>
    void set(FamObjectDescriptor& object, fam::size_t offset, T value);

    template<typename T>
    void add(FamObjectDescriptor& object, fam::size_t offset, T value);

    template<typename T>
    void subtract(FamObjectDescriptor& object, fam::size_t offset, T value);

    template<typename T>
    auto swap(FamObjectDescriptor& object, fam::size_t offset, T value) -> T;

    template<typename T>
    auto compareSwap(FamObjectDescriptor& object, fam::size_t offset, T oldValue, T newValue) -> T;

    //------------------------------------------------------------------------------------------------------------------

private:  // methods
    friend std::ostream& operator<<(std::ostream& out, const FamSessionDetail& session);

    void print(std::ostream& out) const;

private:  // members
    const std::string name_;

    openfam::fam fam_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
