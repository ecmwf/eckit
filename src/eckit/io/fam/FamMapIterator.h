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

/// @file   FamMapIterator.h
/// @author Metin Cakircali
/// @date   Jul 2024

/// @brief Forward iterator for FamMap that walks all entries across all hash buckets.
///
/// ## Iteration Order
///
/// The iterator visits entries in bucket order (0 → bucket_count-1). Within each bucket,
/// entries are visited in FamList insertion order. Empty buckets are skipped.
///
/// ## Thread Safety
///
/// Safe during concurrent insertions and deletions. Relies on FamList iterator's
/// marked-node skipping to handle concurrent deletions transparently.

#pragma once

#include <cstddef>
#include <iterator>
#include <optional>

#include "eckit/io/fam/FamList.h"
#include "eckit/io/fam/FamMapEntry.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

template <typename T>
class FamMap;

/// @brief Forward iterator for FamMap.
///
/// Walks bucket 0..N and within each non-empty bucket walks the FamList entries.
/// Dereference returns FamMapEntry{key, value} by value (data lives in FAM).
template <typename T>
class FamMapIterator {

    static_assert(IsFamMapEntry<T>::value, "FamMapIterator only supports T = FamMapEntry<...>");

public:  // types

    using iterator_category = std::forward_iterator_tag;
    using value_type        = T;
    using difference_type   = std::ptrdiff_t;

public:  // methods

    /// Construct begin/end iterator for the map.
    /// @param map     The owning FamMap.
    /// @param bucket  Bucket index (bucket_count for end).
    /// @param advance If true, advances to the first non-empty entry.
    FamMapIterator(const FamMap<T>& map, std::size_t bucket, bool advance);

    /// Construct iterator pointing to a specific entry in a specific bucket.
    FamMapIterator(const FamMap<T>& map, std::size_t bucket, FamListIterator iter, FamList list);

    FamMapIterator(const FamMapIterator&)            = delete;
    FamMapIterator& operator=(const FamMapIterator&) = delete;
    FamMapIterator(FamMapIterator&&)                 = default;
    FamMapIterator& operator=(FamMapIterator&&)      = default;

    ~FamMapIterator() = default;

    /// Advance to next entry. Crosses bucket boundaries.
    FamMapIterator& operator++();

    /// Compare iterators.
    bool operator==(const FamMapIterator& other) const;
    bool operator!=(const FamMapIterator& other) const { return !operator==(other); }

    /// Dereference: returns FamMapEntry{key, value} by value.
    value_type operator*();

private:  // methods

    /// Advance bucket_ to the next non-empty bucket and load it.
    void advanceToNextBucket();

    /// Load the bucket FamList at the bucket_ and position iter_ to its begin().
    /// Returns false if bucket is empty.
    bool loadBucket();

    /// Check if there's more buckets.
    bool hasMoreBuckets() const;

private:  // members

    const FamMap<T>* map_;
    std::size_t bucket_;
    std::optional<FamList> list_;
    std::optional<FamListIterator> iter_;
};

//----------------------------------------------------------------------------------------------------------------------

/// @brief Const variant of FamMapIterator, for use in const contexts and range-based for loops.
/// Otherwise identical to FamMapIterator, as it already returns entries by value.
template <typename T>
using FamMapConstIterator = FamMapIterator<T>;

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
