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

#include "FamSessionDetail.h"

#include <sys/types.h>  // mode_t

#include <algorithm>
#include <cctype>  // isspace isprint
#include <cstdint>
#include <cstring>
#include <ostream>
#include <string>
#include <string_view>
#include <utility>

#include "fam/fam.h"
#include "fam/fam_exception.h"

#include "eckit/config/LibEcKit.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/io/fam/FamConfig.h"
#include "eckit/io/fam/FamObject.h"
#include "eckit/io/fam/FamProperty.h"
#include "eckit/io/fam/FamRegion.h"
#include "eckit/log/CodeLocation.h"
#include "eckit/log/Log.h"
#include "eckit/net/Endpoint.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------
// HELPERS

namespace {

template <typename Func, typename... Args>
auto invokeFam(openfam::fam& fam, Func&& fn_ptr, Args&&... args) {
    try {
        return (fam.*std::forward<Func>(fn_ptr))(std::forward<Args>(args)...);
    }
    catch (openfam::Fam_Exception& e) {
        const auto code = e.fam_error();
        if (code == openfam::Fam_Error::FAM_ERR_NOTFOUND) {
            throw NotFound(e.fam_error_msg());
        }
        if (code == openfam::Fam_Error::FAM_ERR_ALREADYEXIST) {
            throw AlreadyExists(e.fam_error_msg());
        }
        if (code == openfam::Fam_Error::FAM_ERR_NOPERM) {
            throw PermissionDenied(e.fam_error_msg());
        }
        if (code == openfam::Fam_Error::FAM_ERR_INVALID) {
            throw BadValue(e.fam_error_msg());
        }
        if (code == openfam::Fam_Error::FAM_ERR_NO_SPACE) {
            throw OutOfStorage(e.fam_error_msg());
        }
        if (code == openfam::Fam_Error::FAM_ERR_OUTOFRANGE) {
            throw OutOfRange(e.fam_error_msg(), Here());
        }
        if (code == openfam::Fam_Error::FAM_ERR_METADATA) {
            throw NotFound(e.fam_error_msg());
        }
        if (code == openfam::Fam_Error::FAM_ERR_RPC) {
            std::string option_name       = "CIS_SERVER";
            const std::string server_name = static_cast<const char*>(fam.fam_get_option(option_name.data()));
            throw RemoteException(e.fam_error_msg(), server_name);
        }
        throw SeriousBug("Code=" + std::to_string(code) + ' ' + e.fam_error_msg());
    }
}

auto isValidName(std::string_view str) -> bool {
    if (str.empty()) {
        return false;
    }
    return std::all_of(str.begin(), str.end(), [](char c) { return std::isprint(c) > 0 && std::isspace(c) == 0; });
}

}  // namespace

//----------------------------------------------------------------------------------------------------------------------
// SESSION

FamSessionDetail::FamSessionDetail(const FamConfig& config) : name_{config.sessionName} {
    ASSERT(isValidName(name_));

    Log::debug<LibEcKit>() << "Initializing FAM session: " << config << '\n';

    try {
        // pins
        auto runtime = std::string{"NONE"};
        auto host    = config.endpoint.host();
        auto port    = std::to_string(config.endpoint.port());

        Fam_Options options;
        ::memset(static_cast<void*>(&options), 0, sizeof(Fam_Options));
        options.runtime   = runtime.data();
        options.cisServer = host.data();
        options.grpcPort  = port.data();

        fam_.fam_initialize(name_.c_str(), &options);
    }
    catch (openfam::Fam_Exception& e) {
        fam_.fam_abort(-1);
        throw Exception(e.fam_error_msg(), Here());
    }
}

FamSessionDetail::~FamSessionDetail() {
    try {
        fam_.fam_finalize(name_.c_str());
    }
    catch (openfam::Fam_Exception& e) {
        Log::error() << "Failed to finalize session: " << name_ << ", msg=" << e.fam_error_msg() << '\n';
        fam_.fam_abort(-1);
    }
}

//----------------------------------------------------------------------------------------------------------------------

void FamSessionDetail::print(std::ostream& out) const {
    out << "FamSessionDetail[name=" << name_ << "]";
}

