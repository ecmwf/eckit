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

/// @file   openfam_mock/fam/fam.cc
/// @author Metin Cakircali
/// @date   Mar 2026

#include "fam/fam.h"

#include <unistd.h>  // getuid, getgid

#include <cassert>
#include <cstdlib>  // std::abort
#include <cstring>
#include <string>

#include "fam/fam_exception.h"

#include "eckit/io/fam/openfam_mock/FamMockSession.h"

//----------------------------------------------------------------------------------------------------------------------

namespace openfam {

namespace {

//----------------------------------------------------------------------------------------------------------------------
// HELPERS

template <typename T>
auto typed_fetch(std::uint8_t* base, std::uint64_t objSize, std::uint64_t offset) -> T {
    if (offset > objSize || sizeof(T) > (objSize - offset)) {
        throw Fam_Exception("Offset out of range", FAM_ERR_OUTOFRANGE);
    }
    T value{};
    std::memcpy(&value, base + offset, sizeof(T));
    return value;
}

template <typename T>
void typed_store(std::uint8_t* base, std::uint64_t objSize, std::uint64_t offset, T value) {
    if (offset > objSize || sizeof(T) > (objSize - offset)) {
        throw Fam_Exception("Offset out of range", FAM_ERR_OUTOFRANGE);
    }
    std::memcpy(base + offset, &value, sizeof(T));
}

}  // namespace

//----------------------------------------------------------------------------------------------------------------------

fam::fam()  = default;
fam::~fam() = default;

mock::FamMockSession& fam::session() {
    assert(session_ != nullptr && "fam_initialize() must be called before any other FAM operation");
    return *session_;
}

void fam::fam_initialize(const char* /*name*/, Fam_Options* options) {
    if (options && options->cisServer) {
        serverName_ = options->cisServer;
    }
    session_ = &mock::FamMockSession::instance(serverName_);
}

void fam::fam_finalize(const char* /*name*/) {
    session_ = nullptr;
}

void fam::fam_abort(int /*code*/) {
    std::abort();
}

const void* fam::fam_get_option(char* option_name) {
    if (option_name && std::string(option_name) == "CIS_SERVER") {
        return static_cast<const void*>(serverName_.data());
    }
    return nullptr;
}

//----------------------------------------------------------------------------------------------------------------------
// REGION OPERATIONS

Fam_Region_Descriptor* fam::fam_create_region(const char* name, std::uint64_t size, mode_t perm,
                                              Fam_Region_Attributes* /*region_attributes*/) {
    if (!name || !*name) {
        throw Fam_Exception("Invalid region name", FAM_ERR_INVALID);
    }

    auto& sess = session();
    mock::LockGuard lock(sess);

    if (sess.findRegionByName(name) != nullptr) {
        throw Fam_Exception(std::string("Region already exists: ") + name, FAM_ERR_ALREADYEXIST);
    }

    auto* slot = sess.allocateRegionSlot();
    if (!slot) {
        throw Fam_Exception("Maximum number of regions reached", FAM_ERR_NO_SPACE);
    }

    const auto regionId = sess.nextRegion();

    *slot            = mock::Region{};  // zero-init
    slot->active     = true;
    slot->id         = regionId;
    slot->size       = size;
    slot->perm       = perm;
    slot->nextOffset = 8;  // reserve offset 0 as null sentinel
    std::strncpy(slot->name, name, mock::g_max_name_len - 1);
    slot->name[mock::g_max_name_len - 1] = '\0';

    return new Fam_Region_Descriptor(regionId, size, perm, name);
}

Fam_Region_Descriptor* fam::fam_lookup_region(const char* name) {
    if (!name || !*name) {
        throw Fam_Exception("Invalid region name", FAM_ERR_INVALID);
    }

    auto& sess = session();
    mock::LockGuard lock(sess);

    const auto* region = sess.findRegionByName(name);
    if (!region) {
        throw Fam_Exception(std::string("Region not found: ") + name, FAM_ERR_NOTFOUND);
    }

    return new Fam_Region_Descriptor(region->id, region->size, region->perm, name);
}

void fam::fam_destroy_region(Fam_Region_Descriptor* region_desc) {
    if (!region_desc) {
        return;
    }

    auto& sess = session();
    mock::LockGuard lock(sess);

    const auto regionId = region_desc->get_global_descriptor().regionId;
    auto* region        = sess.findRegionById(regionId);
    if (!region) {
        // Keep region destruction idempotent during test teardown.
        region_desc->mock_invalidate();
        return;
    }

    sess.freeRegion(*region);
    region_desc->mock_invalidate();
}

void fam::fam_resize_region(Fam_Region_Descriptor* region_desc, std::uint64_t size) {
    auto& sess = session();
    mock::LockGuard lock(sess);

    auto& region = sess.findRegion(region_desc);
    region.size  = size;
    region_desc->mock_setSize(size);
}

void fam::fam_stat(Fam_Region_Descriptor* region_desc, Fam_Stat* info) {
    if (!info) {
        return;
    }

    auto& sess = session();
    mock::LockGuard lock(sess);

    const auto& region = sess.findRegion(region_desc);
    info->size         = region.size;
    info->perm         = region.perm;
    std::strncpy(info->name, region.name, sizeof(info->name) - 1);
    info->name[sizeof(info->name) - 1] = '\0';
    info->uid                          = 0;
    info->gid                          = 0;
}

//----------------------------------------------------------------------------------------------------------------------
// OBJECT OPERATIONS

Fam_Descriptor* fam::fam_allocate(const char* name, std::uint64_t size, mode_t perm,
                                  Fam_Region_Descriptor* region_desc) {
    if (!region_desc) {
        throw Fam_Exception("Null region descriptor", FAM_ERR_INVALID);
    }
    if (size == 0) {
        throw Fam_Exception("Object size must be > 0", FAM_ERR_INVALID);
    }

    auto& sess = session();
    mock::LockGuard lock(sess);

    auto& region = sess.findRegion(region_desc);

    // Check for duplicate named object.
    if (name && *name && sess.findObjectByName(region, name) != nullptr) {
        throw Fam_Exception(std::string("Object already exists: ") + name, FAM_ERR_ALREADYEXIST);
    }

    // Match test expectations: reject only objects larger than region size.
    if (size > region.size) {
        throw Fam_Exception("Object exceeds region size", FAM_ERR_NO_SPACE);
    }

    auto* slot = sess.allocateObjectSlot(region);
    if (!slot) {
        throw Fam_Exception("Maximum number of objects per region reached", FAM_ERR_NO_SPACE);
    }

    const std::uint64_t offset  = region.nextOffset;
    const std::uint64_t aligned = ((offset + size) + 7u) & ~std::uint64_t{7};
    region.nextOffset           = aligned;

    // Allocate backing storage in the shared data area.
    const std::uint64_t dataOff = sess.allocateData(size);

    *slot            = mock::Object{};
    slot->active     = true;
    slot->offset     = offset;
    slot->size       = size;
    slot->perm       = perm;
    slot->uid        = ::getuid();
    slot->gid        = ::getgid();
    slot->dataOffset = dataOff;

    if (name && *name) {
        std::strncpy(slot->name, name, mock::g_max_name_len - 1);
        slot->name[mock::g_max_name_len - 1] = '\0';
    }

    // Zero-init the data area for this object.
    std::memset(sess.objectData(*slot), 0, size);

    return new Fam_Descriptor(region.id, offset, size, perm, name, slot->uid, slot->gid);
}

Fam_Descriptor* fam::fam_lookup(const char* object_name, const char* region_name) {
    if (!object_name || !*object_name || !region_name || !*region_name) {
        throw Fam_Exception("Invalid name parameter", FAM_ERR_INVALID);
    }

    auto& sess = session();
    mock::LockGuard lock(sess);

    auto* region = sess.findRegionByName(region_name);
    if (!region) {
        throw Fam_Exception(std::string("Region not found: ") + region_name, FAM_ERR_NOTFOUND);
    }

    auto* obj = sess.findObjectByName(*region, object_name);
    if (!obj) {
        throw Fam_Exception(std::string("Object not found: ") + object_name, FAM_ERR_NOTFOUND);
    }

    return new Fam_Descriptor(region->id, obj->offset, obj->size, obj->perm, object_name, obj->uid, obj->gid);
}

void fam::fam_deallocate(Fam_Descriptor* object) {
    if (!object) {
        return;
    }

    auto& sess = session();
    mock::LockGuard lock(sess);

    const auto regionId     = object->get_global_descriptor().regionId;
    const auto objectOffset = object->get_global_descriptor().offset;

    auto* region = sess.findRegionById(regionId);
    if (!region) {
        // Keep object deallocation idempotent during teardown.
        object->mock_invalidate();
        return;
    }

    auto* obj = sess.findObjectByOffset(*region, objectOffset);
    if (!obj) {
        object->mock_invalidate();
        return;
    }

    const auto nextExpectedOffset  = objectOffset + obj->size;
    const auto nextExpectedAligned = (nextExpectedOffset + 7u) & ~std::uint64_t{7};

    sess.freeObject(*obj);

    // If this was the last allocated object, reclaim its region offset space.
    if (region->nextOffset == nextExpectedAligned) {
        region->nextOffset = objectOffset;
    }

    object->mock_invalidate();
}

void fam::fam_stat(Fam_Descriptor* object, Fam_Stat* info) {
    if (!info) {
        return;
    }

    auto& sess = session();
    mock::LockGuard lock(sess);

    const auto& obj = sess.findObject(object);
    info->size      = obj.size;
    info->perm      = obj.perm;
    std::strncpy(info->name, obj.name, sizeof(info->name) - 1);
    info->name[sizeof(info->name) - 1] = '\0';
    info->uid                          = obj.uid;
    info->gid                          = obj.gid;
}

//----------------------------------------------------------------------------------------------------------------------
// DATA I/O

void fam::fam_put_blocking(void* buffer, Fam_Descriptor* obj, std::uint64_t offset, std::uint64_t length) {
    if (!buffer || !obj || length == 0) {
        throw Fam_Exception("Invalid parameters to fam_put_blocking", FAM_ERR_INVALID);
    }

    auto& sess = session();
    mock::LockGuard lock(sess);

    auto& sobj = sess.findObject(obj);
    if (offset > sobj.size || length > (sobj.size - offset)) {
        throw Fam_Exception("Write range out of bounds", FAM_ERR_OUTOFRANGE);
    }
    std::memcpy(sess.objectData(sobj) + offset, buffer, length);
}

void fam::fam_get_blocking(void* buffer, Fam_Descriptor* obj, std::uint64_t offset, std::uint64_t length) {
    if (!buffer || !obj || length == 0) {
        throw Fam_Exception("Invalid parameters to fam_get_blocking", FAM_ERR_INVALID);
    }

    auto& sess = session();
    mock::LockGuard lock(sess);

    const auto& sobj = sess.findObject(obj);
    if (offset > sobj.size || length > (sobj.size - offset)) {
        throw Fam_Exception("Read range out of bounds", FAM_ERR_OUTOFRANGE);
    }
    std::memcpy(buffer, sess.objectData(sobj) + offset, length);
}

//----------------------------------------------------------------------------------------------------------------------

#define OPENFAM_MOCK_DEFINE_FETCH(TYPE, suffix)                               \
    TYPE fam::fam_fetch_##suffix(Fam_Descriptor* obj, std::uint64_t offset) { \
        auto& sess = session();                                               \
        mock::LockGuard lock(sess);                                           \
        auto& sobj = sess.findObject(obj);                                    \
        return typed_fetch<TYPE>(sess.objectData(sobj), sobj.size, offset);   \
    }

