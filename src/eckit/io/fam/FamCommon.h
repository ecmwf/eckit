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

/// @file   FamCommon.h
/// @author Metin Cakircali
/// @date   Mar 2026

#pragma once

#include <sys/types.h>  // mode_t

#include <cstdint>  // uint64_t

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

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
