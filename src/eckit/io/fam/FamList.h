// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

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
/// - **Concurrent insertions** (`pushFront`, `pushBack`): Lock-free using atomic CAS loops. Two threads can insert
///   simultaneously without serialization.
///
/// - **Concurrent deletions** (`popFront`, `popBack`, `erase`): Wait-free logical deletion. Nodes are *marked* for
///   deletion and unlinked, but not freed. Readers skip marked nodes.
///
/// - **Concurrent iteration**: Safe during concurrent insertions and deletions, because an unlinked node keeps valid
///   next/prev links for as long as the list is live.
///
/// - **Size tracking**: Updated atomically with pointer modifications via careful CAS loops.
///
/// ## Addressing
///
/// Links are packed {regionId, offset} pairs in a single 64-bit word, which is the widest atomic OpenFAM offers.
/// @see FamDescriptor::pack()
///
/// ## Lock-Free Algorithm Details
///
/// ### Insertion (pushBack example):
/// ```
/// 1. Allocate new node with data
/// 2. CAS-loop: tail.prev = new_node
/// 3. CAS-loop: walk forward to the node whose next is tail, then old_last.next = new_node
/// 4. Atomic add to size
/// ```
///
/// ### Deletion (popFront example, logical):
/// ```
/// 1. Mark first node as deleted
/// 2. CAS-loop: update head.next (skip marked node)
/// 3. Atomic subtract from size
/// 4. Node is left allocated; see "Reclamation"
/// ```
///
/// ## Reclamation
///
/// Logically deleted nodes are never freed while the list is live — only `clear()` and region teardown reclaim them,
/// and both require quiescence. This is what makes concurrent traversal safe, and it is also why no ABA protection is
/// needed: a node address is never reused behind a concurrent reader.
///
/// ## Marked Node Convention
///
/// - Deleted nodes are **logically marked** (bit flag) and unlinked.
/// - Readers check the mark bit; they skip marked nodes transparently.

#pragma once

#include <iosfwd>
#include <string>
#include <string_view>

#include "eckit/io/fam/FamListIterator.h"
#include "eckit/io/fam/FamObject.h"
#include "eckit/io/fam/FamRegion.h"
#include "eckit/io/fam/FamTypes.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

/// @brief Concurrent-safe, FAM-resident doubly-linked list.
///
/// Supports multiple readers and writers operating concurrently without locks.
/// Implements lock-free insertion and wait-free logical deletion.
class FamList {
public:  // types

    using size_type      = fam::size_t;
    using iterator       = FamListIterator;
    using const_iterator = FamListConstIterator;
    using value_type     = FamListIterator::data_type;

    /// List descriptor: packed {regionId, offset} addresses of the list's FAM objects.
    /// @see FamDescriptor::pack()
    struct Descriptor {
        fam::index_t head{0};  // head sentinel
        fam::index_t tail{0};  // tail sentinel
        fam::index_t size{0};  // atomic size counter
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
    [[nodiscard]] bool empty() const;

    // ---- iterators ----

    /// Return iterator to first element (or end() if empty).
    /// Safe during concurrent modifications; skips logically deleted nodes.
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

    void pushFront(std::string_view data) { pushFront(data.data(), data.size()); }

    void pushFront(const Buffer& data) { pushFront(data.view()); }

    /// Insert data at back. Lock-free, multiple-writer safe.
    void pushBack(const void* data, size_type length);

    void pushBack(std::string_view data) { pushBack(data.data(), data.size()); }

    void pushBack(const Buffer& data) { pushBack(data.view()); }

    /// Remove first element. Lock-free (logical mark + unlink).
    /// The node is NOT deallocated; concurrent iterators may still
    /// reference it. Physical reclamation happens on region wipe or clear().
    /// Precondition: !empty()
    void popFront();

    /// Remove last element. Lock-free (logical mark + unlink).
    /// The node is NOT deallocated; see popFront() for rationale.
    /// Precondition: !empty()
    void popBack();

    /// Remove element at position. Returns iterator to following element.
    /// The node is NOT deallocated; see popFront() for rationale.
    iterator erase(iterator pos);

    /// Reclaims the FAM memory immediately. Deallocate all data nodes and reset the list to empty.
    /// @pre  No concurrent readers or writers.
    void clear();

    /// Deallocate the list. The list must not be used again afterwards.
    /// @pre  No concurrent readers or writers.
    void deallocate();

private:  // methods

    void print(std::ostream& out) const;

    friend std::ostream& operator<<(std::ostream& out, const FamList& list);

private:  // members

    FamRegion region_;
    FamObject head_;
    FamObject tail_;
    FamObject size_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
