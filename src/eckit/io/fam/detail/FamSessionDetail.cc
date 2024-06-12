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

#include "eckit/config/LibEcKit.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/io/fam/FamConfig.h"
#include "eckit/log/Log.h"

#include <fam/fam_exception.h>

#include <algorithm>
#include <cctype>   // isspace isprint
#include <cstring>  // memset strndup
#include <memory>
#include <string>
#include <utility>

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------
// HELPERS

namespace {

template<typename Func, typename... Args>
auto invokeFam(openfam::fam& fam, Func&& fnPtr, Args&&... args) {
    try {
        return (fam.*std::forward<Func>(fnPtr))(std::forward<Args>(args)...);
    } catch (openfam::Fam_Exception& e) {
        const auto code = e.fam_error();
        if (code == openfam::Fam_Error::FAM_ERR_NOTFOUND) { throw NotFound(e.fam_error_msg()); }
        if (code == openfam::Fam_Error::FAM_ERR_ALREADYEXIST) { throw AlreadyExists(e.fam_error_msg()); }
        if (code == openfam::Fam_Error::FAM_ERR_NOPERM) { throw PermissionDenied(e.fam_error_msg()); }
        if (code == openfam::Fam_Error::FAM_ERR_INVALID) { throw BadValue(e.fam_error_msg()); }
        if (code == openfam::Fam_Error::FAM_ERR_RPC) { throw RemoteException(e.fam_error_msg(), ""); }
        throw SeriousBug(e.fam_error_msg());
    }
}

auto isValidName(std::string_view str) -> bool {
    if (str.empty()) { return false; }
    return std::all_of(str.begin(), str.end(), [](char c) { return std::isprint(c) > 0 && std::isspace(c) == 0; });
}

}  // namespace

//----------------------------------------------------------------------------------------------------------------------
// SESSION

FamSessionDetail::FamSessionDetail(FamConfig config): config_ {std::move(config)} {
    ASSERT(isValidName(config_.sessionName));

    Log::debug<LibEcKit>() << "Initializing FAM session: " << config_ << '\n';

    try {
        // pins
        auto runtime = std::string {"NONE"};
        auto host    = config_.endpoint.host();
        auto port    = std::to_string(config_.endpoint.port());

        Fam_Options options;
        memset(static_cast<void*>(&options), 0, sizeof(Fam_Options));
        options.runtime   = runtime.data();
        options.cisServer = host.data();
        options.grpcPort  = port.data();

        fam_.fam_initialize(config_.sessionName.c_str(), &options);
    } catch (openfam::Fam_Exception& e) {
        fam_.fam_abort(-1);
        throw Exception(e.fam_error_msg(), Here());
    }
}

FamSessionDetail::~FamSessionDetail() {
    Log::debug<LibEcKit>() << "Finalizing FAM session: " << config_ << '\n';
    try {
        fam_.fam_finalize(config_.sessionName.c_str());
    } catch (openfam::Fam_Exception& e) {
        Log::error() << "Failed to finalize session: " << config_ << ", msg=" << e.fam_error_msg() << '\n';
        fam_.fam_abort(-1);
    }
}

//----------------------------------------------------------------------------------------------------------------------

void FamSessionDetail::print(std::ostream& out) const {
    out << "FamSessionDetail[" << config_ << "]";
}

std::ostream& operator<<(std::ostream& out, const FamSessionDetail& session) {
    session.print(out);
    return out;
}

//----------------------------------------------------------------------------------------------------------------------
// REGION

auto FamSessionDetail::lookupRegion(const std::string& regionName) -> FamRegion {
    ASSERT(isValidName(regionName));

    auto* region = invokeFam(fam_, &openfam::fam::fam_lookup_region, regionName.c_str());

    return {*this, std::unique_ptr<FamRegionDescriptor>(region)};
}

auto FamSessionDetail::createRegion(const fam::size_t  regionSize,
                                    const fam::perm_t  regionPerm,
                                    const std::string& regionName) -> FamRegion {
    ASSERT(regionSize > 0);
    ASSERT(isValidName(regionName));

    LOG_DEBUG_LIB(LibEcKit) << "Create region: name=" << regionName << ", size=" << regionSize << '\n';

    auto* region = invokeFam(fam_, &openfam::fam::fam_create_region, regionName.c_str(), regionSize, regionPerm, nullptr);

    return {*this, std::unique_ptr<FamRegionDescriptor>(region)};
}

void FamSessionDetail::resizeRegion(FamRegionDescriptor& region, const fam::size_t size) {
    ASSERT(size > 0);

    LOG_DEBUG_LIB(LibEcKit) << "Resize region: name=" << region.get_name() << ", new size=" << size << '\n';

    invokeFam(fam_, &openfam::fam::fam_resize_region, &region, size);
}

void FamSessionDetail::destroyRegion(FamRegionDescriptor& region) {
    LOG_DEBUG_LIB(LibEcKit) << "Destroy region: name=" << region.get_name() << '\n';

    invokeFam(fam_, &openfam::fam::fam_destroy_region, &region);
}

