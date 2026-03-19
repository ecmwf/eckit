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
#include <mutex>
#include <string>

#include "fam/fam_exception.h"

#include "eckit/io/fam/openfam_mock/FamMockSession.h"

//----------------------------------------------------------------------------------------------------------------------

namespace openfam {

fam::fam()  = default;
fam::~fam() = default;

void fam::fam_initialize(const char* /*name*/, Fam_Options* options) {
    if (options && options->cisServer) {
        serverName_ = options->cisServer;
    }
}

void fam::fam_finalize(const char* /*name*/) {
    // nothing to tear down in the mock
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

Fam_Region_Descriptor* fam::fam_create_region(const char* name, std::uint64_t size, mode_t perm,
                                              Fam_Region_Attributes* /*region_attributes*/) {
    if (!name || !*name) {
        throw Fam_Exception("Invalid region name", FAM_ERR_INVALID);
    }

    auto& session = mock::FamMockSession::instance();
    std::lock_guard lock(session.mutex_);

    if (session.regionsByName_.count(name) != 0) {
        throw Fam_Exception(std::string("Region already exists: ") + name, FAM_ERR_ALREADYEXIST);
    }

    const std::uint64_t regionId = session.nextRegionId_++;

    mock::MockRegion mregion;
    mregion.name = name;
    mregion.id   = regionId;
    mregion.size = size;
    mregion.perm = perm;

    session.regionsByName_[name] = regionId;
    session.regions_[regionId]   = std::move(mregion);

    return new Fam_Region_Descriptor(regionId, size, perm, name);
}

Fam_Region_Descriptor* fam::fam_lookup_region(const char* name) {
    if (!name || !*name) {
        throw Fam_Exception("Invalid region name", FAM_ERR_INVALID);
    }

    auto& session = mock::FamMockSession::instance();
    std::lock_guard lock(session.mutex_);

    const auto iter = session.regionsByName_.find(name);
    if (iter == session.regionsByName_.end()) {
        throw Fam_Exception(std::string("Region not found: ") + name, FAM_ERR_NOTFOUND);
    }

    const auto& mregion = session.regions_.at(iter->second);
    return new Fam_Region_Descriptor(mregion.id, mregion.size, mregion.perm, name);
}

void fam::fam_destroy_region(Fam_Region_Descriptor* region_desc) {
    if (!region_desc) {
        return;
    }

    auto& session = mock::FamMockSession::instance();
    std::lock_guard lock(session.mutex_);

    const auto regionId = region_desc->get_global_descriptor().regionId;
    const auto iter     = session.regions_.find(regionId);
    if (iter == session.regions_.end()) {
        // Keep region destruction idempotent during test teardown.
        region_desc->mock_invalidate();
        return;
    }

    session.regionsByName_.erase(iter->second.name);
    session.regions_.erase(iter);
    region_desc->mock_invalidate();
}

void fam::fam_resize_region(Fam_Region_Descriptor* region_desc, std::uint64_t size) {
    auto& session = mock::FamMockSession::instance();
    std::lock_guard lock(session.mutex_);

    auto& mregion = session.findRegion(region_desc);
    mregion.size  = size;
    region_desc->mock_setSize(size);
}

void fam::fam_stat(Fam_Region_Descriptor* region_desc, Fam_Stat* info) {
    if (!info) {
        return;
    }

    auto& session = mock::FamMockSession::instance();
    std::lock_guard lock(session.mutex_);

    const auto& mregion = session.findRegion(region_desc);
    info->size          = mregion.size;
    info->perm          = mregion.perm;
    std::strncpy(info->name, mregion.name.c_str(), sizeof(info->name) - 1);
    info->name[sizeof(info->name) - 1] = '\0';
    info->uid                          = 0;
    info->gid                          = 0;
}

Fam_Descriptor* fam::fam_allocate(const char* name, std::uint64_t size, mode_t perm,
                                  Fam_Region_Descriptor* region_desc) {
    if (!region_desc) {
        throw Fam_Exception("Null region descriptor", FAM_ERR_INVALID);
    }
    if (size == 0) {
        throw Fam_Exception("Object size must be > 0", FAM_ERR_INVALID);
    }

    auto& session = mock::FamMockSession::instance();
    std::lock_guard lock(session.mutex_);

    auto& mregion = session.findRegion(region_desc);

    const std::string objName = name ? name : "";
    if (!objName.empty() && mregion.objectsByName.count(objName)) {
        throw Fam_Exception(std::string("Object already exists: ") + objName, FAM_ERR_ALREADYEXIST);
    }

    const std::uint64_t offset = mregion.nextOffset;
    // Advance next-offset; align to 8 bytes after the object.
    mregion.nextOffset += size;
    const std::uint64_t aligned = (mregion.nextOffset + 7u) & ~std::uint64_t{7};

    // Match test expectations: reject only objects larger than region size.
    if (size > mregion.size) {
        // Reset to original state on failure
        mregion.nextOffset = offset;
        throw Fam_Exception("Object exceeds region size", FAM_ERR_NO_SPACE);
    }

    mregion.nextOffset = aligned;

    mock::MockObject mobj;
    mobj.name = objName;
    mobj.size = size;
    mobj.perm = perm;
    mobj.uid  = ::getuid();
    mobj.gid  = ::getgid();
    mobj.data.assign(size, std::uint8_t{0});

    if (!objName.empty()) {
        mregion.objectsByName[objName] = offset;
    }
    mregion.objects[offset] = std::move(mobj);

    return new Fam_Descriptor(mregion.id, offset, size, perm, name, ::getuid(), ::getgid());
}

Fam_Descriptor* fam::fam_lookup(const char* object_name, const char* region_name) {
    if (!object_name || !*object_name || !region_name || !*region_name) {
        throw Fam_Exception("Invalid name parameter", FAM_ERR_INVALID);
    }

    auto& session = mock::FamMockSession::instance();
    std::lock_guard lock(session.mutex_);

    const auto riter = session.regionsByName_.find(region_name);
    if (riter == session.regionsByName_.end()) {
        throw Fam_Exception(std::string("Region not found: ") + region_name, FAM_ERR_NOTFOUND);
    }

    mock::MockRegion& mregion = session.regions_[riter->second];
    const auto oiter          = mregion.objectsByName.find(object_name);
    if (oiter == mregion.objectsByName.end()) {
        throw Fam_Exception(std::string("Object not found: ") + object_name, FAM_ERR_NOTFOUND);
    }

    const mock::MockObject& obj = mregion.objects.at(oiter->second);
    return new Fam_Descriptor(mregion.id, oiter->second, obj.size, obj.perm, object_name, obj.uid, obj.gid);
}

void fam::fam_deallocate(Fam_Descriptor* object) {
    if (!object) {
        return;
    }

    auto& session = mock::FamMockSession::instance();
    std::lock_guard lock(session.mutex_);

    const auto regionId     = object->get_global_descriptor().regionId;
    const auto objectOffset = object->get_global_descriptor().offset;

    auto riter = session.regions_.find(regionId);
    if (riter == session.regions_.end()) {
        // Keep object deallocation idempotent during teardown.
        object->mock_invalidate();
        return;
    }

    auto oiter = riter->second.objects.find(objectOffset);
    if (oiter == riter->second.objects.end()) {
        object->mock_invalidate();
        return;
    }
    const auto objectSize          = oiter->second.size;
    const auto nextExpectedOffset  = objectOffset + objectSize;
    const auto nextExpectedAligned = (nextExpectedOffset + 7u) & ~std::uint64_t{7};

    if (!oiter->second.name.empty()) {
        riter->second.objectsByName.erase(oiter->second.name);
    }
    riter->second.objects.erase(oiter);
    // If this was the last allocated object, reclaim its space
    if (riter->second.nextOffset == nextExpectedAligned) {
        riter->second.nextOffset = objectOffset;
    }

    object->mock_invalidate();
}

void fam::fam_stat(Fam_Descriptor* object, Fam_Stat* info) {
    if (!info) {
        return;
    }

    auto& session = mock::FamMockSession::instance();
    std::lock_guard lock(session.mutex_);

    const mock::MockObject& mobj = session.findObject(object);
    info->size                   = mobj.size;
    info->perm                   = mobj.perm;
    std::strncpy(info->name, mobj.name.c_str(), sizeof(info->name) - 1);
    info->name[sizeof(info->name) - 1] = '\0';
    info->uid                          = mobj.uid;
    info->gid                          = mobj.gid;
}

void fam::fam_put_blocking(void* buffer, Fam_Descriptor* obj, std::uint64_t offset, std::uint64_t length) {
    if (!buffer || !obj || length == 0) {
        throw Fam_Exception("Invalid parameters to fam_put_blocking", FAM_ERR_INVALID);
    }

    auto& session = mock::FamMockSession::instance();
    std::lock_guard lock(session.mutex_);

    mock::MockObject& mobj = session.findObject(obj);
    if (offset > mobj.data.size() || length > (mobj.data.size() - offset)) {
        throw Fam_Exception("Write range out of bounds", FAM_ERR_OUTOFRANGE);
    }
    std::memcpy(mobj.data.data() + offset, buffer, length);
}

void fam::fam_get_blocking(void* buffer, Fam_Descriptor* obj, std::uint64_t offset, std::uint64_t length) {
    if (!buffer || !obj || length == 0) {
        throw Fam_Exception("Invalid parameters to fam_get_blocking", FAM_ERR_INVALID);
    }

    auto& session = mock::FamMockSession::instance();
    std::lock_guard lock(session.mutex_);

    const mock::MockObject& mobj = session.findObject(obj);
    if (offset > mobj.data.size() || length > (mobj.data.size() - offset)) {
        throw Fam_Exception("Read range out of bounds", FAM_ERR_OUTOFRANGE);
    }
    std::memcpy(buffer, mobj.data.data() + offset, length);
}

//----------------------------------------------------------------------------------------------------------------------

#define OPENFAM_MOCK_DEFINE_FETCH(TYPE, suffix)                               \
    TYPE fam::fam_fetch_##suffix(Fam_Descriptor* obj, std::uint64_t offset) { \
        auto& session = mock::FamMockSession::instance();                     \
        std::lock_guard lock(session.mutex_);                                 \
        mock::MockObject& mobj = session.findObject(obj);                     \
        return mock::typedFetch<TYPE>(mobj.data, offset);                     \
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
        auto& session = mock::FamMockSession::instance();                      \
        std::lock_guard lock(session.mutex_);                                  \
        mock::MockObject& mobj = session.findObject(obj);                      \
        mock::typedStore<TYPE>(mobj.data, offset, value);                      \
    }

OPENFAM_MOCK_DEFINE_SET(std::int32_t)
OPENFAM_MOCK_DEFINE_SET(std::int64_t)
OPENFAM_MOCK_DEFINE_SET(int128_t)
OPENFAM_MOCK_DEFINE_SET(std::uint32_t)
OPENFAM_MOCK_DEFINE_SET(std::uint64_t)
OPENFAM_MOCK_DEFINE_SET(float)
OPENFAM_MOCK_DEFINE_SET(double)

#undef OPENFAM_MOCK_DEFINE_SET

#define OPENFAM_MOCK_DEFINE_ADD(TYPE)                                                                 \
    void fam::fam_add(Fam_Descriptor* obj, std::uint64_t offset, TYPE value) {                        \
        auto& session = mock::FamMockSession::instance();                                             \
        std::lock_guard lock(session.mutex_);                                                         \
        mock::MockObject& mobj = session.findObject(obj);                                             \
        mock::typedStore<TYPE>(mobj.data, offset,                                                     \
                               static_cast<TYPE>(mock::typedFetch<TYPE>(mobj.data, offset) + value)); \
    }

OPENFAM_MOCK_DEFINE_ADD(std::int32_t)
OPENFAM_MOCK_DEFINE_ADD(std::int64_t)
OPENFAM_MOCK_DEFINE_ADD(std::uint32_t)
OPENFAM_MOCK_DEFINE_ADD(std::uint64_t)
OPENFAM_MOCK_DEFINE_ADD(float)
OPENFAM_MOCK_DEFINE_ADD(double)

#undef OPENFAM_MOCK_DEFINE_ADD

#define OPENFAM_MOCK_DEFINE_SUB(TYPE)                                                                 \
    void fam::fam_subtract(Fam_Descriptor* obj, std::uint64_t offset, TYPE value) {                   \
        auto& session = mock::FamMockSession::instance();                                             \
        std::lock_guard lock(session.mutex_);                                                         \
        mock::MockObject& mobj = session.findObject(obj);                                             \
        mock::typedStore<TYPE>(mobj.data, offset,                                                     \
                               static_cast<TYPE>(mock::typedFetch<TYPE>(mobj.data, offset) - value)); \
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
        auto& session = mock::FamMockSession::instance();                       \
        std::lock_guard lock(session.mutex_);                                   \
        mock::MockObject& mobj = session.findObject(obj);                       \
        auto old               = mock::typedFetch<TYPE>(mobj.data, offset);     \
        mock::typedStore<TYPE>(mobj.data, offset, value);                       \
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
        auto& session = mock::FamMockSession::instance();                                               \
        std::lock_guard lock(session.mutex_);                                                           \
        mock::MockObject& mobj = session.findObject(obj);                                               \
        auto current           = mock::typedFetch<TYPE>(mobj.data, offset);                             \
        if (current == old_val) {                                                                       \
            mock::typedStore<TYPE>(mobj.data, offset, new_val);                                         \
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
