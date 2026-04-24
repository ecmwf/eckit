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

/// @file   FamNode.h
/// @author Metin Cakircali
/// @date   Mar 2024

#pragma once

#include <cstddef>
#include <cstdint>  // uint8_t
#include <type_traits>

#include "eckit/io/fam/FamObject.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

/// @important: DO NOT add any virtual functions in this class.
struct FamNode {
    std::uint8_t version{1};  // 1 byte
    FamDescriptor next;

    static FamDescriptor getNext(const FamObject& object) { return object.get<FamDescriptor>(offsetof(FamNode, next)); }

    static std::uint64_t getNextOffset(const FamObject& object) {
        return object.get<std::uint64_t>(offsetof(FamNode, next.offset));
    }
};

static_assert(std::is_standard_layout_v<FamNode>, "FamNode must be standard-layout for offsetof()");
static_assert(std::is_trivially_copyable_v<FamNode>, "FamNode must be trivially copyable for FAM put/get");
static_assert(sizeof(FamNode) == 24, "FamNode layout changed — FAM on-wire format depends on this");

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
