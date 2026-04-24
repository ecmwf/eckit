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

#include "FamSession.h"

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

std::unique_ptr<openfam::fam> initializeFamSession(const std::string& name, const net::Endpoint& endpoint) {
    Log::debug<LibEcKit>() << "Initializing FAM session: " << name << " with endpoint " << endpoint << '\n';

    auto fam = std::make_unique<openfam::fam>();

    try {
        // pins
        auto runtime = std::string{"NONE"};
        auto host    = endpoint.host();
        auto port    = std::to_string(endpoint.port());

        Fam_Options options;
        ::memset(static_cast<void*>(&options), 0, sizeof(Fam_Options));
        options.runtime   = runtime.data();
        options.cisServer = host.data();
        options.grpcPort  = port.data();

        fam->fam_initialize(name.c_str(), &options);
    }
    catch (openfam::Fam_Exception& e) {
        fam->fam_abort(-1);
        throw Exception(e.fam_error_msg(), Here());
    }

    return fam;
}

bool isValidName(std::string_view str) {
    if (str.empty()) {
        return false;
    }
    return std::all_of(str.begin(), str.end(), [](char chr) {
        const auto uchr = static_cast<unsigned char>(chr);
        return std::isprint(uchr) != 0 && std::isspace(uchr) == 0;
    });
}


}  // namespace

//----------------------------------------------------------------------------------------------------------------------
// SESSION

FamSession::FamSession(std::string name, const net::Endpoint& endpoint) : name_{std::move(name)}, endpoint_{endpoint} {
    ASSERT(isValidName(name_));
}

