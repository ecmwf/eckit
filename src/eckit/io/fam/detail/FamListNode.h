// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/*
 * This software was developed as part of the Horizon Europe programme funded project OpenCUBE
 * (Grant agreement: 101092984) horizon-opencube.eu
 */

/// @file   FamListNode.h
/// @author Metin Cakircali
/// @date   Mar 2024

#pragma once

#include <cstddef>
#include <cstdint>
#include <type_traits>

#include "eckit/io/Buffer.h"
#include "eckit/io/fam/FamObject.h"
#include "eckit/io/fam/detail/FamNode.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

/// @brief Concurrent-safe list node for FAM-resident doubly-linked list.
///
/// Layout (40+ bytes):
///  - version:   u8  - reserved, see "ABA" below
///  - next:      u64 - packed FamDescriptor of the next node
///  - prev:      u64 - packed FamDescriptor of the previous node
///  - length:    u64 - data payload length in bytes
///  - marked:    u8  - logical deletion marker (for wait-free deletion)
///  - padding:   u7  - alignment
///  - [data]:    next node allocation starts here
///
/// Links are stored packed into a single 64-bit word so they can be published with one atomic compare-and-swap; see
/// FamDescriptor::pack() for why the region id must travel with the offset.
///
/// Thread safety:
///  - The 'marked' bit enables logical deletion: physical deallocation happens asynchronously to avoid disrupting
///    other readers
///  - compareSwap() on the packed next/prev links coordinates insertions
///  - Readers can safely traverse marked nodes; they'll eventually be cleaned up
///
/// ABA: not currently possible, because unlinked nodes are never physically reclaimed while the list is live — only
/// clear() and region teardown free them, and both require quiescence. The 'version' field is reserved for a future
/// reclaiming allocator; nothing reads it today. Note that a packed link word has no room for a tag, so tagged
/// pointers are not an option here.
///
/// @important: DO NOT add any virtual functions in this class.
struct FamListNode {
    FamNode header{};        // 16 bytes: { version, next }
    fam::index_t prev{0};    //  8 bytes: packed FamDescriptor
    fam::size_t length{0};   //  8 bytes
    std::uint8_t marked{0};  //  1 byte: 0=active, 1=logically deleted
    // 7 bytes padding to align 'data' to 8-byte boundary

    /// Byte offset of the packed `prev` link within FamListNode.
    static constexpr std::size_t prevOff() noexcept { return offsetof(FamListNode, prev); }

    /// Byte offset of the packed `next` link within FamListNode.
    /// Equivalent to FamNode::nextOff() because `header` is placed first.
    static constexpr std::size_t nextOff() noexcept { return offsetof(FamListNode, header) + FamNode::nextOff(); }

    /// Forwarders to the embedded FamNode header.
    static FamDescriptor getNext(const FamObject& object) { return FamNode::getNext(object); }
    static fam::index_t getNextPacked(const FamObject& object) { return FamNode::getNextPacked(object); }

    /// Fetch previous node address
    static FamDescriptor getPrev(const FamObject& object) { return FamDescriptor::unpack(getPrevPacked(object)); }

    /// Fetch previous node address, still packed
    static fam::index_t getPrevPacked(const FamObject& object) { return object.get<fam::index_t>(prevOff()); }

    /// Fetch data payload length
    static fam::size_t getLength(const FamObject& object) {
        return object.get<fam::size_t>(offsetof(FamListNode, length));
    }

    /// Check if node is logically deleted (marked for removal)
    static bool isMarked(const FamObject& object) {
        return object.get<std::uint8_t>(offsetof(FamListNode, marked)) != 0;
    }

    /// Mark node as logically deleted (wait-free deletion)
    static void mark(const FamObject& object) {
        object.put(static_cast<std::uint8_t>(1), offsetof(FamListNode, marked));
    }

    /// Copy node data to buffer
    static void getData(const FamObject& object, Buffer& buffer) {
        if (const auto length = getLength(object); length > 0) {
            buffer.resize(length);
            object.get(buffer.data(), sizeof(FamListNode), length);
        }
    }
};

static_assert(std::is_standard_layout_v<FamListNode>, "FamListNode must be standard-layout for offsetof()");
static_assert(std::is_trivially_copyable_v<FamListNode>, "FamListNode must be trivially copyable for FAM put/get");
static_assert(sizeof(FamListNode) == 40, "FamListNode layout changed (FAM on-wire format depends on this)");
static_assert(offsetof(FamListNode, header) == 0, "FamListNode::header must be at offset 0 (wire layout)");
static_assert(offsetof(FamListNode, prev) == 16, "FamListNode::prev offset mismatch");
static_assert(offsetof(FamListNode, length) == 24, "FamListNode::length offset mismatch");
static_assert(offsetof(FamListNode, marked) == 32, "FamListNode::marked offset mismatch");
static_assert(offsetof(FamListNode, prev) % sizeof(fam::index_t) == 0,
              "FamListNode::prev must be 8-byte aligned for CAS");

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