std::ostream& operator<<(std::ostream& out, const FamSessionDetail& session) {
    session.print(out);
    return out;
}

//----------------------------------------------------------------------------------------------------------------------
// REGION

auto FamSessionDetail::config() -> FamConfig {
    std::string cis_server = "CIS_SERVER";
    std::string grpc_port  = "GRPC_PORT";

    const auto* host = static_cast<const char*>(fam_.fam_get_option(cis_server.data()));
    const auto* port = static_cast<const char*>(fam_.fam_get_option(grpc_port.data()));

    return {{host, std::stoi(port)}, name_};
}

auto FamSessionDetail::lookupRegion(const std::string& region_name) -> FamRegion {
    ASSERT(isValidName(region_name));

    auto* region = invokeFam(fam_, &openfam::fam::fam_lookup_region, region_name.c_str());

    return {*this, region};
}

auto FamSessionDetail::createRegion(const fam::size_t region_size, const fam::perm_t region_perm,
                                    const std::string& region_name) -> FamRegion {
    ASSERT(region_size > 0);
    ASSERT(isValidName(region_name));

    auto* region =
        invokeFam(fam_, &openfam::fam::fam_create_region, region_name.c_str(), region_size, region_perm, nullptr);

    return {*this, region};
}

void FamSessionDetail::resizeRegion(FamRegionDescriptor& region, const fam::size_t size) {
    ASSERT(size > 0);

    invokeFam(fam_, &openfam::fam::fam_resize_region, &region, size);
}

void FamSessionDetail::destroyRegion(FamRegionDescriptor& region) {
    invokeFam(fam_, &openfam::fam::fam_destroy_region, &region);
}

void FamSessionDetail::destroyRegion(const std::string& region_name) {
    lookupRegion(region_name).destroy();
}

auto FamSessionDetail::ensureCreateRegion(const fam::size_t region_size, const fam::perm_t region_perm,
                                          const std::string& region_name) -> FamRegion {
    try {
        return createRegion(region_size, region_perm, region_name);
    }
    catch (const AlreadyExists& e) {
        destroyRegion(region_name);
        return createRegion(region_size, region_perm, region_name);
    }
}

auto FamSessionDetail::stat(FamRegionDescriptor& region) -> FamProperty {
    Fam_Stat info;

    auto fn_ptr = static_cast<void (openfam::fam::*)(FamRegionDescriptor*, Fam_Stat*)>(&openfam::fam::fam_stat);
    invokeFam(fam_, fn_ptr, &region, &info);

    return {info.size, info.perm, info.name, info.uid, info.gid};
}

//----------------------------------------------------------------------------------------------------------------------
//  OBJECT

auto FamSessionDetail::proxyObject(const std::uint64_t region, const std::uint64_t offset) -> FamObject {
    return {*this, region, offset};
}

auto FamSessionDetail::lookupObject(const std::string& region_name, const std::string& object_name) -> FamObject {
    ASSERT(isValidName(region_name));
    ASSERT(isValidName(object_name));

    auto* object = invokeFam(fam_, &openfam::fam::fam_lookup, object_name.c_str(), region_name.c_str());

    return {*this, object};
}

auto FamSessionDetail::allocateObject(FamRegionDescriptor& region, const fam::size_t object_size,
                                      const fam::perm_t object_perm, const std::string& object_name) -> FamObject {
    ASSERT(object_size > 0);

    auto allocate =
        static_cast<FamObjectDescriptor* (openfam::fam::*)(const char*, uint64_t, mode_t, FamRegionDescriptor*)>(
            &openfam::fam::fam_allocate);

    auto* object = invokeFam(fam_, allocate, object_name.c_str(), object_size, object_perm, &region);

    return {*this, object};
}

void FamSessionDetail::deallocateObject(FamObjectDescriptor& object) {
    invokeFam(fam_, &openfam::fam::fam_deallocate, &object);
}

void FamSessionDetail::deallocateObject(const std::string& region_name, const std::string& object_name) {
    lookupObject(region_name, object_name).deallocate();
}

