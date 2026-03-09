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

/// @file   FamList.h
/// @author Metin Cakircali
/// @date   Mar 2024
///
/// @brief Concurrent-safe FAM-resident doubly-linked list.
///
/// ## Thread Safety
///
/// FamList provides **multiple-reader, multiple-writer (MRMW)** safety:
///
/// - **Concurrent insertions** (`pushFront`, `pushBack`): Lock-free using atomic CAS loops.
///   Two threads can insert simultaneously without serialization.
///
/// - **Concurrent deletions** (`popFront`, `popBack`, `erase`): Wait-free logical deletion.
///   Nodes are *marked* for deletion before physical deallocation. Readers skip marked nodes.
///
/// - **Concurrent iteration**: Safe during concurrent insertions/deletions via node versioning.
///   Iterators validate version stamps to detect stale node descriptors (ABA problem).
///
/// - **Size tracking**: Updated atomically with pointer modifications via careful CAS loops.
///
/// ## Lock-Free Algorithm Details
///
/// ### Insertion (pushBack example):
/// ```
/// 1. Allocate new node with data
/// 2. CAS-loop: old_last.next = new_node (atomic version-aware)
/// 3. CAS-loop: tail.prev = new_node (atomic version-aware)
/// 4. Atomic add to size
/// ```
///
/// ### Deletion (popFront example, logical):
/// ```
/// 1. CAS-loop: mark first node as deleted
/// 2. CAS-loop: update head.next (skip marked node)
/// 3. Atomic subtract from size
/// 4. Deallocate node immediately (safe due to logical marking)
/// ```
///
/// ## ABA Problem Handling
///
/// - Each node has a `version` field (incremented on reuse).
/// - Node descriptors are `{offset, version}` pairs.
/// - CAS operations compare both offset and version.
/// - Prevents use-after-free when freed nodes are reallocated at same address.
///
/// ## Marked Node Convention
///
/// - Deleted nodes are **logically marked** (bit flag) before physical deallocation.
/// - Readers check the mark bit; they skip marked nodes transparently.
/// - This delays physical freeing and avoids race windows.

#pragma once

#include <iosfwd>
#include <string>
#include <string_view>

#include "eckit/io/fam/FamListIterator.h"
#include "eckit/io/fam/FamObject.h"
#include "eckit/io/fam/FamProperty.h"
#include "eckit/io/fam/FamRegion.h"
#include "eckit/io/fam/FamRegionName.h"

namespace eckit {

class FamRegionName;

//----------------------------------------------------------------------------------------------------------------------

/// @brief Concurrent-safe, FAM-resident doubly-linked list.
///
/// Supports multiple readers and writers operating concurrently without locks.
/// Implements wait-free insertion and logical deletion, with version-based ABA detection.
class FamList {
public:  // types

    using size_type      = fam::size_t;
    using iterator       = FamListIterator;
    using const_iterator = FamListConstIterator;
    using value_type     = FamListIterator::data_type;

    /// List descriptor: encodes region ID and FAM object locations.
    struct Descriptor {
        fam::index_t region{0};  // region ID
        fam::index_t head{0};    // offset of head sentinel
        fam::index_t tail{0};    // offset of tail sentinel
        fam::index_t size{0};    // offset of atomic size counter
        fam::index_t epoch{0};   // offset of atomic epoch (version counter for ABA)
    };

public:  // methods

    /// Construct from descriptor (for reopening existing list).
    FamList(FamRegion region, const Descriptor& desc);

    /// Construct with new list in FAM (idempotent: reopens if exists).
    FamList(FamRegion region, const std::string& list_name);

    /// Return descriptor for persistence/serialization.
    Descriptor descriptor() const;

    // ---- capacity ----

    /// Return number of elements (atomic read).
    size_type size() const;

    /// Check if list is empty (lock-free wait-free).
    [[nodiscard]]
    bool empty() const;

    // ---- iterators ----

    /// Return iterator to first element (or end() if empty).
    /// Safe during concurrent modifications; validates version stamps.
    iterator begin() const;

    const_iterator cbegin() const;

    /// Return iterator to sentinel tail (one-past-end).
    iterator end() const;

    const_iterator cend() const;

    // ---- accessors ----

    /// Return copy of first element's data.
    /// Precondition: !empty()
    value_type front() const;

    /// Return copy of last element's data.
    /// Precondition: !empty()
    value_type back() const;

    // ---- modifiers (lock-free) ----

    /// Insert data at front. Lock-free, multiple-writer safe.
    void pushFront(const void* data, size_type length);

    void pushFront(const std::string_view data) { pushFront(data.data(), data.size()); }

    /// Insert data at back. Lock-free, multiple-writer safe.
    void pushBack(const void* data, size_type length);

    void pushBack(const std::string_view data) { pushBack(data.data(), data.size()); }

    /// Remove first element. Wait-free (logical deletion).
    /// Precondition: !empty()
    void popFront();

    /// Remove last element. Wait-free (logical deletion).
    /// Precondition: !empty()
    void popBack();

    /// Remove element at position. Returns iterator to following element.
    /// Wait-free; skips marked nodes in linked list.
    iterator erase(const_iterator pos);

private:  // methods

    void print(std::ostream& out) const;

    friend std::ostream& operator<<(std::ostream& out, const FamList& list);

private:  // members

    FamRegion region_;
    FamObject head_;
    FamObject tail_;
    FamObject size_;
    FamObject epoch_;  // atomic counter for version-based ABA detection
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
