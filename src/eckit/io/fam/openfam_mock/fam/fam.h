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

/// @file   openfam_mock/fam/fam.h
/// @author Metin Cakircali
/// @date   Mar 2026

/// @brief  Mock of OpenFAM.
//          Mirrors the OpenFAM fam.h: types, descriptors, and the openfam::fam class.

#pragma once

#include <sys/types.h>  // mode_t

#include <cstdint>
#include <cstring>
#include <string>

//----------------------------------------------------------------------------------------------------------------------

struct Fam_Global_Descriptor {
    std::uint64_t regionId{0};
    std::uint64_t offset{0};
};

struct Fam_Stat {
    std::uint64_t size{0};
    mode_t perm{0};
    char name[256]{};
    std::uint32_t uid{0};
    std::uint32_t gid{0};
};

enum Fam_Permission_Level {
    PERMISSION_LEVEL_DEFAULT = 0,
    REGION,
    DATAITEM,
};

struct Fam_Region_Attributes {
    int redundancyLevel{0};
    int memoryType{0};
    int interleaveEnable{0};
    Fam_Permission_Level permissionLevel{PERMISSION_LEVEL_DEFAULT};
};

struct Fam_Options {
    char* runtime{nullptr};
    char* cisServer{nullptr};
    char* grpcPort{nullptr};
    char* openFamModel{nullptr};
    char* famThreadModel{nullptr};
    char* numConsumer{nullptr};
    char* allocator{nullptr};
    char* rpcOn{nullptr};
};

//----------------------------------------------------------------------------------------------------------------------

namespace openfam {

using int128_t = __int128;

enum class Fam_Descriptor_Status {
    DESC_INVALID,
    DESC_INIT_DONE,
    DESC_INIT_DONE_BUT_KEY_NOT_VALID,
    DESC_UNINITIALIZED,
};

//----------------------------------------------------------------------------------------------------------------------
// Region descriptor

class Fam_Region_Descriptor {
public:

    Fam_Region_Descriptor() = default;

    Fam_Region_Descriptor(std::uint64_t regionId, std::uint64_t size, mode_t perm, const char* name,
                          std::uint32_t uid = 0, std::uint32_t gid = 0) :
        desc_{regionId, 0},
        size_(size),
        perm_(perm),
        name_(name ? name : ""),
        uid_(uid),
        gid_(gid),
        status_(Fam_Descriptor_Status::DESC_INIT_DONE) {}

    Fam_Global_Descriptor get_global_descriptor() const noexcept { return desc_; }
    Fam_Descriptor_Status get_desc_status() const noexcept { return status_; }
    std::uint64_t get_size() const noexcept { return size_; }
    mode_t get_perm() const noexcept { return perm_; }

    /// Returns a pointer to the region name string, or nullptr when empty.
    const char* get_name() const noexcept { return name_.empty() ? nullptr : name_.c_str(); }

    std::uint32_t get_uid() const noexcept { return uid_; }
    std::uint32_t get_gid() const noexcept { return gid_; }

    void set_permissionLevel(Fam_Permission_Level level) noexcept { permLevel_ = level; }
    Fam_Permission_Level get_permissionLevel() const noexcept { return permLevel_; }

    /// Internal helpers used by the mock storage implementation.
    void mock_invalidate() noexcept { status_ = Fam_Descriptor_Status::DESC_INVALID; }
    void mock_setSize(std::uint64_t size) noexcept { size_ = size; }

private:

    Fam_Global_Descriptor desc_;
    std::uint64_t size_{0};
    mode_t perm_{0};
    std::string name_;
    std::uint32_t uid_{0};
    std::uint32_t gid_{0};
    Fam_Descriptor_Status status_{Fam_Descriptor_Status::DESC_UNINITIALIZED};
    Fam_Permission_Level permLevel_{PERMISSION_LEVEL_DEFAULT};
};

//----------------------------------------------------------------------------------------------------------------------
// Object (data item) descriptor

class Fam_Descriptor {
public:

    /// Construct a proxy descriptor from a raw global address.
    /// Used by FamObject(session, region, offset) and replaceWith().
    explicit Fam_Descriptor(Fam_Global_Descriptor desc) : desc_(desc), status_(Fam_Descriptor_Status::DESC_INIT_DONE) {}

