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

/// @brief  Provides a singleton used by mock openfam::fam methods.
///
/// Notes:
/// - Regions and objects are stored in std::map containers protected.
/// - Atomic read-modify-write operations are serialised via mutex,
///   giving sequential-consistency semantics (stricter than real FAM but safe).
/// - The singleton persists for the lifetime of the process.  Tests that need
///   isolated state should call FamMockSession::reset() between test cases.

#include <unistd.h>  // getuid, getgid

#include <cassert>
#include <cstdlib>  // std::abort
#include <cstring>
#include <map>
#include <mutex>
#include <string>
#include <vector>

#include "fam/fam.h"
#include "fam/fam_exception.h"

namespace openfam::mock {

//----------------------------------------------------------------------------------------------------------------------
// Helpers for accessing typed data within an object's byte buffer

template <typename T>
T typedFetch(const std::vector<std::uint8_t>& data, std::uint64_t offset) {
    if (offset > data.size() || sizeof(T) > (data.size() - offset)) {
        throw openfam::Fam_Exception("Offset out of range", openfam::FAM_ERR_OUTOFRANGE);
    }
    T value{};
    std::memcpy(&value, data.data() + offset, sizeof(T));
    return value;
}

template <typename T>
void typedStore(std::vector<std::uint8_t>& data, std::uint64_t offset, T value) {
    if (offset > data.size() || sizeof(T) > (data.size() - offset)) {
        throw openfam::Fam_Exception("Offset out of range", openfam::FAM_ERR_OUTOFRANGE);
    }
    std::memcpy(data.data() + offset, &value, sizeof(T));
}

//----------------------------------------------------------------------------------------------------------------------

struct MockObject {
    std::string name;
    std::uint64_t size{0};
    mode_t perm{0};
    std::uint32_t uid{0};
    std::uint32_t gid{0};
    std::vector<std::uint8_t> data;
};

struct MockRegion {
    std::string name;
    std::uint64_t id{0};
    std::uint64_t size{0};
    mode_t perm{0};

    /// Name-to-offset index for named objects.
    std::map<std::string, std::uint64_t> objectsByName;

    /// Primary object store: offset -> object data.
    std::map<std::uint64_t, MockObject> objects;

    /// Next allocation offset inside this region.
    /// Starts at 8 so that offset 0 remains a sentinel/null value
    /// (FamDescriptor{0,0} is used as the null descriptor in eckit).
    std::uint64_t nextOffset{8};
};

//----------------------------------------------------------------------------------------------------------------------

class FamMockSession {
public:

    static FamMockSession& instance() {
        // Intentionally leaked to avoid static-destruction order issues with
        // global test fixtures calling mock APIs during process shutdown.
        static auto* session = new FamMockSession();
        return *session;
    }

    /// Wipe all state.  Useful for test isolation.
    void reset() {
        std::lock_guard lock(mutex_);
        regionsByName_.clear();
        regions_.clear();
        nextRegionId_ = 1;
    }

    //------------------------------------------------------------------------------------------------------------------
    // Region helpers

    MockRegion& findRegion(openfam::Fam_Region_Descriptor* desc) {
        const auto regionId = desc->get_global_descriptor().regionId;
        auto iter           = regions_.find(regionId);
        if (iter == regions_.end()) {
            throw openfam::Fam_Exception("Region not found", openfam::FAM_ERR_NOTFOUND);
        }
        return iter->second;
    }

    const MockRegion& findRegion(const openfam::Fam_Region_Descriptor* desc) const {
        const auto regionId = desc->get_global_descriptor().regionId;
        const auto iter     = regions_.find(regionId);
        if (iter == regions_.end()) {
            throw openfam::Fam_Exception("Region not found", openfam::FAM_ERR_NOTFOUND);
        }
        return iter->second;
    }

    //------------------------------------------------------------------------------------------------------------------
    // Object helpers

    MockObject& findObject(openfam::Fam_Descriptor* desc) {
        const auto regionId     = desc->get_global_descriptor().regionId;
        const auto objectOffset = desc->get_global_descriptor().offset;

        auto riter = regions_.find(regionId);
        if (riter == regions_.end()) {
            throw openfam::Fam_Exception("Region not found", openfam::FAM_ERR_NOTFOUND);
        }
        auto oiter = riter->second.objects.find(objectOffset);
        if (oiter == riter->second.objects.end()) {
            throw openfam::Fam_Exception("Object not found", openfam::FAM_ERR_NOTFOUND);
        }
        return oiter->second;
    }

    //------------------------------------------------------------------------------------------------------------------
    // Public member data (accessed directly by openfam::fam methods)

    std::mutex mutex_;

    std::map<std::string, std::uint64_t> regionsByName_;  // name -> regionId
    std::map<std::uint64_t, MockRegion> regions_;         // regionId -> region

    std::uint64_t nextRegionId_{1};  // 0 is invalid

private:

    FamMockSession() = default;
};

}  // namespace openfam::mock

//----------------------------------------------------------------------------------------------------------------------
