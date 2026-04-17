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
/// Capacity
/// default: 64 MiB total shared memory
/// use "export ECKIT_FAM_MOCK_SHM_SIZE=536870912" for 512 MiB, etc. Must be > sizeof(State) (currently ~2 MiB).

constexpr std::size_t g_max_name_len        = 40;    // OpenFAM real dataitem name limit
constexpr std::size_t g_max_regions         = 64;    // Max number of regions (arbitrary limit for testing)
constexpr std::size_t g_max_objs_per_region = 4096;  // Max number of objects per region (arbitrary limit for testing)
constexpr std::size_t g_default_shm_size    = 64 * 1024 * 1024;  // 64 MiB default

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

/// Lives in POSIX shared memory and is accessed by multiple processes.
/// @note POD no virtual, no std::string
struct State {
    /// We use pthread_mutex_t (with PTHREAD_PROCESS_SHARED) instead of std::mutex,
    /// since it is not guaranteed to be copyable, and does not support inter-process locking.
    pthread_mutex_t mutex;

    /// marker for initialization completion (set to `k_init_magic`)
    std::uint32_t initialized;

    /// next region ID (0 is invalid)
    std::uint64_t nextRegion;

    /// bytes used in data area (starts at 0)
    std::uint64_t dataUsed;

    /// runtime data area capacity (set by creator, read by joiners)
    std::uint64_t dataCapacity;

    Region regions[g_max_regions];
};

// Ensure State is 8-byte aligned so that the data area is properly aligned for any type.
static_assert(sizeof(State) % 8 == 0, "State size must be a multiple of 8 for proper data alignment");

// Build time check that State fits into the default shared-memory segment.
static_assert(sizeof(State) < g_default_shm_size,
              "State overflows g_default_shm_size! reduce g_max_objs_per_region or g_max_regions");

//----------------------------------------------------------------------------------------------------------------------

/// Manages a shared-memory that stores all mock FAM States.
/// Thread-safe and process-safe via shared mutex (satisfies Lockable for std::lock_guard).
class FamMockSession {
public:

    struct ShmHandle {
        std::string name;
        std::size_t size{g_default_shm_size};
        int fd{-1};
        void* mapping{nullptr};

        /// Unmaps the shared memory and closes the file descriptor.
        void close();

        /// Removes the shared-memory segment from the filesystem.
        void unlink() const;
    };

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

    /// Derives state_ and data_ pointers from handle_.mapping_.
    void mapFields();

    /// Same as reset() but must be called while the mutex is held (e.g., during initialization).
    void resetUnlocked();

    ShmHandle handle_;

    State* state_{nullptr};
    std::uint8_t* data_{nullptr};
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace mock

}  // namespace openfam
