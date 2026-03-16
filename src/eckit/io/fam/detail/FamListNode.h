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

/// @file   FamListNode.h
/// @author Metin Cakircali
/// @date   Mar 2024

#pragma once

#include <cstddef>
#include <cstdint>

#include "eckit/io/Buffer.h"
#include "eckit/io/fam/FamObject.h"
#include "eckit/io/fam/FamProperty.h"
#include "eckit/io/fam/detail/FamNode.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

/// @brief Concurrent-safe list node for FAM-resident doubly-linked list.
///
/// Layout (56+ bytes):
///  - version:   u8  - node version (incremented on reuse, for ABA detection)
///  - next:      FamDescriptor  - next node pointer
///  - prev:      FamDescriptor  - previous node pointer
///  - length:    u64 - data payload length in bytes
///  - marked:    u8  - logical deletion marker (for wait-free deletion)
///  - padding:   u7  - alignment
///  - [data]:    next node allocation starts here
///
/// Thread safety:
///  - Use version field to detect ABA problems when reusing freed nodes
///  - The 'marked' bit enables logical deletion: physical deallocation happens
///    asynchronously to avoid disrupting other readers
///  - Atomic swap() and compareSwap() on next/prev pointers coordinate insertions
///  - Readers can safely traverse marked nodes; they'll eventually be cleaned up
///
/// @important: DO NOT add any virtual functions in this class.
struct FamListNode : public FamNode {
    FamDescriptor prev;
    fam::size_t length{0};
    std::uint8_t marked{0};  // 0=active, 1=logically deleted (concurrent-safe marker)
    // 7 bytes padding to align 'data' to 8-byte boundary

    /// Increment version stamp to prevent ABA problems on node reuse
    static void bumpVersion(const FamObject& object) {
        auto ver = object.get<std::uint8_t>(offsetof(FamNode, version));
        object.put(static_cast<std::uint8_t>(ver + 1), offsetof(FamNode, version));
    }

    /// Fetch previous node descriptor (includes version for ABA detection)
    static FamDescriptor getPrev(const FamObject& object) {
        return object.get<FamDescriptor>(offsetof(FamListNode, prev));
    }

    /// Fetch previous node offset only
    static std::uint64_t getPrevOffset(const FamObject& object) {
        return object.get<std::uint64_t>(offsetof(FamListNode, prev.offset));
    }

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

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
