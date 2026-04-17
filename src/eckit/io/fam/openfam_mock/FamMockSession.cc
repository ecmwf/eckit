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

#include "FamMockSession.h"

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
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>

#include "fam/fam.h"
#include "fam/fam_exception.h"

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

template <typename... Args>
void debugLog(Args&&... args) {
    static const bool enabled = std::getenv("DEBUG") != nullptr;
    if (enabled) {
        ((std::cerr << "[openfam mock] ") << ... << std::forward<Args>(args)) << '\n';
    }
}

/// Byte offset of the data area from the start of the shared memory segment.
constexpr std::size_t k_data_offset = alignTo8(sizeof(State));

using SessionMap = std::map<std::string, std::unique_ptr<FamMockSession>>;

// Process-local mock session cache
std::pair<std::mutex, SessionMap>& sessionCache() {
    static std::pair<std::mutex, SessionMap> cache;
    return cache;
}

std::string getShmName(std::string name) {
    std::transform(name.begin(), name.end(), name.begin(),
                   [](unsigned char code) { return std::isalnum(code) ? static_cast<char>(code) : '_'; });
    // Limit length to be safe on all platforms (POSIX requires NAME_MAX support).
    if (name.size() > 200) {
        name = name.substr(0, 64) + "_" + std::to_string(std::hash<std::string>{}(name));
    }
    return "/eckit_fam_mock_" + (name.empty() ? "default" : name);
}

/// Gets shm size from $ECKIT_FAM_MOCK_SHM_SIZE (bytes) or defaults to g_default_shm_size.
std::size_t getShmSize() {
    const char* env = std::getenv("ECKIT_FAM_MOCK_SHM_SIZE");
    auto size       = env ? static_cast<std::size_t>(std::stol(env)) : g_default_shm_size;
    if (size <= k_data_offset) {
        throw std::runtime_error("ECKIT_FAM_MOCK_SHM_SIZE too small to hold State metadata");
    }
    return size;
}

//----------------------------------------------------------------------------------------------------------------------
// SHM lifecycle helper

/// Opens (or creates) the POSIX shm segment, truncates if creator, and maps it.
/// Populates @p handle with the fd and mapping. Returns true if this call created the segment.
bool openOrCreateShm(FamMockSession::ShmHandle& handle) {
    bool creator = false;
    auto shm_fd  = ::shm_open(handle.name.c_str(), O_CREAT | O_EXCL | O_RDWR, 0600);
    if (shm_fd >= 0) {
        creator = true;
        debugLog("Created new shared memory segment.");
    }
    else if (errno == EEXIST) {
        shm_fd = ::shm_open(handle.name.c_str(), O_RDWR, 0600);
        debugLog("Opened existing shared memory segment.");
    }

    if (shm_fd < 0) {
        throw std::system_error(errno, std::system_category(), "shm_open(" + handle.name + ")");
    }

    if (creator && ::ftruncate(shm_fd, static_cast<off_t>(handle.size)) != 0) {
        ::close(shm_fd);
        handle.unlink();
        throw std::system_error(errno, std::system_category(), "ftruncate");
    }

    auto* mapping = ::mmap(nullptr, handle.size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (mapping == MAP_FAILED) {
        ::close(shm_fd);
        if (creator) {
            handle.unlink();
        }
        throw std::system_error(errno, std::system_category(), "mmap");
    }

    handle.fd      = shm_fd;
    handle.mapping = mapping;
    return creator;
}

}  // namespace

//----------------------------------------------------------------------------------------------------------------------

void FamMockSession::ShmHandle::close() {
    if (mapping && mapping != MAP_FAILED) {
        ::munmap(mapping, size);
        mapping = nullptr;
    }
    if (fd >= 0) {
        ::close(fd);
        fd = -1;
    }
}

void FamMockSession::ShmHandle::unlink() const {
    ::shm_unlink(name.c_str());
}

