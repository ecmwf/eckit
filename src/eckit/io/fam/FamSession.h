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

/// @file   FamSession.h
/// @author Metin Cakircali
/// @date   Mar 2024

#pragma once

#include <chrono>
#include <cstdint>
#include <iosfwd>
#include <memory>
#include <string>

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

class FamSession : public std::enable_shared_from_this<FamSession> {
public:  // types

    using TimePoint = std::chrono::system_clock::time_point;

public:  // methods

    FamSession(std::string name, const net::Endpoint& endpoint);

    // rule of five
    FamSession(const FamSession&)            = delete;
    FamSession& operator=(const FamSession&) = delete;
    FamSession(FamSession&&)                 = delete;
    FamSession& operator=(FamSession&&)      = delete;

    ~FamSession();

    // Operations

    bool operator==(const FamSession& other) const { return name_ == other.name_ && endpoint_ == other.endpoint_; }

    bool operator!=(const FamSession& other) const { return !(*this == other); }

    // Accessors

    const std::string& name() const { return name_; }

    const TimePoint& lastAccess() const { return lastAccess_; }

    const net::Endpoint& endpoint() const { return endpoint_; }

    // Modifiers

    void updateLastAccess(const TimePoint& when = std::chrono::system_clock::now()) { lastAccess_ = when; }

    //------------------------------------------------------------------------------------------------------------------
    // REGION

    FamRegion lookupRegion(const std::string& region_name);

    FamRegion createRegion(fam::size_t region_size, fam::perm_t region_perm, const std::string& region_name);

    FamRegion createRegion(const FamProperty& property) {
        return createRegion(property.size, property.perm, property.name);
    }

    void resizeRegion(FamRegionDescriptor& region, fam::size_t size);

    void destroyRegion(FamRegionDescriptor& region);

    void destroyRegion(const std::string& region_name);

    FamRegion ensureCreateRegion(fam::size_t region_size, fam::perm_t region_perm, const std::string& region_name);

    FamProperty stat(FamRegionDescriptor& region);

    //------------------------------------------------------------------------------------------------------------------
    // OBJECT

    FamObject proxyObject(std::uint64_t region, std::uint64_t offset);

    FamObject lookupObject(const std::string& region_name, const std::string& object_name);

    FamObject allocateObject(FamRegionDescriptor& region, fam::size_t object_size, fam::perm_t object_perm,
                             const std::string& object_name = "");

    FamObject allocateObject(FamRegionDescriptor& region, const FamProperty& property) {
        return allocateObject(region, property.size, property.perm, property.name);
    }

    void deallocateObject(FamObjectDescriptor& object);

    void deallocateObject(const std::string& region_name, const std::string& object_name);

    /// IMPORTANT: This method will deallocate any existing object with the same name
    FamObject ensureAllocateObject(FamRegionDescriptor& region, fam::size_t object_size, fam::perm_t object_perm,
                                   const std::string& object_name);

    FamProperty stat(FamObjectDescriptor& object);

    void put(FamObjectDescriptor& object, const void* buffer, fam::size_t offset, fam::size_t length);

    void get(FamObjectDescriptor& object, void* buffer, fam::size_t offset, fam::size_t length);

    //------------------------------------------------------------------------------------------------------------------
    // OBJECT - ATOMIC

    template <typename T>
    T fetch(FamObjectDescriptor& object, fam::size_t offset);

    template <typename T>
    void set(FamObjectDescriptor& object, fam::size_t offset, T value);

    template <typename T>
    void add(FamObjectDescriptor& object, fam::size_t offset, T value);

    template <typename T>
    void subtract(FamObjectDescriptor& object, fam::size_t offset, T value);

    template <typename T>
    T swap(FamObjectDescriptor& object, fam::size_t offset, T value);

    template <typename T>
    T compareSwap(FamObjectDescriptor& object, fam::size_t offset, T old_value, T new_value);

    //------------------------------------------------------------------------------------------------------------------

private:  // methods

    friend std::ostream& operator<<(std::ostream& out, const FamSession& session);

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
