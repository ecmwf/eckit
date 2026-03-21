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

/// @file   FamMockSession.h
/// @author Metin Cakircali
/// @date   Mar 2026

/// @brief  Implements multi-process FAM mock.
///
/// Uses POSIX shared-memory to simulate FAM (Fabric-Attached Memory).
///
/// All mock states live in POSIX shared memory.
//  Layout:
///   [State] (mutex, counters, region table)
///     └ Region[g_max_regions]
///        └ Object[g_max_objs_per_region]
///   [Data] — raw bytes

#pragma once

#include <pthread.h>
#include <sys/types.h>  // mode_t

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>

//----------------------------------------------------------------------------------------------------------------------

namespace openfam {

class Fam_Descriptor;
class Fam_Region_Descriptor;

namespace mock {

//----------------------------------------------------------------------------------------------------------------------
/// Capacity (@note: don't just change values)

constexpr std::size_t g_max_regions         = 64;
constexpr std::size_t g_max_objs_per_region = 4096;
constexpr std::size_t g_max_name_len        = 256;
constexpr std::size_t g_shm_total_size      = 256 * 1024 * 1024;  // 256 MiB

//----------------------------------------------------------------------------------------------------------------------

/// @note POD no virtual, no std::string
struct Object {
    bool active{false};

    char name[g_max_name_len]{};

    std::uint64_t offset{0};

    std::uint64_t size{0};
    mode_t perm{0};
    std::uint32_t uid{0};
    std::uint32_t gid{0};

    std::uint64_t dataOffset{0};
};

/// @note POD no virtual, no std::string
struct Region {
    bool active{false};

    char name[g_max_name_len]{};

    std::uint64_t id{0};
    std::uint64_t size{0};
    mode_t perm{0};

    /// Starts at 8 as offset 0 is used as a null value
    std::uint64_t nextOffset{8};

    Object objects[g_max_objs_per_region];
};

/// @note POD no virtual, no std::string
struct State {
    pthread_mutex_t mutex;

    /// marker for initialization completion (set to `k_init_magic`)
    std::uint32_t initialized;

    /// next region ID (0 is invalid)
    std::uint64_t nextRegion;

    /// bytes used in data area (starts at 0)
    std::uint64_t dataUsed;

    Region regions[g_max_regions];

    // The data area begins here (8-byte aligned)
};

// Build time check that State fits into the shared-memory segment.
// Leave at least 16 MiB for the data area.
static_assert(sizeof(State) + 16 * 1024 * 1024 <= g_shm_total_size,
              "State overflows g_shm_total_size! reduce g_max_objs_per_region or g_max_regions");

//----------------------------------------------------------------------------------------------------------------------

/// Manages a shared-memory that stores all mock FAM States.
/// Thread-safe and process-safe via the shared mutex `LockGuard`.
class FamMockSession {
public:

    /// Obtain (or create) the shared-memory session
    static FamMockSession& instance(const std::string& name = "");

    ~FamMockSession();

    // rules
    FamMockSession(const FamMockSession&)            = delete;
    FamMockSession& operator=(const FamMockSession&) = delete;
    FamMockSession(FamMockSession&&)                 = delete;
    FamMockSession& operator=(FamMockSession&&)      = delete;

    //------------------------------------------------------------------------------------------------------------------

    /// Wipes all mock states and resets the session to the initial state.
    void reset();

    /// Same as reset() but must be called while the mutex is held (e.g., during initialization).
    void resetUnlocked();

    //------------------------------------------------------------------------------------------------------------------

    void lock();
    void unlock();

    //------------------------------------------------------------------------------------------------------------------
    // Region

    Region* findRegionByName(const char* name);
    Region* findRegionById(std::uint64_t regionId);
    Region* allocateRegionSlot();

    /// throws `FAM_ERR_NOTFOUND`
    Region& findRegion(Fam_Region_Descriptor* desc);

    /// Frees all objects in the region and marks it inactive.
    static void freeRegion(Region& region);

    //------------------------------------------------------------------------------------------------------------------
    // Object

    static Object* findObjectByOffset(Region& region, std::uint64_t offset);
    static Object* findObjectByName(Region& region, const char* name);
    static Object* allocateObjectSlot(Region& region);

    /// Finds an object by descriptor or throws `FAM_ERR_NOTFOUND`.
    Object& findObject(Fam_Descriptor* desc);

    static void freeObject(Object& obj);

    //------------------------------------------------------------------------------------------------------------------
    // Data

    /// Allocate @p size bytes in the data area.
    /// Returns the offset from data-area start.
    /// Throws `FAM_ERR_NO_SPACE` if the data area is exhausted.
    std::uint64_t allocateData(std::uint64_t size);

    /// Returns a raw pointer to the payload bytes of an object.
    std::uint8_t* objectData(const Object& obj);

    //------------------------------------------------------------------------------------------------------------------
    // Accessors

    std::uint64_t nextRegion() { return state_->nextRegion++; }

private:

    explicit FamMockSession(const std::string& name);

    /// @param name Identifier for the shared-memory segment
    void mapFields(void* base);

    std::string shmName_;
    int fd_{-1};
    void* mapping_{nullptr};
    State* state_{nullptr};
    std::uint8_t* data_{nullptr};
};

//----------------------------------------------------------------------------------------------------------------------

class LockGuard {
public:

    explicit LockGuard(FamMockSession& session) : session_(session) { session_.lock(); }
    ~LockGuard() { session_.unlock(); }

    // rules
    LockGuard(const LockGuard&)            = delete;
    LockGuard& operator=(const LockGuard&) = delete;
    LockGuard(LockGuard&&)                 = delete;
    LockGuard& operator=(LockGuard&&)      = delete;

private:

    FamMockSession& session_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace mock

}  // namespace openfam
