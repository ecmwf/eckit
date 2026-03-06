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

#include <chrono>
#include <cstdint>
#include <iosfwd>
#include <memory>
#include <string>

// #include "fam/fam.h"

#include "eckit/io/fam/FamObject.h"
#include "eckit/io/fam/FamProperty.h"
#include "eckit/io/fam/FamRegion.h"
#include "eckit/net/Endpoint.h"

namespace openfam {
class fam;
}  // namespace openfam

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------
// SESSION

class FamSessionDetail : public std::enable_shared_from_this<FamSessionDetail> {
public:  // types

    using TimePoint = std::chrono::system_clock::time_point;

public:  // methods

    FamSessionDetail(std::string name, const net::Endpoint& endpoint);

    // rule of five
    FamSessionDetail(const FamSessionDetail&)            = delete;
    FamSessionDetail& operator=(const FamSessionDetail&) = delete;
    FamSessionDetail(FamSessionDetail&&)                 = delete;
    FamSessionDetail& operator=(FamSessionDetail&&)      = delete;

    ~FamSessionDetail();

    // Operations

    bool operator==(const FamSessionDetail& other) const {
        return name_ == other.name_ && endpoint_ == other.endpoint_;
    }

    bool operator!=(const FamSessionDetail& other) const { return !(*this == other); }

    // Accessors

    const std::string& name() const { return name_; }

    const TimePoint& lastAccess() const { return lastAccess_; }

    const net::Endpoint& endpoint() const { return endpoint_; }

    // Modifiers

    void updateLastAccess(const TimePoint& when = std::chrono::system_clock::now()) { lastAccess_ = when; }

    //------------------------------------------------------------------------------------------------------------------
    // REGION

    auto lookupRegion(const std::string& region_name) -> FamRegion;

    auto createRegion(fam::size_t region_size, fam::perm_t region_perm, const std::string& region_name) -> FamRegion;

    auto createRegion(const FamProperty& property) -> FamRegion {
        return createRegion(property.size, property.perm, property.name);
    }

    void resizeRegion(FamRegionDescriptor& region, fam::size_t size);

    void destroyRegion(FamRegionDescriptor& region);

    void destroyRegion(const std::string& region_name);

    auto ensureCreateRegion(fam::size_t region_size, fam::perm_t region_perm, const std::string& region_name)
        -> FamRegion;

    auto stat(FamRegionDescriptor& region) -> FamProperty;

    //------------------------------------------------------------------------------------------------------------------
    // OBJECT

    auto proxyObject(std::uint64_t region, std::uint64_t offset) -> FamObject;

    auto lookupObject(const std::string& region_name, const std::string& object_name) -> FamObject;

    auto allocateObject(FamRegionDescriptor& region, fam::size_t object_size, fam::perm_t object_perm,
                        const std::string& object_name = "") -> FamObject;

    auto allocateObject(FamRegionDescriptor& region, const FamProperty& property) -> FamObject {
        return allocateObject(region, property.size, property.perm, property.name);
    }

    void deallocateObject(FamObjectDescriptor& object);

    void deallocateObject(const std::string& region_name, const std::string& object_name);

    /// IMPORTANT: This method will deallocate any existing object with the same name
    auto ensureAllocateObject(FamRegionDescriptor& region, fam::size_t object_size, fam::perm_t object_perm,
                              const std::string& object_name) -> FamObject;

    auto stat(FamObjectDescriptor& object) -> FamProperty;

    void put(FamObjectDescriptor& object, const void* buffer, fam::size_t offset, fam::size_t length);

    void get(FamObjectDescriptor& object, void* buffer, fam::size_t offset, fam::size_t length);

    //------------------------------------------------------------------------------------------------------------------
    // OBJECT - ATOMIC

    template <typename T>
    auto fetch(FamObjectDescriptor& object, fam::size_t offset) -> T;

    template <typename T>
    void set(FamObjectDescriptor& object, fam::size_t offset, T value);

    template <typename T>
    void add(FamObjectDescriptor& object, fam::size_t offset, T value);

    template <typename T>
    void subtract(FamObjectDescriptor& object, fam::size_t offset, T value);

    template <typename T>
    auto swap(FamObjectDescriptor& object, fam::size_t offset, T value) -> T;

    template <typename T>
    auto compareSwap(FamObjectDescriptor& object, fam::size_t offset, T old_value, T new_value) -> T;

    //------------------------------------------------------------------------------------------------------------------

private:  // methods

    friend std::ostream& operator<<(std::ostream& out, const FamSessionDetail& session);

    void print(std::ostream& out) const;

    template <typename Func, typename... Args>
    auto invokeFam(Func&& fn_ptr, Args&&... args);

private:  // members

    std::string name_;

    net::Endpoint endpoint_;

    TimePoint lastAccess_{std::chrono::system_clock::now()};

    std::unique_ptr<openfam::fam> fam_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