void FamSessionDetail::destroyRegion(const std::string& regionName) {
    lookupRegion(regionName).destroy();
}

auto FamSessionDetail::ensureCreateRegion(const fam::size_t  regionSize,
                                          const fam::perm_t  regionPerm,
                                          const std::string& regionName) -> FamRegion {
    try {
        return createRegion(regionSize, regionPerm, regionName);
    } catch (const AlreadyExists& e) {
        destroyRegion(regionName);
        return createRegion(regionSize, regionPerm, regionName);
    }
}

auto FamSessionDetail::stat(FamRegionDescriptor& region) -> FamProperty {
    Fam_Stat info;

    auto fnPtr = static_cast<void (openfam::fam::*)(FamRegionDescriptor*, Fam_Stat*)>(&openfam::fam::fam_stat);
    invokeFam(fam_, fnPtr, &region, &info);

    return {info.size, info.perm, info.name, info.uid, info.gid};
}

//----------------------------------------------------------------------------------------------------------------------
//  OBJECT

auto FamSessionDetail::proxyObject(const std::uint64_t region, const std::uint64_t offset) -> FamObject {
    return {*this, std::make_unique<FamObjectDescriptor>(Fam_Global_Descriptor {region, offset})};
}

auto FamSessionDetail::lookupObject(const std::string& regionName, const std::string& objectName) -> FamObject {
    ASSERT(isValidName(regionName));
    ASSERT(isValidName(objectName));

    auto* object = invokeFam(fam_, &openfam::fam::fam_lookup, objectName.c_str(), regionName.c_str());

    return {*this, std::unique_ptr<FamObjectDescriptor>(object)};
}

auto FamSessionDetail::allocateObject(FamRegionDescriptor& region,
                                      const fam::size_t    objectSize,
                                      const fam::perm_t    objectPerm,
                                      const std::string&   objectName) -> FamObject {
    ASSERT(objectSize > 0);

    // LOG_DEBUG_LIB(LibEcKit) << "Allocate object: name=" << objectName << ", size=" << objectSize
    //                         << ", region=" << region.get_name() << '\n';

    auto allocate =
        static_cast<FamObjectDescriptor* (openfam::fam::*)(const char*, uint64_t, mode_t, FamRegionDescriptor*)>(
            &openfam::fam::fam_allocate);

    auto* object = invokeFam(fam_, allocate, objectName.c_str(), objectSize, objectPerm, &region);

    return {*this, std::unique_ptr<FamObjectDescriptor>(object)};
}

void FamSessionDetail::deallocateObject(FamObjectDescriptor& object) {
    LOG_DEBUG_LIB(LibEcKit) << "Deallocate object: name=" << object.get_name() << '\n';

    invokeFam(fam_, &openfam::fam::fam_deallocate, &object);
}

void FamSessionDetail::deallocateObject(const std::string& regionName, const std::string& objectName) {
    lookupObject(regionName, objectName).deallocate();
}

auto FamSessionDetail::ensureAllocateObject(FamRegionDescriptor& region,
                                            const fam::size_t    objectSize,
                                            const fam::perm_t    objectPerm,
                                            const std::string&   objectName) -> FamObject {
    try {
        return allocateObject(region, objectSize, objectPerm, objectName);
    } catch (const AlreadyExists& e) {
        deallocateObject(region.get_name(), objectName);
        return allocateObject(region, objectSize, objectPerm, objectName);
    }
}

auto FamSessionDetail::stat(FamObjectDescriptor& object) -> FamProperty {
    Fam_Stat info;

    auto fnPtr = static_cast<void (openfam::fam::*)(FamObjectDescriptor*, Fam_Stat*)>(&openfam::fam::fam_stat);
    invokeFam(fam_, fnPtr, &object, &info);

    return {info.size, info.perm, info.name, info.uid, info.gid};
}

void FamSessionDetail::put(FamObjectDescriptor& object,
                           const void*          buffer,
                           const fam::size_t    offset,
                           const fam::size_t    length) {
    ASSERT(buffer);
    ASSERT(length > 0);

    /// @note we have to remove "const" qualifier from buffer
    invokeFam(fam_, &openfam::fam::fam_put_blocking, const_cast<void*>(buffer), &object, offset, length);
}

void FamSessionDetail::get(FamObjectDescriptor& object, void* buffer, const fam::size_t offset, const fam::size_t length) {
    ASSERT(buffer);
    ASSERT(length > 0);

    invokeFam(fam_, &openfam::fam::fam_get_blocking, buffer, &object, offset, length);
}

//----------------------------------------------------------------------------------------------------------------------
// OBJECT - ATOMIC

template<typename T>
auto FamSessionDetail::fetch(FamObjectDescriptor& /* object */, const fam::size_t /* offset */) -> T {
    throw SeriousBug("This type is not specialized!");
}

