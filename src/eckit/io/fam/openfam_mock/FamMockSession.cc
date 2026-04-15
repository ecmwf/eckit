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

/// @file   FamMockSession.cc
/// @author Metin Cakircali
/// @date   Mar 2026

#include "eckit/io/fam/openfam_mock/FamMockSession.h"

#include <fcntl.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include <algorithm>
#include <cctype>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <map>
#include <mutex>
#include <string>
#include <string_view>

#include "fam/fam.h"
#include "fam/fam_exception.h"

#include "eckit/config/LibEcKit.h"
#include "eckit/config/Resource.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/log/Log.h"

using eckit::LibEcKit;

namespace openfam::mock {

//----------------------------------------------------------------------------------------------------------------------
// HELPERS

namespace {

/// Magic value written to `State::initialized` once the creator finishes setup.
constexpr std::uint32_t k_init_magic = 0xFA00CAFE;

/// Round @p n up to the next multiple of 8.
template <typename T>
constexpr T alignTo8(T n) {
    return (n + T{7}) & ~T{7};
}

/// Byte offset of the data area from the start of the shared memory segment.
constexpr std::size_t k_data_offset = alignTo8(sizeof(State));

using SessionMap = std::map<std::string, FamMockSession*>;

// Process-local mock session cache
std::pair<std::mutex, SessionMap>& sessionCache() {
    static std::pair<std::mutex, SessionMap> cache;
    return cache;
}

std::string generateShmName(std::string name) {
    std::transform(name.begin(), name.end(), name.begin(),
                   [](unsigned char ch) { return std::isalnum(ch) ? static_cast<char>(ch) : '_'; });
    // Limit length to be safe on all platforms (POSIX requires NAME_MAX support).
    if (name.size() > 200) {
        name = name.substr(0, 64) + "_" + std::to_string(std::hash<std::string>{}(name));
    }
    return "/eckit_fam_mock_" + (name.empty() ? "default" : name);
}

/// Reads total shm size from $ECKIT_FAM_MOCK_SHM_SIZE (bytes), defaulting to g_default_shm_size.
std::size_t resolvedShmSize() {
    auto size = static_cast<std::size_t>(
        eckit::Resource<long>("famMockShmSize;$ECKIT_FAM_MOCK_SHM_SIZE", static_cast<long>(g_default_shm_size)));
    ASSERT_MSG(size > k_data_offset, "ECKIT_FAM_MOCK_SHM_SIZE too small to hold State metadata");
    return size;
}

//----------------------------------------------------------------------------------------------------------------------
// SHM lifecycle helper

/// Opens (or creates) the POSIX shm segment, truncates if creator, and maps it.
/// Populates @p handle with the fd and mapping. Returns true if this call created the segment.
bool openOrCreateShm(FamMockSession::ShmHandle& handle) {
    bool creator = false;
    auto shmFd   = ::shm_open(handle.name(), O_CREAT | O_EXCL | O_RDWR, 0666);
    if (shmFd >= 0) {
        creator = true;
        LOG_DEBUG_LIB(LibEcKit) << "Created new shared memory segment.\n";
    }
    else if (errno == EEXIST) {
        shmFd = ::shm_open(handle.name(), O_RDWR, 0666);
        LOG_DEBUG_LIB(LibEcKit) << "Opened existing shared memory segment.\n";
    }

    if (shmFd < 0) {
        throw eckit::FailedSystemCall("shm_open(" + handle.shmName_ + ")", Here(), errno);
    }

    if (creator && ::ftruncate(shmFd, static_cast<off_t>(handle.shmSize_)) != 0) {
        ::close(shmFd);
        handle.unlink();
        throw eckit::FailedSystemCall("ftruncate", Here(), errno);
    }

    auto* mapping = ::mmap(nullptr, handle.shmSize_, PROT_READ | PROT_WRITE, MAP_SHARED, shmFd, 0);
    if (mapping == MAP_FAILED) {
        ::close(shmFd);
        if (creator) {
            handle.unlink();
        }
        throw eckit::FailedSystemCall("mmap", Here(), errno);
    }

    handle.fd_      = shmFd;
    handle.mapping_ = mapping;
    return creator;
}

}  // namespace

//----------------------------------------------------------------------------------------------------------------------

void FamMockSession::ShmHandle::close() {
    if (mapping_ && mapping_ != MAP_FAILED) {
        ::munmap(mapping_, shmSize_);
        mapping_ = nullptr;
    }
    if (fd_ >= 0) {
        ::close(fd_);
        fd_ = -1;
    }
}

void FamMockSession::ShmHandle::unlink() {
    ::shm_unlink(name());
}

//----------------------------------------------------------------------------------------------------------------------

FamMockSession& FamMockSession::instance(const std::string& name) {
    auto& [mutex, cache] = sessionCache();
    std::lock_guard lock(mutex);

    if (auto iter = cache.find(name); iter != cache.end()) {
        return *iter->second;
    }

    /// @note Intentionally leaking to avoid static-destruction-order issues
    auto* session = new FamMockSession(name);
    cache[name]   = session;
    return *session;
}

//----------------------------------------------------------------------------------------------------------------------

void FamMockSession::mapFields() {
    state_ = static_cast<State*>(handle_.mapping_);
    data_  = static_cast<std::uint8_t*>(handle_.mapping_) + k_data_offset;
}

FamMockSession::FamMockSession(const std::string& name) : handle_{generateShmName(name), resolvedShmSize()} {
    LOG_DEBUG_LIB(LibEcKit) << "Opening shared memory: " << handle_.shmName_ << " ("
                            << (handle_.shmSize_ / (1024 * 1024)) << " MiB)\n";

    const auto dataCapacity = handle_.shmSize_ - k_data_offset;

    bool creator = openOrCreateShm(handle_);
    mapFields();

    // Stale/uninitialized segment (e.g., after crash or forced kill) — tear down and recreate.
    if (!creator && state_->initialized != k_init_magic) {
        LOG_DEBUG_LIB(LibEcKit) << "Detected stale/uninitialized segment. recreating...\n";
        handle_.close();
        handle_.unlink();

        creator = openOrCreateShm(handle_);
        mapFields();
    }

    if (creator) {
        LOG_DEBUG_LIB(LibEcKit) << "Zero-initializing shared memory and mutex.\n";
        std::memset(handle_.mapping_, 0, handle_.shmSize_);

        // Initialize process-shared robust mutex.
        pthread_mutexattr_t attr;
        ::pthread_mutexattr_init(&attr);
        ::pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
        ::pthread_mutexattr_setrobust(&attr, PTHREAD_MUTEX_ROBUST);
        const int mrc = ::pthread_mutex_init(&state_->mutex, &attr);
        ::pthread_mutexattr_destroy(&attr);
        if (mrc != 0) {
            throw eckit::FailedSystemCall("pthread_mutex_init", Here(), mrc);
        }
        LOG_DEBUG_LIB(LibEcKit) << "pthread_mutex_init returned " << mrc << '\n';

        state_->nextRegion   = 1;
        state_->dataCapacity = dataCapacity;
        // Release fence to ensure preceding writes (mutex, nextRegion) are visible.
        __atomic_store_n(&state_->initialized, k_init_magic, __ATOMIC_RELEASE);
        LOG_DEBUG_LIB(LibEcKit) << "Shared memory initialization complete.\n";
    }
    else {
        // Wait for the creator to finish initialization.
        LOG_DEBUG_LIB(LibEcKit) << "Waiting for creator to finish initialization...\n";
        // Spin with microsecond sleeps — acceptable for test infrastructure.
        while (__atomic_load_n(&state_->initialized, __ATOMIC_ACQUIRE) != k_init_magic) {
            ::usleep(100);
        }
        LOG_DEBUG_LIB(LibEcKit) << "Shared memory initialization detected, proceeding.\n";
    }
}

FamMockSession::~FamMockSession() {
    handle_.close();
}

//----------------------------------------------------------------------------------------------------------------------

void FamMockSession::lock() {
    LOG_DEBUG_LIB(LibEcKit) << "Attempting to lock mutex...\n";
    const auto code = ::pthread_mutex_lock(&state_->mutex);
    LOG_DEBUG_LIB(LibEcKit) << "pthread_mutex_lock returned " << code << '\n';
    if (code == EOWNERDEAD) {
        // The previous owner died holding the mutex.
        // MUST NOT call lock()/LockGuard here — the mutex is already ours.
        LOG_DEBUG_LIB(LibEcKit) << "EOWNERDEAD detected, calling pthread_mutex_consistent and full reset.\n";
        ::pthread_mutex_consistent(&state_->mutex);
        resetUnlocked();
    }
    else if (code != 0) {
        throw eckit::FailedSystemCall("pthread_mutex_lock", Here(), code);
    }
}

void FamMockSession::unlock() {
    LOG_DEBUG_LIB(LibEcKit) << "Unlocking mutex.\n";
    const int code = ::pthread_mutex_unlock(&state_->mutex);
    if (code != 0) {
        throw eckit::FailedSystemCall("pthread_mutex_unlock", Here(), code);
    }
}

//----------------------------------------------------------------------------------------------------------------------

void FamMockSession::resetUnlocked() {
    // Must be called while the mutex is already held (or during constructor setup).
    state_->nextRegion = 1;
    state_->dataUsed   = 0;
    for (auto& region : state_->regions) {
        region = Region{};
    }
    std::memset(data_, 0, state_->dataCapacity);
}

void FamMockSession::reset() {
    std::lock_guard guard(*this);
    resetUnlocked();
}

//----------------------------------------------------------------------------------------------------------------------
// Region helpers

Region* FamMockSession::allocateRegionSlot() {
    for (auto& region : state_->regions) {
        if (!region.active) {
            return &region;
        }
    }
    return nullptr;
}

Region* FamMockSession::findRegionByName(const char* name) {
    for (auto& region : state_->regions) {
        if (region.active && std::string_view{region.name} == name) {
            return &region;
        }
    }
    return nullptr;
}

Region* FamMockSession::findRegionById(std::uint64_t regionId) {
    for (auto& region : state_->regions) {
        if (region.active && region.id == regionId) {
            return &region;
        }
    }
    return nullptr;
}

Region& FamMockSession::findRegion(Fam_Region_Descriptor* desc) {
    if (auto* region = findRegionById(desc->get_global_descriptor().regionId)) {
        return *region;
    }
    throw Fam_Exception("Region not found", FAM_ERR_NOTFOUND);
}

void FamMockSession::freeRegion(Region& region) {
    region = Region{};
}

//----------------------------------------------------------------------------------------------------------------------
// Object helpers

Object* FamMockSession::findObjectByOffset(Region& region, std::uint64_t offset) {
    for (auto& obj : region.objects) {
        if (obj.active && obj.offset == offset) {
            return &obj;
        }
    }
    return nullptr;
}

Object* FamMockSession::findObjectByName(Region& region, const char* name) {
    for (auto& obj : region.objects) {
        if (obj.active && obj.name[0] != '\0' && std::strcmp(obj.name, name) == 0) {
            return &obj;
        }
    }
    return nullptr;
}

Object* FamMockSession::allocateObjectSlot(Region& region) {
    for (auto& obj : region.objects) {
        if (!obj.active) {
            return &obj;
        }
    }
    return nullptr;
}

Object& FamMockSession::findObject(Fam_Descriptor* desc) {
    const auto [regionId, offset] = desc->get_global_descriptor();

    auto* region = findRegionById(regionId);
    if (!region) {
        throw Fam_Exception("Region not found", FAM_ERR_NOTFOUND);
    }

    auto* obj = findObjectByOffset(*region, offset);
    if (!obj) {
        throw Fam_Exception("Object not found", FAM_ERR_NOTFOUND);
    }
    return *obj;
}

void FamMockSession::freeObject(Object& obj) {
    obj = Object{};
}

//----------------------------------------------------------------------------------------------------------------------
// Data area

/// @note freed space is never reclaimed
/// Resets clear the whole data area at once.
std::uint64_t FamMockSession::allocateData(std::uint64_t size) {
    const auto aligned = alignTo8(size);

    if (state_->dataUsed + aligned > state_->dataCapacity) {
        throw Fam_Exception("Mock FAM data area exhausted", FAM_ERR_NO_SPACE);
    }

    const auto offset = state_->dataUsed;
    state_->dataUsed += aligned;
    return offset;
}

std::uint8_t* FamMockSession::objectData(const Object& obj) {
    return data_ + obj.dataOffset;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace openfam::mock