auto FamSessionDetail::ensureAllocateObject(FamRegionDescriptor& region, const fam::size_t object_size,
                                            const fam::perm_t object_perm, const std::string& object_name)
    -> FamObject {
    try {
        return allocateObject(region, object_size, object_perm, object_name);
    }
    catch (const AlreadyExists& e) {
        deallocateObject(region.get_name(), object_name);
        return allocateObject(region, object_size, object_perm, object_name);
    }
}

auto FamSessionDetail::stat(FamObjectDescriptor& object) -> FamProperty {
    Fam_Stat info;

    auto fn_ptr = static_cast<void (openfam::fam::*)(FamObjectDescriptor*, Fam_Stat*)>(&openfam::fam::fam_stat);
    invokeFam(fam_, fn_ptr, &object, &info);

    return {info.size, info.perm, info.name, info.uid, info.gid};
}

void FamSessionDetail::put(FamObjectDescriptor& object, const void* buffer, const fam::size_t offset,
                           const fam::size_t length) {
    ASSERT(buffer);
    ASSERT(length > 0);

    /// @note we have to remove "const" qualifier from buffer
    invokeFam(fam_, &openfam::fam::fam_put_blocking, const_cast<void*>(buffer), &object, offset, length);
}

void FamSessionDetail::get(FamObjectDescriptor& object, void* buffer, const fam::size_t offset,
                           const fam::size_t length) {
    ASSERT(buffer);
    ASSERT(length > 0);

    invokeFam(fam_, &openfam::fam::fam_get_blocking, buffer, &object, offset, length);
}

//----------------------------------------------------------------------------------------------------------------------
// OBJECT - ATOMIC

template <typename T>
auto FamSessionDetail::fetch(FamObjectDescriptor& /* object */, const fam::size_t /* offset */) -> T {
    throw SeriousBug("This type is not specialized!");
}

template <>
auto FamSessionDetail::fetch(FamObjectDescriptor& object, const fam::size_t offset) -> int32_t {
    return invokeFam(fam_, &openfam::fam::fam_fetch_int32, &object, offset);
}

template <>
auto FamSessionDetail::fetch(FamObjectDescriptor& object, const fam::size_t offset) -> int64_t {
    return invokeFam(fam_, &openfam::fam::fam_fetch_int64, &object, offset);
}

template <>
auto FamSessionDetail::fetch(FamObjectDescriptor& object, const fam::size_t offset) -> openfam::int128_t {
    return invokeFam(fam_, &openfam::fam::fam_fetch_int128, &object, offset);
}

template <>
auto FamSessionDetail::fetch(FamObjectDescriptor& object, const fam::size_t offset) -> uint32_t {
    return invokeFam(fam_, &openfam::fam::fam_fetch_uint32, &object, offset);
}

template <>
auto FamSessionDetail::fetch(FamObjectDescriptor& object, const fam::size_t offset) -> uint64_t {
    return invokeFam(fam_, &openfam::fam::fam_fetch_uint64, &object, offset);
}

template <>
auto FamSessionDetail::fetch(FamObjectDescriptor& object, const fam::size_t offset) -> float {
    return invokeFam(fam_, &openfam::fam::fam_fetch_float, &object, offset);
}

template <>
auto FamSessionDetail::fetch(FamObjectDescriptor& object, const fam::size_t offset) -> double {
    return invokeFam(fam_, &openfam::fam::fam_fetch_double, &object, offset);
}

template <typename T>
void FamSessionDetail::set(FamObjectDescriptor& object, const fam::size_t offset, const T value) {
    auto fptr = static_cast<void (openfam::fam::*)(FamObjectDescriptor*, fam::size_t, T)>(&openfam::fam::fam_set);
    invokeFam(fam_, fptr, &object, offset, value);
}

template <typename T>
void FamSessionDetail::add(FamObjectDescriptor& object, const fam::size_t offset, const T value) {
    auto fptr = static_cast<void (openfam::fam::*)(FamObjectDescriptor*, fam::size_t, T)>(&openfam::fam::fam_add);
    invokeFam(fam_, fptr, &object, offset, value);
}

template <typename T>
void FamSessionDetail::subtract(FamObjectDescriptor& object, const fam::size_t offset, const T value) {
    auto fptr = static_cast<void (openfam::fam::*)(FamObjectDescriptor*, fam::size_t, T)>(&openfam::fam::fam_subtract);
    invokeFam(fam_, fptr, &object, offset, value);
}