    /// Construct a fully-populated descriptor (from allocate / lookup).
    Fam_Descriptor(std::uint64_t regionId, std::uint64_t offset, std::uint64_t size, mode_t perm, const char* name,
                   std::uint32_t uid = 0, std::uint32_t gid = 0) :
        desc_{regionId, offset},
        size_(size),
        perm_(perm),
        name_(name ? name : ""),
        uid_(uid),
        gid_(gid),
        status_(Fam_Descriptor_Status::DESC_INIT_DONE) {}

    Fam_Global_Descriptor get_global_descriptor() const noexcept { return desc_; }
    Fam_Descriptor_Status get_desc_status() const noexcept { return status_; }
    std::uint64_t get_size() const noexcept { return size_; }
    mode_t get_perm() const noexcept { return perm_; }

    /// Returns a pointer to the object name string, or nullptr when empty.
    const char* get_name() const noexcept { return name_.empty() ? nullptr : name_.c_str(); }

    std::uint32_t get_uid() const noexcept { return uid_; }
    std::uint32_t get_gid() const noexcept { return gid_; }

    /// Internal helpers used by the mock storage implementation.
    void mock_invalidate() noexcept { status_ = Fam_Descriptor_Status::DESC_INVALID; }
    void mock_setSize(std::uint64_t size) noexcept { size_ = size; }

private:

    Fam_Global_Descriptor desc_{};
    std::uint64_t size_{0};
    mode_t perm_{0};
    std::string name_;
    std::uint32_t uid_{0};
    std::uint32_t gid_{0};
    Fam_Descriptor_Status status_{Fam_Descriptor_Status::DESC_UNINITIALIZED};
};

//----------------------------------------------------------------------------------------------------------------------

namespace mock {
class FamMockSession;
}  // namespace mock

class fam {
public:

    fam();
    ~fam();

    // non-copyable, non-movable (same as real openfam::fam)
    fam(const fam&)            = delete;
    fam& operator=(const fam&) = delete;
    fam(fam&&)                 = delete;
    fam& operator=(fam&&)      = delete;

    //------------------------------------------------------------------
    // Session lifecycle

    void fam_initialize(const char* name, Fam_Options* options);
    void fam_finalize(const char* name);
    void fam_abort(int code);
    const void* fam_get_option(char* option_name);

    //------------------------------------------------------------------
    // Region operations

    Fam_Region_Descriptor* fam_create_region(const char* name, std::uint64_t size, mode_t perm,
                                             Fam_Region_Attributes* region_attributes);

    Fam_Region_Descriptor* fam_lookup_region(const char* name);

    void fam_destroy_region(Fam_Region_Descriptor* region);
    void fam_resize_region(Fam_Region_Descriptor* region, std::uint64_t size);

    void fam_stat(Fam_Region_Descriptor* region, Fam_Stat* info);

    //------------------------------------------------------------------
    // Object operations

    Fam_Descriptor* fam_allocate(const char* name, std::uint64_t size, mode_t perm, Fam_Region_Descriptor* region);

    Fam_Descriptor* fam_lookup(const char* object_name, const char* region_name);

    void fam_deallocate(Fam_Descriptor* object);
    void fam_stat(Fam_Descriptor* object, Fam_Stat* info);

    //------------------------------------------------------------------
    // Blocking data I/O

    void fam_put_blocking(void* buffer, Fam_Descriptor* obj, std::uint64_t offset, std::uint64_t length);

    void fam_get_blocking(void* buffer, Fam_Descriptor* obj, std::uint64_t offset, std::uint64_t length);

    //------------------------------------------------------------------
    // Atomic fetch

    std::int32_t fam_fetch_int32(Fam_Descriptor* obj, std::uint64_t offset);
    std::int64_t fam_fetch_int64(Fam_Descriptor* obj, std::uint64_t offset);
    int128_t fam_fetch_int128(Fam_Descriptor* obj, std::uint64_t offset);
    std::uint32_t fam_fetch_uint32(Fam_Descriptor* obj, std::uint64_t offset);
    std::uint64_t fam_fetch_uint64(Fam_Descriptor* obj, std::uint64_t offset);
    float fam_fetch_float(Fam_Descriptor* obj, std::uint64_t offset);
    double fam_fetch_double(Fam_Descriptor* obj, std::uint64_t offset);

