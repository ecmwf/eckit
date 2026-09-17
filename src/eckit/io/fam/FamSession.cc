// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/*
 * This software was developed as part of the Horizon Europe programme funded
 * project OpenCUBE (Grant agreement: 101092984) horizon-opencube.eu
 */

#include "FamSession.h"

#include <sys/types.h>  // mode_t

#include <algorithm>
#include <cctype>  // isspace isprint
#include <chrono>
#include <cstdint>
#include <cstring>
#include <memory>
#include <ostream>
#include <string>
#include <string_view>
#include <utility>

#include "eckit/io/fam/FamTypes.h"
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

// OpenFAM exceptions carry no object identity, so record what the in-flight
// call targets and report it when the call fails.
struct FamTarget {
    const char* op{nullptr};
    std::uint64_t region{0};
    std::uint64_t object{0};
    std::uint64_t offset{0};
};

thread_local FamTarget g_target{};

class FamTargetScope {
public:

    FamTargetScope(const char* op, FamObjectDescriptor& object, const fam::size_t offset) {
        const auto descriptor = object.get_global_descriptor();
        g_target              = {op, descriptor.regionId, descriptor.offset, offset};
    }

    FamTargetScope(const FamTargetScope&)            = delete;
    FamTargetScope& operator=(const FamTargetScope&) = delete;
    FamTargetScope(FamTargetScope&&)                 = delete;
    FamTargetScope& operator=(FamTargetScope&&)      = delete;

    ~FamTargetScope() { g_target = {}; }
};

void log_atomic(const char* operation, FamObjectDescriptor& object, const fam::size_t offset,
                const std::chrono::steady_clock::time_point start, const bool completed) {
    const auto descriptor = object.get_global_descriptor();
    const auto elapsed =
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start);
    LOG_DEBUG_LIB(LibEcKit) << "FAM atomic " << operation << (completed ? " completed" : " started")
                            << " region=" << descriptor.regionId << " object=" << descriptor.offset
                            << " offset=" << offset << " elapsed_ms=" << elapsed.count() << '\n';
}

// A NotFound from OpenFAM does not say whether the item is absent from metadata
// or whether the lookup used the wrong key. Re-stat the same descriptor to tell
// them apart.
void probe_not_found(openfam::fam& fam) {
    if (g_target.op == nullptr) {
        LOG_DEBUG_LIB(LibEcKit) << "FAM not-found re-probe skipped: no object target recorded\n";
        return;
    }

    Fam_Global_Descriptor global{g_target.region, g_target.object};
    FamObjectDescriptor probe{global};
    try {
        Fam_Stat info;
        fam.fam_stat(&probe, &info);
        LOG_DEBUG_LIB(LibEcKit) << "FAM not-found re-probe SUCCEEDED (item exists; failing lookup used "
                                   "a different key)"
                                << " region=" << g_target.region << " object=" << g_target.object
                                << " size=" << info.size << '\n';
    }
    catch (openfam::Fam_Exception& error) {
        LOG_DEBUG_LIB(LibEcKit) << "FAM not-found re-probe FAILED (item absent from metadata for this "
                                   "region)"
                                << " region=" << g_target.region << " object=" << g_target.object
                                << " code=" << error.fam_error() << " message=" << error.fam_error_msg() << '\n';
    }
}