template <typename T>
auto FamSessionDetail::swap(FamObjectDescriptor& object, const fam::size_t offset, const T value) -> T {  // NOLINT
    auto fptr = static_cast<T (openfam::fam::*)(FamObjectDescriptor*, fam::size_t, T)>(&openfam::fam::fam_swap);
    return invokeFam(fam_, fptr, &object, offset, value);
}

template <typename T>
auto FamSessionDetail::compareSwap(FamObjectDescriptor& object, const fam::size_t offset, const T old_value,
                                   const T new_value) -> T {
    auto fptr =
        static_cast<T (openfam::fam::*)(FamObjectDescriptor*, fam::size_t, T, T)>(&openfam::fam::fam_compare_swap);
    return invokeFam(fam_, fptr, &object, offset, old_value, new_value);
}

//----------------------------------------------------------------------------------------------------------------------
// forward instantiations

template void FamSessionDetail::set(FamObjectDescriptor&, const fam::size_t, const int32_t);
template void FamSessionDetail::set(FamObjectDescriptor&, const fam::size_t, const int64_t);
template void FamSessionDetail::set(FamObjectDescriptor&, const fam::size_t, const openfam::int128_t);
template void FamSessionDetail::set(FamObjectDescriptor&, const fam::size_t, const uint32_t);
template void FamSessionDetail::set(FamObjectDescriptor&, const fam::size_t, const uint64_t);
template void FamSessionDetail::set(FamObjectDescriptor&, const fam::size_t, const float);
template void FamSessionDetail::set(FamObjectDescriptor&, const fam::size_t, const double);

template void FamSessionDetail::add(FamObjectDescriptor&, const fam::size_t, const int32_t);
template void FamSessionDetail::add(FamObjectDescriptor&, const fam::size_t, const int64_t);
template void FamSessionDetail::add(FamObjectDescriptor&, const fam::size_t, const uint32_t);
template void FamSessionDetail::add(FamObjectDescriptor&, const fam::size_t, const uint64_t);
template void FamSessionDetail::add(FamObjectDescriptor&, const fam::size_t, const float);
template void FamSessionDetail::add(FamObjectDescriptor&, const fam::size_t, const double);

template void FamSessionDetail::subtract(FamObjectDescriptor&, const fam::size_t, const int32_t);
template void FamSessionDetail::subtract(FamObjectDescriptor&, const fam::size_t, const int64_t);
template void FamSessionDetail::subtract(FamObjectDescriptor&, const fam::size_t, const uint32_t);
template void FamSessionDetail::subtract(FamObjectDescriptor&, const fam::size_t, const uint64_t);
template void FamSessionDetail::subtract(FamObjectDescriptor&, const fam::size_t, const float);
template void FamSessionDetail::subtract(FamObjectDescriptor&, const fam::size_t, const double);

template auto FamSessionDetail::swap(FamObjectDescriptor&, const fam::size_t, const int32_t) -> int32_t;
template auto FamSessionDetail::swap(FamObjectDescriptor&, const fam::size_t, const int64_t) -> int64_t;
template auto FamSessionDetail::swap(FamObjectDescriptor&, const fam::size_t, const uint32_t) -> uint32_t;
template auto FamSessionDetail::swap(FamObjectDescriptor&, const fam::size_t, const uint64_t) -> uint64_t;
template auto FamSessionDetail::swap(FamObjectDescriptor&, const fam::size_t, const float) -> float;
template auto FamSessionDetail::swap(FamObjectDescriptor&, const fam::size_t, const double) -> double;

template auto FamSessionDetail::compareSwap(FamObjectDescriptor&, const fam::size_t, const int32_t, const int32_t)
    -> int32_t;
template auto FamSessionDetail::compareSwap(FamObjectDescriptor&, const fam::size_t, const int64_t, const int64_t)
    -> int64_t;
template auto FamSessionDetail::compareSwap(FamObjectDescriptor&, const fam::size_t, const uint32_t, const uint32_t)
    -> uint32_t;
template auto FamSessionDetail::compareSwap(FamObjectDescriptor&, const fam::size_t, const uint64_t, const uint64_t)
    -> uint64_t;

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
