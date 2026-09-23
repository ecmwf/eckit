// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/*
 * This software was developed as part of the Horizon Europe programme funded project OpenCUBE
 * (Grant agreement: 101092984) horizon-opencube.eu
 */

/// @file   FamTypes.h
/// @author Metin Cakircali
/// @date   Mar 2026

#pragma once

#include <sys/types.h>  // mode_t

#include <cstdint>  // uint64_t
#include <type_traits>

//----------------------------------------------------------------------------------------------------------------------

namespace openfam {
class Fam_Descriptor;
class Fam_Region_Descriptor;
}  // namespace openfam

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

using FamObjectDescriptor = openfam::Fam_Descriptor;
using FamRegionDescriptor = openfam::Fam_Region_Descriptor;

namespace fam {

constexpr const char* scheme = "fam";

using size_t  = std::uint64_t;
using perm_t  = mode_t;
using index_t = std::uint64_t;

/// Bit split used by FamDescriptor::pack(): low bits hold the object offset, high bits the region id.
///
/// Region half — 24 bits. A data item's region id is `regionNumber | (memoryServerId << REGIONID_BITS)` with
/// REGIONID_BITS = 14, so the remaining 10 bits cap configured `memsrv_list` node ids at 1023. OpenFAM itself caps the
/// memory server *count* at MAX_MEMORY_SERVERS_CNT = 256, but the ids are free-form values parsed from the config.
///
/// Offset half — 40 bits. An offset is an NVMM GlobalPtr, `{24-bit shelfId, 40-bit offset}` (nvmm/global_ptr.h,
/// kOffsetShift = 40). A region is created as a single shelf — `CreateHeap(regionId, perServerSize, ...)`, where the
/// size argument is the shelf size — so only a fam_resize_region() could push an offset past 2^40. OpenFAM would
/// break first anyway: its fabric keys encode `offset / MIN_OBJ_SIZE` in DATAITEMID_BITS = 33.
///
/// Both bounds are checked by pack().
constexpr unsigned packed_offset_bits = 40;
constexpr index_t packed_offset_mask  = (index_t{1} << packed_offset_bits) - 1;
constexpr index_t packed_region_max   = (index_t{1} << (64U - packed_offset_bits)) - 1;

}  // namespace fam

/// @brief Address of a FAM object: OpenFAM's region id plus the object's byte offset.
///
/// @section regionid The region id encodes the memory server
///
/// `Fam_Global_Descriptor::regionId` is not a region id alone. For a *data item* it is the region number with the
/// serving memory server packed into bits 14 and above. OpenFAM says so itself, in Fam_Allocator_Client::allocate:
///
/// @code
/// // Note : This global descriptor can not be used to create
/// // Fam_Region_Descriptor because along with region id, first memory
/// // server id is stored in regionId field of Fam_Global_Descriptor
/// globalDescriptor.regionId = regionId | (info.memoryServerIds[0] << MEMSERVERID_SHIFT);
/// @endcode
///
/// *Region* descriptors never carry those bits — `create_region` and `lookup_region` both store a bare
/// `info.regionId` — which is why FamRegion::index() and the ids on that region's objects routinely disagree.
///
/// Placement is deterministic, not random (fam_metadata_service_direct.cpp):
///  - a region's first server is `hash(regionName) % memoryServerCount`; it spans one server when
///    `size <= region_span_size_per_memoryserver` (default 1 GiB), otherwise `ceil(size / span)` of them;
///  - an item's first server is `hash(itemName) % used_memsrv_cnt`. This runs even with `interleaveEnable = DISABLE`;
///    that flag only controls striping of a single item, never the scatter of items across servers.
///
/// So a single-server region gives every item one shared id, while a multi-server region holds items under several
/// different ids at once. A per-region "corrected" id cannot work.
///
/// @section offsets Offsets are unique only per memory server
///
/// The memory server is part of an item's metadata identity — `Fam_CIS_Direct::get_dataitem_id` builds the key as
/// `(memoryServerId << 48) | (offset / MIN_OBJ_SIZE)` — so offsets repeat across servers. Two live items in one region
/// can both sit at offset 4096 under different region ids.
///
/// An offset alone is therefore **not an address**. Resolving one under the wrong region id does not merely fail, it
/// can silently resolve to a *different, valid* item; probing candidate region ids until one succeeds is unsafe.
///
/// @section usage Consequence
///
/// Carry the full {regionId, offset} pair that fam_allocate/fam_lookup returned, everywhere an item address is stored
/// or published. Never rebuild the region half from the region descriptor.
struct FamDescriptor {
    fam::index_t region{0};
    fam::index_t offset{0};

    /// Pack into a single 64-bit word, so that FAM-resident links can be published with one atomic compare-and-swap
    /// (OpenFAM offers no wider atomics). Throws if either half exceeds its budget.
    fam::index_t pack() const;

    static FamDescriptor unpack(const fam::index_t packed) {
        return {packed >> fam::packed_offset_bits, packed & fam::packed_offset_mask};
    }

    bool operator==(const FamDescriptor& other) const { return region == other.region && offset == other.offset; }

    bool operator!=(const FamDescriptor& other) const { return !(*this == other); }
};

static_assert(std::is_standard_layout_v<FamDescriptor>, "FamDescriptor must be standard-layout for offsetof()");
static_assert(std::is_trivially_copyable_v<FamDescriptor>, "FamDescriptor must be trivially copyable for FAM put/get");
static_assert(sizeof(FamDescriptor) == 16, "FamDescriptor layout changed");

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