//----------------------------------------------------------------------------------------------------------------------

FamMockSession& FamMockSession::instance(const std::string& name) {
    auto& [mutex, cache] = sessionCache();
    std::lock_guard lock(mutex);

    if (auto iter = cache.find(name); iter != cache.end()) {
        return *iter->second;
    }

    auto session = std::unique_ptr<FamMockSession>(new FamMockSession(name));
    auto& ref    = *session;
    cache[name]  = std::move(session);
    return ref;
}

//----------------------------------------------------------------------------------------------------------------------

void FamMockSession::mapFields() {
    state_ = static_cast<State*>(handle_.mapping);
    data_  = static_cast<std::uint8_t*>(handle_.mapping) + k_data_offset;
}

FamMockSession::FamMockSession(const std::string& name) : handle_{getShmName(name), getShmSize()} {
    debugLog("Opening shared memory: ", handle_.name, " (", (handle_.size / (1024 * 1024)), " MiB)");

    const auto data_capacity = handle_.size - k_data_offset;

    bool creator = openOrCreateShm(handle_);
    mapFields();

    // Stale/uninitialized segment (e.g., after crash or forced kill) — tear down and recreate.
    if (!creator && state_->initialized != k_init_magic) {
        debugLog("Detected stale/uninitialized segment. recreating...");
        handle_.close();
        handle_.unlink();

        creator = openOrCreateShm(handle_);
        mapFields();
    }

    if (creator) {
        debugLog("Zero-initializing shared memory and mutex.");
        std::memset(handle_.mapping, 0, handle_.size);

        // Initialize process-shared robust mutex.
        pthread_mutexattr_t attr;
        ::pthread_mutexattr_init(&attr);
        ::pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
        ::pthread_mutexattr_setrobust(&attr, PTHREAD_MUTEX_ROBUST);
        const int mrc = ::pthread_mutex_init(&state_->mutex, &attr);
        ::pthread_mutexattr_destroy(&attr);
        if (mrc != 0) {
            throw std::system_error(mrc, std::system_category(), "pthread_mutex_init");
        }
        debugLog("pthread_mutex_init returned ", mrc);

        state_->nextRegion   = 1;
        state_->dataCapacity = data_capacity;
        // Release fence to ensure preceding writes (mutex, nextRegion) are visible.
        __atomic_store_n(&state_->initialized, k_init_magic, __ATOMIC_RELEASE);
        debugLog("Shared memory initialization complete.");
    }
    else {
        // Wait for the creator to finish initialization.
        debugLog("Waiting for creator to finish initialization...");
        // Spin with microsecond sleeps — acceptable for test infrastructure.
        while (__atomic_load_n(&state_->initialized, __ATOMIC_ACQUIRE) != k_init_magic) {
            ::usleep(100);
        }
        debugLog("Shared memory initialization detected, proceeding.");
    }
}

FamMockSession::~FamMockSession() {
    handle_.close();
}

//----------------------------------------------------------------------------------------------------------------------

void FamMockSession::lock() {
    debugLog("Attempting to lock mutex...");
    const auto code = ::pthread_mutex_lock(&state_->mutex);
    debugLog("pthread_mutex_lock returned ", code);
    if (code == EOWNERDEAD) {
        // The previous owner died holding the mutex.
        // MUST NOT call lock()/LockGuard here — the mutex is already ours.
        debugLog("EOWNERDEAD detected, calling pthread_mutex_consistent and full reset.");
        ::pthread_mutex_consistent(&state_->mutex);
        resetUnlocked();
    }
    else if (code != 0) {
        throw std::system_error(code, std::system_category(), "pthread_mutex_lock");
    }
}

void FamMockSession::unlock() {
    debugLog("Unlocking mutex.");
    const int code = ::pthread_mutex_unlock(&state_->mutex);
    if (code != 0) {
        throw std::system_error(code, std::system_category(), "pthread_mutex_unlock");
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