FamSession::~FamSession() {
    if (fam_) {
        try {
            fam_->fam_finalize(name_.c_str());
        }
        catch (openfam::Fam_Exception& e) {
            Log::error() << "Failed to finalize session: " << name_ << ", msg=" << e.fam_error_msg() << '\n';
            fam_->fam_abort(-1);
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------

void FamSession::print(std::ostream& out) const {
    out << "FamSession[name=" << name_ << "]";
}

std::ostream& operator<<(std::ostream& out, const FamSession& session) {
    session.print(out);
    return out;
}

template <typename Func, typename... Args>
auto FamSession::invokeFam(Func&& fn_ptr, Args&&... args) {
    if (!fam_) {
        fam_ = initializeFamSession(name_, endpoint_);
    }

    try {
        return (fam_.get()->*std::forward<Func>(fn_ptr))(std::forward<Args>(args)...);
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
            const auto* server_cstr       = static_cast<const char*>(fam_->fam_get_option(option_name.data()));
            const std::string server_name = server_cstr ? server_cstr : "<unknown>";
            throw RemoteException(e.fam_error_msg(), server_name);
        }
        throw SeriousBug("Code=" + std::to_string(code) + ' ' + e.fam_error_msg());
    }
}


//----------------------------------------------------------------------------------------------------------------------
// REGION

FamRegion FamSession::lookupRegion(const std::string& region_name) {
    ASSERT(isValidName(region_name));

    auto* region = invokeFam(&openfam::fam::fam_lookup_region, region_name.c_str());

    return {*this, region};
}

FamRegion FamSession::createRegion(const fam::size_t region_size, const fam::perm_t region_perm,
                                   const std::string& region_name) {
    ASSERT(region_size > 0);
    ASSERT(isValidName(region_name));

    auto* region = invokeFam(&openfam::fam::fam_create_region, region_name.c_str(), region_size, region_perm, nullptr);

    return {*this, region};
}

void FamSession::resizeRegion(FamRegionDescriptor& region, const fam::size_t size) {
    ASSERT(size > 0);

    invokeFam(&openfam::fam::fam_resize_region, &region, size);
}

void FamSession::destroyRegion(FamRegionDescriptor& region) {
    invokeFam(&openfam::fam::fam_destroy_region, &region);
}

void FamSession::destroyRegion(const std::string& region_name) {
    lookupRegion(region_name).destroy();
}

FamRegion FamSession::ensureCreateRegion(const fam::size_t region_size, const fam::perm_t region_perm,
                                         const std::string& region_name) {
    // Retry loop guards against TOCTOU
    constexpr int max_retries = 3;
    for (int attempt = 0; attempt <= max_retries; ++attempt) {
        try {
            return createRegion(region_size, region_perm, region_name);
        }
        catch (const AlreadyExists&) {
            try {
                destroyRegion(region_name);
            }
            catch (const NotFound&) {
                LOG_DEBUG_LIB(LibEcKit) << "Region '" << region_name
                                        << "' already existed but was concurrently destroyed by another "
                                           "process/thread; retrying create (attempt "
                                        << (attempt + 1) << " of " << max_retries << ")\n";
            }
        }
    }
    throw SeriousBug("ensureCreateRegion: failed after " + std::to_string(max_retries) + " retries for region '" +
                     region_name + "'");
}

FamProperty FamSession::stat(FamRegionDescriptor& region) {
    Fam_Stat info;

    auto fn_ptr = static_cast<void (openfam::fam::*)(FamRegionDescriptor*, Fam_Stat*)>(&openfam::fam::fam_stat);
    invokeFam(fn_ptr, &region, &info);

    return {info.size, info.perm, info.name, info.uid, info.gid};
}

//----------------------------------------------------------------------------------------------------------------------
//  OBJECT

FamObject FamSession::proxyObject(const std::uint64_t region, const std::uint64_t offset) {
    return {*this, region, offset};
}

FamObject FamSession::lookupObject(const std::string& region_name, const std::string& object_name) {
    ASSERT(isValidName(region_name));
    ASSERT(isValidName(object_name));

    auto* object = invokeFam(&openfam::fam::fam_lookup, object_name.c_str(), region_name.c_str());

    return {*this, object};
}

FamObject FamSession::allocateObject(FamRegionDescriptor& region, const fam::size_t object_size,
                                     const fam::perm_t object_perm, const std::string& object_name) {
    ASSERT(object_size > 0);

    auto allocate =
        static_cast<FamObjectDescriptor* (openfam::fam::*)(const char*, uint64_t, mode_t, FamRegionDescriptor*)>(
            &openfam::fam::fam_allocate);

    auto* object = invokeFam(allocate, object_name.c_str(), object_size, object_perm, &region);

    return {*this, object};
}

void FamSession::deallocateObject(FamObjectDescriptor& object) {
    invokeFam(&openfam::fam::fam_deallocate, &object);
}

void FamSession::deallocateObject(const std::string& region_name, const std::string& object_name) {
    lookupObject(region_name, object_name).deallocate();
}

FamObject FamSession::ensureAllocateObject(FamRegionDescriptor& region, const fam::size_t object_size,
                                           const fam::perm_t object_perm, const std::string& object_name) {
    // Retry loop guards against TOCTOU
    constexpr int max_retries = 3;
    for (int attempt = 0; attempt <= max_retries; ++attempt) {
        try {
            return allocateObject(region, object_size, object_perm, object_name);
        }
        catch (const AlreadyExists&) {
            try {
                deallocateObject(region.get_name(), object_name);
            }
            catch (const NotFound&) {
                LOG_DEBUG_LIB(LibEcKit) << "Object '" << object_name
                                        << "' already existed but was concurrently destroyed by another "
                                           "process/thread; retrying allocate (attempt "
                                        << (attempt + 1) << " of " << max_retries << ")\n";
            }
        }
    }
    throw SeriousBug("ensureAllocateObject: failed after " + std::to_string(max_retries) + " retries for object '" +
                     object_name + "'");
}

FamProperty FamSession::stat(FamObjectDescriptor& object) {
    Fam_Stat info;

    auto fn_ptr = static_cast<void (openfam::fam::*)(FamObjectDescriptor*, Fam_Stat*)>(&openfam::fam::fam_stat);
    invokeFam(fn_ptr, &object, &info);

    return {info.size, info.perm, info.name, info.uid, info.gid};
}

void FamSession::put(FamObjectDescriptor& object, const void* buffer, const fam::size_t offset,
                     const fam::size_t length) {
    ASSERT(buffer);
    ASSERT(length > 0);

    /// @note we have to remove "const" qualifier from buffer
    invokeFam(&openfam::fam::fam_put_blocking, const_cast<void*>(buffer), &object, offset, length);
}

void FamSession::get(FamObjectDescriptor& object, void* buffer, const fam::size_t offset, const fam::size_t length) {
    ASSERT(buffer);
    ASSERT(length > 0);

    invokeFam(&openfam::fam::fam_get_blocking, buffer, &object, offset, length);
}

//----------------------------------------------------------------------------------------------------------------------
// OBJECT - ATOMIC

template <typename T>
T FamSession::fetch(FamObjectDescriptor& /* object */, const fam::size_t /* offset */) {
    throw SeriousBug("This type is not specialized!", Here());
}

template <>
int32_t FamSession::fetch(FamObjectDescriptor& object, const fam::size_t offset) {
    return invokeFam(&openfam::fam::fam_fetch_int32, &object, offset);
}

template <>
int64_t FamSession::fetch(FamObjectDescriptor& object, const fam::size_t offset) {
    return invokeFam(&openfam::fam::fam_fetch_int64, &object, offset);
}

template <>
openfam::int128_t FamSession::fetch(FamObjectDescriptor& object, const fam::size_t offset) {
    return invokeFam(&openfam::fam::fam_fetch_int128, &object, offset);
}

template <>
uint32_t FamSession::fetch(FamObjectDescriptor& object, const fam::size_t offset) {
    return invokeFam(&openfam::fam::fam_fetch_uint32, &object, offset);
}

template <>
uint64_t FamSession::fetch(FamObjectDescriptor& object, const fam::size_t offset) {
    return invokeFam(&openfam::fam::fam_fetch_uint64, &object, offset);
}

template <>
float FamSession::fetch(FamObjectDescriptor& object, const fam::size_t offset) {
    return invokeFam(&openfam::fam::fam_fetch_float, &object, offset);
}

template <>
double FamSession::fetch(FamObjectDescriptor& object, const fam::size_t offset) {
    return invokeFam(&openfam::fam::fam_fetch_double, &object, offset);
}

template <typename T>
void FamSession::set(FamObjectDescriptor& object, const fam::size_t offset, const T value) {
    auto fptr = static_cast<void (openfam::fam::*)(FamObjectDescriptor*, fam::size_t, T)>(&openfam::fam::fam_set);
    invokeFam(fptr, &object, offset, value);
}

template <typename T>
void FamSession::add(FamObjectDescriptor& object, const fam::size_t offset, const T value) {
    auto fptr = static_cast<void (openfam::fam::*)(FamObjectDescriptor*, fam::size_t, T)>(&openfam::fam::fam_add);
    invokeFam(fptr, &object, offset, value);
}

template <typename T>
void FamSession::subtract(FamObjectDescriptor& object, const fam::size_t offset, const T value) {
    auto fptr = static_cast<void (openfam::fam::*)(FamObjectDescriptor*, fam::size_t, T)>(&openfam::fam::fam_subtract);
    invokeFam(fptr, &object, offset, value);
}

template <typename T>
T FamSession::swap(FamObjectDescriptor& object, const fam::size_t offset, const T value) {  // NOLINT
    auto fptr = static_cast<T (openfam::fam::*)(FamObjectDescriptor*, fam::size_t, T)>(&openfam::fam::fam_swap);
    return invokeFam(fptr, &object, offset, value);
}

template <typename T>
T FamSession::compareSwap(FamObjectDescriptor& object, const fam::size_t offset, const T old_value, const T new_value) {
    auto fptr =
        static_cast<T (openfam::fam::*)(FamObjectDescriptor*, fam::size_t, T, T)>(&openfam::fam::fam_compare_swap);
    return invokeFam(fptr, &object, offset, old_value, new_value);
}

//----------------------------------------------------------------------------------------------------------------------
// forward instantiations

template void FamSession::set(FamObjectDescriptor&, const fam::size_t, const int32_t);
template void FamSession::set(FamObjectDescriptor&, const fam::size_t, const int64_t);
template void FamSession::set(FamObjectDescriptor&, const fam::size_t, const openfam::int128_t);
template void FamSession::set(FamObjectDescriptor&, const fam::size_t, const uint32_t);
template void FamSession::set(FamObjectDescriptor&, const fam::size_t, const uint64_t);
template void FamSession::set(FamObjectDescriptor&, const fam::size_t, const float);
template void FamSession::set(FamObjectDescriptor&, const fam::size_t, const double);

template void FamSession::add(FamObjectDescriptor&, const fam::size_t, const int32_t);
template void FamSession::add(FamObjectDescriptor&, const fam::size_t, const int64_t);
template void FamSession::add(FamObjectDescriptor&, const fam::size_t, const uint32_t);
template void FamSession::add(FamObjectDescriptor&, const fam::size_t, const uint64_t);
template void FamSession::add(FamObjectDescriptor&, const fam::size_t, const float);
template void FamSession::add(FamObjectDescriptor&, const fam::size_t, const double);

template void FamSession::subtract(FamObjectDescriptor&, const fam::size_t, const int32_t);
template void FamSession::subtract(FamObjectDescriptor&, const fam::size_t, const int64_t);
template void FamSession::subtract(FamObjectDescriptor&, const fam::size_t, const uint32_t);
template void FamSession::subtract(FamObjectDescriptor&, const fam::size_t, const uint64_t);
template void FamSession::subtract(FamObjectDescriptor&, const fam::size_t, const float);
template void FamSession::subtract(FamObjectDescriptor&, const fam::size_t, const double);

template int32_t FamSession::swap(FamObjectDescriptor&, const fam::size_t, const int32_t);
template int64_t FamSession::swap(FamObjectDescriptor&, const fam::size_t, const int64_t);
template uint32_t FamSession::swap(FamObjectDescriptor&, const fam::size_t, const uint32_t);
template uint64_t FamSession::swap(FamObjectDescriptor&, const fam::size_t, const uint64_t);
template float FamSession::swap(FamObjectDescriptor&, const fam::size_t, const float);
template double FamSession::swap(FamObjectDescriptor&, const fam::size_t, const double);

template int32_t FamSession::compareSwap(FamObjectDescriptor&, const fam::size_t, const int32_t, const int32_t);
template int64_t FamSession::compareSwap(FamObjectDescriptor&, const fam::size_t, const int64_t, const int64_t);
template uint32_t FamSession::compareSwap(FamObjectDescriptor&, const fam::size_t, const uint32_t, const uint32_t);
template uint64_t FamSession::compareSwap(FamObjectDescriptor&, const fam::size_t, const uint64_t, const uint64_t);

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