template<>
auto FamSessionDetail::fetch(FamObjectDescriptor& object, const fam::size_t offset) -> int32_t {
    return invokeFam(fam_, &openfam::fam::fam_fetch_int32, &object, offset);
}

template<>
auto FamSessionDetail::fetch(FamObjectDescriptor& object, const fam::size_t offset) -> int64_t {
    return invokeFam(fam_, &openfam::fam::fam_fetch_int64, &object, offset);
}

template<>
auto FamSessionDetail::fetch(FamObjectDescriptor& object, const fam::size_t offset) -> openfam::int128_t {
    return invokeFam(fam_, &openfam::fam::fam_fetch_int128, &object, offset);
}

template<>
auto FamSessionDetail::fetch(FamObjectDescriptor& object, const fam::size_t offset) -> uint32_t {
    return invokeFam(fam_, &openfam::fam::fam_fetch_uint32, &object, offset);
}

template<>
auto FamSessionDetail::fetch(FamObjectDescriptor& object, const fam::size_t offset) -> uint64_t {
    return invokeFam(fam_, &openfam::fam::fam_fetch_uint64, &object, offset);
}

template<>
auto FamSessionDetail::fetch(FamObjectDescriptor& object, const fam::size_t offset) -> float {
    return invokeFam(fam_, &openfam::fam::fam_fetch_float, &object, offset);
}

template<>
auto FamSessionDetail::fetch(FamObjectDescriptor& object, const fam::size_t offset) -> double {
    return invokeFam(fam_, &openfam::fam::fam_fetch_double, &object, offset);
}

template<typename T>
void FamSessionDetail::set(FamObjectDescriptor& object, const fam::size_t offset, const T value) {
    auto fptr = static_cast<void (openfam::fam::*)(FamObjectDescriptor*, fam::size_t, T)>(&openfam::fam::fam_set);
    invokeFam(fam_, fptr, &object, offset, value);
}

template<typename T>
void FamSessionDetail::add(FamObjectDescriptor& object, const fam::size_t offset, const T value) {
    auto fptr = static_cast<void (openfam::fam::*)(FamObjectDescriptor*, fam::size_t, T)>(&openfam::fam::fam_add);
    invokeFam(fam_, fptr, &object, offset, value);
}

template<typename T>
void FamSessionDetail::subtract(FamObjectDescriptor& object, const fam::size_t offset, const T value) {
    auto fptr = static_cast<void (openfam::fam::*)(FamObjectDescriptor*, fam::size_t, T)>(&openfam::fam::fam_subtract);
    invokeFam(fam_, fptr, &object, offset, value);
}

template<typename T>
auto FamSessionDetail::swap(FamObjectDescriptor& object, const fam::size_t offset, const T value) -> T {  // NOLINT
    auto fptr = static_cast<T (openfam::fam::*)(FamObjectDescriptor*, fam::size_t, T)>(&openfam::fam::fam_swap);
    return invokeFam(fam_, fptr, &object, offset, value);
}

template<typename T>
auto FamSessionDetail::compareSwap(FamObjectDescriptor& object,
                                   const fam::size_t    offset,
                                   const T              oldValue,
                                   const T              newValue) -> T {
    auto fptr =
        static_cast<T (openfam::fam::*)(FamObjectDescriptor*, fam::size_t, T, T)>(&openfam::fam::fam_compare_swap);
    return invokeFam(fam_, fptr, &object, offset, oldValue, newValue);
}

//----------------------------------------------------------------------------------------------------------------------
// forward instantiations

template auto FamSessionDetail::fetch(FamObjectDescriptor&, const uint64_t) -> int32_t;
template auto FamSessionDetail::fetch(FamObjectDescriptor&, const uint64_t) -> int64_t;
template auto FamSessionDetail::fetch(FamObjectDescriptor&, const uint64_t) -> openfam::int128_t;
template auto FamSessionDetail::fetch(FamObjectDescriptor&, const uint64_t) -> uint32_t;
template auto FamSessionDetail::fetch(FamObjectDescriptor&, const uint64_t) -> uint64_t;
template auto FamSessionDetail::fetch(FamObjectDescriptor&, const uint64_t) -> float;
template auto FamSessionDetail::fetch(FamObjectDescriptor&, const uint64_t) -> double;

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

template auto FamSessionDetail::compareSwap(FamObjectDescriptor&, const fam::size_t, const int32_t, const int32_t) -> int32_t;
template auto FamSessionDetail::compareSwap(FamObjectDescriptor&, const fam::size_t, const int64_t, const int64_t) -> int64_t;
template auto FamSessionDetail::compareSwap(FamObjectDescriptor&,
                                            const fam::size_t,
                                            const uint32_t,
                                            const uint32_t) -> uint32_t;
template auto FamSessionDetail::compareSwap(FamObjectDescriptor&,
                                            const fam::size_t,
                                            const uint64_t,
                                            const uint64_t) -> uint64_t;

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