    //------------------------------------------------------------------
    // Atomic set (overloaded on value type)

    void fam_set(Fam_Descriptor* obj, std::uint64_t offset, std::int32_t value);
    void fam_set(Fam_Descriptor* obj, std::uint64_t offset, std::int64_t value);
    void fam_set(Fam_Descriptor* obj, std::uint64_t offset, int128_t value);
    void fam_set(Fam_Descriptor* obj, std::uint64_t offset, std::uint32_t value);
    void fam_set(Fam_Descriptor* obj, std::uint64_t offset, std::uint64_t value);
    void fam_set(Fam_Descriptor* obj, std::uint64_t offset, float value);
    void fam_set(Fam_Descriptor* obj, std::uint64_t offset, double value);

    //------------------------------------------------------------------
    // Atomic add

    void fam_add(Fam_Descriptor* obj, std::uint64_t offset, std::int32_t value);
    void fam_add(Fam_Descriptor* obj, std::uint64_t offset, std::int64_t value);
    void fam_add(Fam_Descriptor* obj, std::uint64_t offset, std::uint32_t value);
    void fam_add(Fam_Descriptor* obj, std::uint64_t offset, std::uint64_t value);
    void fam_add(Fam_Descriptor* obj, std::uint64_t offset, float value);
    void fam_add(Fam_Descriptor* obj, std::uint64_t offset, double value);

    //------------------------------------------------------------------
    // Atomic subtract

    void fam_subtract(Fam_Descriptor* obj, std::uint64_t offset, std::int32_t value);
    void fam_subtract(Fam_Descriptor* obj, std::uint64_t offset, std::int64_t value);
    void fam_subtract(Fam_Descriptor* obj, std::uint64_t offset, std::uint32_t value);
    void fam_subtract(Fam_Descriptor* obj, std::uint64_t offset, std::uint64_t value);
    void fam_subtract(Fam_Descriptor* obj, std::uint64_t offset, float value);
    void fam_subtract(Fam_Descriptor* obj, std::uint64_t offset, double value);

    //------------------------------------------------------------------
    // Atomic swap (returns old value)

    std::int32_t fam_swap(Fam_Descriptor* obj, std::uint64_t offset, std::int32_t value);
    std::int64_t fam_swap(Fam_Descriptor* obj, std::uint64_t offset, std::int64_t value);
    std::uint32_t fam_swap(Fam_Descriptor* obj, std::uint64_t offset, std::uint32_t value);
    std::uint64_t fam_swap(Fam_Descriptor* obj, std::uint64_t offset, std::uint64_t value);
    float fam_swap(Fam_Descriptor* obj, std::uint64_t offset, float value);
    double fam_swap(Fam_Descriptor* obj, std::uint64_t offset, double value);

    //------------------------------------------------------------------
    // Atomic compare-and-swap (returns old value; writes new_val only when old matches)

    std::int32_t fam_compare_swap(Fam_Descriptor* obj, std::uint64_t offset, std::int32_t old_val,
                                  std::int32_t new_val);
    std::int64_t fam_compare_swap(Fam_Descriptor* obj, std::uint64_t offset, std::int64_t old_val,
                                  std::int64_t new_val);
    std::uint32_t fam_compare_swap(Fam_Descriptor* obj, std::uint64_t offset, std::uint32_t old_val,
                                   std::uint32_t new_val);
    std::uint64_t fam_compare_swap(Fam_Descriptor* obj, std::uint64_t offset, std::uint64_t old_val,
                                   std::uint64_t new_val);
    int128_t fam_compare_swap(Fam_Descriptor* obj, std::uint64_t offset, int128_t old_val, int128_t new_val);

private:

    mock::FamMockSession& getSession();

private:

    std::string serverName_;

    mock::FamMockSession* session_{nullptr};  // local cache, initialized in fam_initialize()
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace openfam