OPENFAM_MOCK_DEFINE_FETCH(std::int32_t, int32)
OPENFAM_MOCK_DEFINE_FETCH(std::int64_t, int64)
OPENFAM_MOCK_DEFINE_FETCH(int128_t, int128)
OPENFAM_MOCK_DEFINE_FETCH(std::uint32_t, uint32)
OPENFAM_MOCK_DEFINE_FETCH(std::uint64_t, uint64)
OPENFAM_MOCK_DEFINE_FETCH(float, float)
OPENFAM_MOCK_DEFINE_FETCH(double, double)

#undef OPENFAM_MOCK_DEFINE_FETCH

#define OPENFAM_MOCK_DEFINE_SET(TYPE)                                          \
    void fam::fam_set(Fam_Descriptor* obj, std::uint64_t offset, TYPE value) { \
        auto& sess = session();                                                \
        mock::LockGuard lock(sess);                                            \
        auto& sobj = sess.findObject(obj);                                     \
        typed_store<TYPE>(sess.objectData(sobj), sobj.size, offset, value);    \
    }

OPENFAM_MOCK_DEFINE_SET(std::int32_t)
OPENFAM_MOCK_DEFINE_SET(std::int64_t)
OPENFAM_MOCK_DEFINE_SET(int128_t)
OPENFAM_MOCK_DEFINE_SET(std::uint32_t)
OPENFAM_MOCK_DEFINE_SET(std::uint64_t)
OPENFAM_MOCK_DEFINE_SET(float)
OPENFAM_MOCK_DEFINE_SET(double)

