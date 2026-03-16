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

/// @file   FamListIterator.h
/// @author Metin Cakircali
/// @date   Mar 2024
///
/// @brief Concurrent-safe bidirectional iterator for FamList.
///
/// ## Thread Safety
///
/// Iterators are safe to use during concurrent insertions and deletions:
///
/// - **Marked Node Skipping**: When a node is logically deleted (marked), iterators
///   transparently skip over it on `++` and `--` operations.
/// - **Lock-Free Traversal**: Iteration does not require any synchronization.
/// - **Consistency**: Each iterator snapshot sees a consistent view of the list
///   at the moment the iterator was created, with logical deletions becoming visible
///   as nodes are skipped.

#pragma once

#include <iterator>
#include <optional>

#include "eckit/io/Buffer.h"
#include "eckit/io/fam/FamObject.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------
// ITERATOR

/// @brief Concurrent-safe forward and bidirectional iterator for FamList.
///
/// Advances through the list, automatically skipping marked (deleted) nodes.
/// Safe to use while other threads insert/delete elements.
class FamListIterator {
public:  // types

    using iterator_category = std::bidirectional_iterator_tag;

    using value_type = FamObject;
    using pointer    = value_type*;
    using reference  = value_type&;
    using data_type  = Buffer;

public:  // methods

    /// Construct iterator wrapping a FAM object.
    FamListIterator(value_type object);  // NOLINT(google-explicit-constructor)

    /// Advance to next node (skip marked nodes).
    FamListIterator& operator++();

    /// Retreat to previous node (skip marked nodes).
    FamListIterator& operator--();

    /// Compare iterators for equality.
    bool operator==(const FamListIterator& other) const;

    /// Compare iterators for inequality.
    bool operator!=(const FamListIterator& other) const { return !operator==(other); }

    /// Access the underlying FAM object (to read node metadata).
    pointer operator->();

    /// Dereference to return data payload as Buffer.
    data_type& operator*();

    /// Access underlying FAM object.
    value_type& object() { return object_; }

    /// Access underlying FAM object (const).
    const value_type& object() const { return object_; }

private:  // members

    value_type object_;

    std::optional<data_type> buffer_;
};

//----------------------------------------------------------------------------------------------------------------------
// CONST ITERATOR

/// @brief Const variant of FamListIterator.
class FamListConstIterator : public FamListIterator {
public:  // types

    using pointer   = const value_type*;
    using reference = const data_type&;

public:  // methods

    using FamListIterator::FamListIterator;

    pointer operator->() { return FamListIterator::operator->(); }

    reference operator*() { return FamListIterator::operator*(); }
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