std::unique_ptr<openfam::fam> initializeFamSession(const std::string& name, const net::Endpoint& endpoint) {
    LOG_DEBUG_LIB(LibEcKit) << "Initializing FAM session: " << name << " with endpoint " << endpoint << '\n';

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

    const auto start = std::chrono::steady_clock::now();
    try {
        return (fam_.get()->*std::forward<Func>(fn_ptr))(std::forward<Args>(args)...);
    }
    catch (openfam::Fam_Exception& e) {
        const auto code = e.fam_error();
        const auto elapsed =
            std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start);
        LOG_DEBUG_LIB(LibEcKit) << "FAM operation failed: session=" << name_ << " endpoint=" << endpoint_
                                << " code=" << code << " message=" << e.fam_error_msg()
                                << " elapsed_ms=" << elapsed.count() << " op=" << (g_target.op ? g_target.op : "?")
                                << " region=" << g_target.region << " object=" << g_target.object
                                << " offset=" << g_target.offset << '\n';
        if (code == openfam::Fam_Error::FAM_ERR_NOTFOUND) {
            probe_not_found(*fam_);
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

    LOG_DEBUG_LIB(LibEcKit) << "FAM region lookup name=" << region_name
                            << " regionId=" << region->get_global_descriptor().regionId << '\n';

    return {*this, region};
}

FamRegion FamSession::createRegion(const fam::size_t region_size, const fam::perm_t region_perm,
                                   const std::string& region_name) {
    ASSERT(region_size > 0);
    ASSERT(isValidName(region_name));

    auto* region = invokeFam(&openfam::fam::fam_create_region, region_name.c_str(), region_size, region_perm, nullptr);

    LOG_DEBUG_LIB(LibEcKit) << "FAM region create name=" << region_name
                            << " regionId=" << region->get_global_descriptor().regionId << '\n';

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
    // Retry loop guards against TOCTOU (Time Of Check To Time Of Use)
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
    LOG_DEBUG_LIB(LibEcKit) << "FAM object proxy region=" << region << " object=" << offset << '\n';
    return {*this, region, offset};
}

FamObject FamSession::lookupObject(const std::string& region_name, const std::string& object_name) {
    ASSERT(isValidName(region_name));
    ASSERT(isValidName(object_name));

    auto* object = invokeFam(&openfam::fam::fam_lookup, object_name.c_str(), region_name.c_str());

    LOG_DEBUG_LIB(LibEcKit) << "FAM object lookup name=" << object_name << " region=" << region_name
                            << " regionId=" << object->get_global_descriptor().regionId
                            << " object=" << object->get_global_descriptor().offset << '\n';

    return {*this, object};
}

FamObject FamSession::allocateObject(FamRegionDescriptor& region, const fam::size_t object_size,
                                     const fam::perm_t object_perm, const std::string& object_name) {
    ASSERT(object_size > 0);

    auto allocate =
        static_cast<FamObjectDescriptor* (openfam::fam::*)(const char*, uint64_t, mode_t, FamRegionDescriptor*)>(
            &openfam::fam::fam_allocate);

    auto* object = invokeFam(allocate, object_name.c_str(), object_size, object_perm, &region);

    // Region id is the metadata lookup key for later atomics; flag any drift.
    const auto requested_region = region.get_global_descriptor().regionId;
    const auto returned_region  = object->get_global_descriptor().regionId;
    LOG_DEBUG_LIB(LibEcKit) << "FAM object allocate name=" << object_name << " requestedRegion=" << requested_region
                            << " returnedRegion=" << returned_region
                            << " object=" << object->get_global_descriptor().offset
                            << (requested_region == returned_region ? "" : " REGION-ID-MISMATCH") << '\n';

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

    const FamTargetScope target{"stat", object, 0};
    auto fn_ptr = static_cast<void (openfam::fam::*)(FamObjectDescriptor*, Fam_Stat*)>(&openfam::fam::fam_stat);
    invokeFam(fn_ptr, &object, &info);

    return {info.size, info.perm, info.name, info.uid, info.gid};
}

void FamSession::put(FamObjectDescriptor& object, const void* buffer, const fam::size_t offset,
                     const fam::size_t length) {
    ASSERT(buffer);
    ASSERT(length > 0);

    /// @note we have to remove "const" qualifier from buffer
    const FamTargetScope target{"put", object, offset};
    invokeFam(&openfam::fam::fam_put_blocking, const_cast<void*>(buffer), &object, offset, length);
}

void FamSession::get(FamObjectDescriptor& object, void* buffer, const fam::size_t offset, const fam::size_t length) {
    ASSERT(buffer);
    ASSERT(length > 0);

    const FamTargetScope target{"get", object, offset};
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
    const FamTargetScope target{"fetch_int32", object, offset};
    return invokeFam(&openfam::fam::fam_fetch_int32, &object, offset);
}

template <>
int64_t FamSession::fetch(FamObjectDescriptor& object, const fam::size_t offset) {
    const FamTargetScope target{"fetch_int64", object, offset};
    return invokeFam(&openfam::fam::fam_fetch_int64, &object, offset);
}

template <>
openfam::int128_t FamSession::fetch(FamObjectDescriptor& object, const fam::size_t offset) {
    const FamTargetScope target{"fetch_int128", object, offset};
    return invokeFam(&openfam::fam::fam_fetch_int128, &object, offset);
}

template <>
uint32_t FamSession::fetch(FamObjectDescriptor& object, const fam::size_t offset) {
    const FamTargetScope target{"fetch_uint32", object, offset};
    return invokeFam(&openfam::fam::fam_fetch_uint32, &object, offset);
}

template <>
uint64_t FamSession::fetch(FamObjectDescriptor& object, const fam::size_t offset) {
    const FamTargetScope target{"fetch_uint64", object, offset};
    return invokeFam(&openfam::fam::fam_fetch_uint64, &object, offset);
}

template <>
float FamSession::fetch(FamObjectDescriptor& object, const fam::size_t offset) {
    const FamTargetScope target{"fetch_float", object, offset};
    return invokeFam(&openfam::fam::fam_fetch_float, &object, offset);
}

template <>
double FamSession::fetch(FamObjectDescriptor& object, const fam::size_t offset) {
    const FamTargetScope target{"fetch_double", object, offset};
    return invokeFam(&openfam::fam::fam_fetch_double, &object, offset);
}

template <typename T>
void FamSession::set(FamObjectDescriptor& object, const fam::size_t offset, const T value) {
    auto fptr = static_cast<void (openfam::fam::*)(FamObjectDescriptor*, fam::size_t, T)>(&openfam::fam::fam_set);
    const FamTargetScope target{"set", object, offset};
    invokeFam(fptr, &object, offset, value);
}

template <typename T>
void FamSession::add(FamObjectDescriptor& object, const fam::size_t offset, const T value) {
    auto fptr = static_cast<void (openfam::fam::*)(FamObjectDescriptor*, fam::size_t, T)>(&openfam::fam::fam_add);
    const FamTargetScope target{"add", object, offset};
    invokeFam(fptr, &object, offset, value);
}

template <typename T>
T FamSession::fetchAdd(FamObjectDescriptor& object, const fam::size_t offset, const T value) {
    const auto start = std::chrono::steady_clock::now();
    log_atomic("fetch_add", object, offset, start, false);
    auto fptr = static_cast<T (openfam::fam::*)(FamObjectDescriptor*, fam::size_t, T)>(&openfam::fam::fam_fetch_add);
    const FamTargetScope target{"fetch_add", object, offset};
    auto result = invokeFam(fptr, &object, offset, value);
    log_atomic("fetch_add", object, offset, start, true);
    return result;
}

template <typename T>
void FamSession::subtract(FamObjectDescriptor& object, const fam::size_t offset, const T value) {
    auto fptr = static_cast<void (openfam::fam::*)(FamObjectDescriptor*, fam::size_t, T)>(&openfam::fam::fam_subtract);
    const FamTargetScope target{"subtract", object, offset};
    invokeFam(fptr, &object, offset, value);
}

template <typename T>
T FamSession::swap(FamObjectDescriptor& object, const fam::size_t offset,
                   const T value) {  // NOLINT
    auto fptr = static_cast<T (openfam::fam::*)(FamObjectDescriptor*, fam::size_t, T)>(&openfam::fam::fam_swap);
    const FamTargetScope target{"swap", object, offset};
    return invokeFam(fptr, &object, offset, value);
}

template <typename T>
T FamSession::compareSwap(FamObjectDescriptor& object, const fam::size_t offset, const T old_value, const T new_value) {
    const auto start = std::chrono::steady_clock::now();
    log_atomic("compare_swap", object, offset, start, false);
    auto fptr =
        static_cast<T (openfam::fam::*)(FamObjectDescriptor*, fam::size_t, T, T)>(&openfam::fam::fam_compare_swap);
    const FamTargetScope target{"compare_swap", object, offset};
    auto result = invokeFam(fptr, &object, offset, old_value, new_value);
    log_atomic("compare_swap", object, offset, start, true);
    return result;
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

template int32_t FamSession::fetchAdd(FamObjectDescriptor&, const fam::size_t, const int32_t);
template int64_t FamSession::fetchAdd(FamObjectDescriptor&, const fam::size_t, const int64_t);
template uint32_t FamSession::fetchAdd(FamObjectDescriptor&, const fam::size_t, const uint32_t);
template uint64_t FamSession::fetchAdd(FamObjectDescriptor&, const fam::size_t, const uint64_t);
template float FamSession::fetchAdd(FamObjectDescriptor&, const fam::size_t, const float);
template double FamSession::fetchAdd(FamObjectDescriptor&, const fam::size_t, const double);

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
