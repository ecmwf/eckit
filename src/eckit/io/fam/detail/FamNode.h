// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

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
    std::uint8_t version{1};  // 1 byte (+7 padding), reserved; see FamListNode "ABA"
    fam::index_t next{0};     // 8 bytes: packed FamDescriptor of the next node

    /// Byte offset of the packed `next` link within FamNode.
    static constexpr std::size_t nextOff() noexcept { return offsetof(FamNode, next); }

    static fam::index_t getNextPacked(const FamObject& object) {
        return object.get<fam::index_t>(offsetof(FamNode, next));
    }

    static FamDescriptor getNext(const FamObject& object) { return FamDescriptor::unpack(getNextPacked(object)); }
};

static_assert(std::is_standard_layout_v<FamNode>, "FamNode must be standard-layout for offsetof()");
static_assert(std::is_trivially_copyable_v<FamNode>, "FamNode must be trivially copyable for FAM put/get");
static_assert(sizeof(FamNode) == 16, "FamNode layout changed (FAM on-wire format depends on this)");
static_assert(offsetof(FamNode, next) % sizeof(fam::index_t) == 0, "FamNode::next must be 8-byte aligned for CAS");

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
