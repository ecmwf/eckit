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

}  // namespace fam

struct FamDescriptor {
    fam::index_t region{0};
    fam::index_t offset{0};
};

static_assert(std::is_standard_layout_v<FamDescriptor>, "FamDescriptor must be standard-layout for offsetof()");
static_assert(std::is_trivially_copyable_v<FamDescriptor>, "FamDescriptor must be trivially copyable for FAM put/get");
static_assert(sizeof(FamDescriptor) == 16, "FamDescriptor layout changed (FAM on-wire format depends on this)");

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