#undef OPENFAM_MOCK_DEFINE_SET

#define OPENFAM_MOCK_DEFINE_ADD(TYPE)                                                   \
    void fam::fam_add(Fam_Descriptor* obj, std::uint64_t offset, TYPE value) {          \
        auto& sess = session();                                                         \
        mock::LockGuard lock(sess);                                                     \
        auto& sobj   = sess.findObject(obj);                                            \
        auto* data   = sess.objectData(sobj);                                           \
        auto current = typed_fetch<TYPE>(data, sobj.size, offset);                      \
        typed_store<TYPE>(data, sobj.size, offset, static_cast<TYPE>(current + value)); \
    }

OPENFAM_MOCK_DEFINE_ADD(std::int32_t)
OPENFAM_MOCK_DEFINE_ADD(std::int64_t)
OPENFAM_MOCK_DEFINE_ADD(std::uint32_t)
OPENFAM_MOCK_DEFINE_ADD(std::uint64_t)
OPENFAM_MOCK_DEFINE_ADD(float)
OPENFAM_MOCK_DEFINE_ADD(double)

#undef OPENFAM_MOCK_DEFINE_ADD

#define OPENFAM_MOCK_DEFINE_SUB(TYPE)                                                   \
    void fam::fam_subtract(Fam_Descriptor* obj, std::uint64_t offset, TYPE value) {     \
        auto& sess = session();                                                         \
        mock::LockGuard lock(sess);                                                     \
        auto& sobj   = sess.findObject(obj);                                            \
        auto* data   = sess.objectData(sobj);                                           \
        auto current = typed_fetch<TYPE>(data, sobj.size, offset);                      \
        typed_store<TYPE>(data, sobj.size, offset, static_cast<TYPE>(current - value)); \
    }

