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

#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <iterator>
#include <map>
#include <mutex>
#include <string>
#include <string_view>

#include "fam/fam.h"
#include "fam/fam_exception.h"

#include "eckit/config/LibEcKit.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/log/Log.h"

using eckit::LibEcKit;

namespace openfam::mock {

//----------------------------------------------------------------------------------------------------------------------
// HELPERS

namespace {

/// Magic value written to `State::initialized` once the creator finishes setup.
constexpr std::uint32_t k_init_magic = 0xFA00CAFE;

using SessionMap = std::map<std::string, FamMockSession*>;

// process-local mock Session cache ()
std::pair<std::mutex, SessionMap>& sessionCache() {
    static std::pair<std::mutex, SessionMap> cache;
    return cache;
}

/// The name must start with '/' and contain no further slashes.
std::string generateShmName(std::string name) {
    for (char& chr : name) {
        chr = (std::isalnum(static_cast<unsigned char>(chr)) != 0) ? chr : '_';
    }
    // Limit length to be safe on all platforms (POSIX requires NAME_MAX support).
    if (name.size() > 200) {
        const auto hash = std::hash<std::string>{}(name);
        name            = name.substr(0, 64) + "_" + std::to_string(hash);
    }
    return "/eckit_fam_mock_" + (name.empty() ? "default" : name);
}

//----------------------------------------------------------------------------------------------------------------------

/// Byte offset of the data area from the start of the shared memory segment.
std::size_t dataOffset() {
    return (sizeof(State) + 7U) & ~std::size_t{7U};
}

/// Total capacity of the data area in bytes.
std::size_t dataCapacity() {
    return g_shm_total_size - dataOffset();
}

//----------------------------------------------------------------------------------------------------------------------
// SHM lifecycle helper

struct ShmMapping {
    int fd{-1};
    void* mapping{nullptr};
    bool creator{false};
};

/// Opens (or creates) the POSIX shm segment, truncates if creator, and maps it.
ShmMapping openOrCreateShm(const std::string& name) {
    bool creator = false;
    auto shmFd   = ::shm_open(name.c_str(), O_CREAT | O_EXCL | O_RDWR, 0666);
    if (shmFd >= 0) {
        creator = true;
        LOG_DEBUG_LIB(LibEcKit) << "Created new shared memory segment.\n";
    }
    else if (errno == EEXIST) {
        shmFd = ::shm_open(name.c_str(), O_RDWR, 0666);
        LOG_DEBUG_LIB(LibEcKit) << "Opened existing shared memory segment.\n";
    }

    if (shmFd < 0) {
        throw eckit::FailedSystemCall("shm_open(" + name + ")", Here(), errno);
    }

    if (creator && ::ftruncate(shmFd, static_cast<off_t>(g_shm_total_size)) != 0) {
        ::close(shmFd);
        ::shm_unlink(name.c_str());
        throw eckit::FailedSystemCall("ftruncate", Here(), errno);
    }

    auto* mapping = ::mmap(nullptr, g_shm_total_size, PROT_READ | PROT_WRITE, MAP_SHARED, shmFd, 0);
    if (mapping == MAP_FAILED) {
        ::close(shmFd);
        if (creator) {
            ::shm_unlink(name.c_str());
        }
        throw eckit::FailedSystemCall("mmap", Here(), errno);
    }

    return {shmFd, mapping, creator};
}

}  // namespace

//----------------------------------------------------------------------------------------------------------------------

auto FamMockSession::instance(const std::string& name) -> FamMockSession& {
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

FamMockSession::FamMockSession(const std::string& name) : shmName_{generateShmName(name)} {
    LOG_DEBUG_LIB(LibEcKit) << "Opening shared memory: " << shmName_ << '\n';

    const auto shm = openOrCreateShm(shmName_);

    bool creator = shm.creator;
    fd_          = shm.fd;
    mapping_     = shm.mapping;
    state_       = static_cast<State*>(mapping_);
    data_        = std::next(static_cast<std::uint8_t*>(mapping_), static_cast<std::ptrdiff_t>(dataOffset()));

    // Check stale/uninitialized segment (e.g., after crash or forced kill)
    if (!creator && state_->initialized != k_init_magic) {
        LOG_DEBUG_LIB(LibEcKit) << "Detected stale/uninitialized segment. recreating...\n";
        ::munmap(mapping_, g_shm_total_size);
        ::close(fd_);
        ::shm_unlink(shmName_.c_str());

        const auto shmNew = openOrCreateShm(shmName_);

        fd_      = shmNew.fd;
        mapping_ = shmNew.mapping;
        state_   = static_cast<State*>(mapping_);
        data_    = std::next(static_cast<std::uint8_t*>(mapping_), static_cast<std::ptrdiff_t>(dataOffset()));
        creator  = true;
    }

    if (creator) {
        LOG_DEBUG_LIB(LibEcKit) << "Zero-initializing shared memory and mutex.\n";
        std::memset(mapping_, 0, g_shm_total_size);

        // Initialize process-shared robust mutex.
        pthread_mutexattr_t attr;
        ::pthread_mutexattr_init(&attr);
        ::pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
        ::pthread_mutexattr_setrobust(&attr, PTHREAD_MUTEX_ROBUST);
        const int mrc = ::pthread_mutex_init(&state_->mutex, &attr);
        ::pthread_mutexattr_destroy(&attr);
        LOG_DEBUG_LIB(LibEcKit) << "pthread_mutex_init returned " << mrc << '\n';

        state_->nextRegion  = 1;
        state_->dataUsed    = 0;
        state_->initialized = k_init_magic;
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
    if (mapping_ && mapping_ != MAP_FAILED) {
        ::munmap(mapping_, g_shm_total_size);
    }
    if (fd_ >= 0) {
        ::close(fd_);
    }
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
    ::pthread_mutex_unlock(&state_->mutex);
}

//----------------------------------------------------------------------------------------------------------------------

void FamMockSession::resetUnlocked() {
    // Must be called while the mutex is already held (or during constructor setup).
    for (auto& region : state_->regions) {
        if (region.active) {
            for (auto& obj : region.objects) {
                obj.active = false;
            }
            region.active = false;
        }
    }
    state_->nextRegion = 1;
    state_->dataUsed   = 0;
    std::memset(data_, 0, dataCapacity());
}

void FamMockSession::reset() {
    LockGuard guard(*this);
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
        if (region.active && std::string_view{std::data(region.name)} == name) {
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
    const auto regionId = desc->get_global_descriptor().regionId;
    if (auto* region = findRegionById(regionId)) {
        return *region;
    }
    throw Fam_Exception("Region not found", FAM_ERR_NOTFOUND);
}

void FamMockSession::freeRegion(Region& region) {
    for (auto& obj : region.objects) {
        obj.active = false;
    }
    region.active     = false;
    region.name[0]    = '\0';
    region.nextOffset = 8;
}

//----------------------------------------------------------------------------------------------------------------------
// Object helpers

auto FamMockSession::findObjectByOffset(Region& region, std::uint64_t offset) -> Object* {
    for (auto& obj : region.objects) {
        if (obj.active && obj.offset == offset) {
            return &obj;
        }
    }
    return nullptr;
}

auto FamMockSession::findObjectByName(Region& region, const char* name) -> Object* {
    for (auto& obj : region.objects) {
        if (obj.active && obj.name[0] != '\0' && std::strcmp(obj.name, name) == 0) {
            return &obj;
        }
    }
    return nullptr;
}

auto FamMockSession::allocateObjectSlot(Region& region) -> Object* {
    for (auto& obj : region.objects) {
        if (!obj.active) {
            return &obj;
        }
    }
    return nullptr;
}

auto FamMockSession::findObject(Fam_Descriptor* desc) -> Object& {
    const auto regionId = desc->get_global_descriptor().regionId;
    const auto offset   = desc->get_global_descriptor().offset;

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
    obj.active  = false;
    obj.name[0] = '\0';
}

//----------------------------------------------------------------------------------------------------------------------
// Data area

auto FamMockSession::allocateData(std::uint64_t size) -> std::uint64_t {
    const auto capacity = dataCapacity();
    const auto aligned  = (size + 7U) & ~std::uint64_t{7U};

    if (state_->dataUsed + aligned > capacity) {
        throw Fam_Exception("Mock FAM data area exhausted", FAM_ERR_NO_SPACE);
    }

    const auto offset = state_->dataUsed;
    state_->dataUsed += aligned;
    return offset;
}

auto FamMockSession::objectData(const Object& obj) -> std::uint8_t* {
    return data_ + obj.dataOffset;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace openfam::mock
