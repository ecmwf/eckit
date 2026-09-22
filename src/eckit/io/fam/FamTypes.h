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
/// Budget, from OpenFAM src/common/fam_internal.h and NVMM global_ptr.h:
///  - region id = regionNumber | (memoryServerId << REGIONID_BITS), with REGIONID_BITS = 14. The remaining 10 bits cap
///    the memory server ids configured in `memsrv_list` at 1023 (OpenFAM itself caps the *count* at 256).
///  - offset is an NVMM GlobalPtr: shelfIndex in bits 40+, intra-shelf offset below. A region is created as a single
///    shelf, and OpenFAM's own fabric keys encode `offset / 128` in 33 bits, so it too breaks past 2^40.
/// Both bounds are checked by pack().
constexpr unsigned packed_offset_bits = 40;
constexpr index_t packed_offset_mask  = (index_t{1} << packed_offset_bits) - 1;
constexpr index_t packed_region_max   = (index_t{1} << (64U - packed_offset_bits)) - 1;

}  // namespace fam

/// @brief Address of a FAM object: OpenFAM's region id plus the object's byte offset.
///
/// Both halves are significant. When a region spans several memory servers OpenFAM encodes the serving memory server in
/// the high bits of the region id, and object offsets are only unique within one memory server — two live objects of
/// the same region can share an offset.
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