OPENFAM_MOCK_DEFINE_SUB(std::int32_t)
OPENFAM_MOCK_DEFINE_SUB(std::int64_t)
OPENFAM_MOCK_DEFINE_SUB(std::uint32_t)
OPENFAM_MOCK_DEFINE_SUB(std::uint64_t)
OPENFAM_MOCK_DEFINE_SUB(float)
OPENFAM_MOCK_DEFINE_SUB(double)

#undef OPENFAM_MOCK_DEFINE_SUB

#define OPENFAM_MOCK_DEFINE_SWAP(TYPE)                                          \
    TYPE fam::fam_swap(Fam_Descriptor* obj, std::uint64_t offset, TYPE value) { \
        auto& sess = session();                                                 \
        mock::LockGuard lock(sess);                                             \
        auto& sobj = sess.findObject(obj);                                      \
        auto* data = sess.objectData(sobj);                                     \
        auto old   = typed_fetch<TYPE>(data, sobj.size, offset);                \
        typed_store<TYPE>(data, sobj.size, offset, value);                      \
        return old;                                                             \
    }

OPENFAM_MOCK_DEFINE_SWAP(std::int32_t)
OPENFAM_MOCK_DEFINE_SWAP(std::int64_t)
OPENFAM_MOCK_DEFINE_SWAP(std::uint32_t)
OPENFAM_MOCK_DEFINE_SWAP(std::uint64_t)
OPENFAM_MOCK_DEFINE_SWAP(float)
OPENFAM_MOCK_DEFINE_SWAP(double)

#undef OPENFAM_MOCK_DEFINE_SWAP

#define OPENFAM_MOCK_DEFINE_CAS(TYPE)                                                                   \
    TYPE fam::fam_compare_swap(Fam_Descriptor* obj, std::uint64_t offset, TYPE old_val, TYPE new_val) { \
        auto& sess = session();                                                                         \
        mock::LockGuard lock(sess);                                                                     \
        auto& sobj   = sess.findObject(obj);                                                            \
        auto* data   = sess.objectData(sobj);                                                           \
        auto current = typed_fetch<TYPE>(data, sobj.size, offset);                                      \
        if (current == old_val) {                                                                       \
            typed_store<TYPE>(data, sobj.size, offset, new_val);                                        \
        }                                                                                               \
        return current;                                                                                 \
    }

OPENFAM_MOCK_DEFINE_CAS(std::int32_t)
OPENFAM_MOCK_DEFINE_CAS(std::int64_t)
OPENFAM_MOCK_DEFINE_CAS(std::uint32_t)
OPENFAM_MOCK_DEFINE_CAS(std::uint64_t)
OPENFAM_MOCK_DEFINE_CAS(int128_t)

#undef OPENFAM_MOCK_DEFINE_CAS

//----------------------------------------------------------------------------------------------------------------------

}  // namespace openfam
