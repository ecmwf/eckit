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

/// @file   FamMap.h
/// @author Metin Cakircali
/// @date   Jul 2024

/// @brief Concurrent-safe, FAM-resident unordered associative container.
///
/// ## Overview
///
/// FamMap is a hash-based key-value store residing entirely in Fabric-Attached Memory (FAM).
/// It provides an `std::unordered_map`-like interface with fixed types, using `FixedString<KeySize>` keys and
/// variable-length `Buffer` values.
///
/// ## Architecture
///
/// - **Hash table**: A flat FAM object holding `bucket_count` (1024) bucket slots. Each slot stores a
///   `FamList::Descriptor` (40 bytes). A zero `head` field means the bucket is empty.
/// - **Buckets**: Each non-empty bucket is a `FamList` whose nodes store key-value entries as:
///   `[key (32/64/128 bytes)] [value data (variable length)]`
/// - **Size counter**: An atomic FAM counter tracking total number of entries across all buckets.
///
/// ## Concurrency
///
/// Provides **multiple-reader, multiple-writer (MRMW)** safety:
///
/// - **Bucket creation**: When the first element hashes to an empty bucket, a new `FamList` is
///   created using a CAS-based protocol on the bucket slot's `head` field:
///   - CAS `head` from 0 → `CREATING` sentinel to claim the slot.
///   - Create `FamList`, write remaining descriptor fields, then write the real `head` value.
///   - Competing writers spin-wait on the sentinel until the bucket is ready.
///
/// - **Per-bucket operations**: Insertions, deletions, and lookups within a bucket delegate to
///   `FamList`, which provides lock-free insert and wait-free logical deletion.
///
/// - **Size tracking**: Updated atomically after each insert/erase.
///
/// ## Entry Layout in FamList Nodes
///
/// Each FamList node's data payload holds:
/// ```
/// | key (32/64/128 bytes, FixedString<32/64/128>) | value_data (node.length - key_size bytes) |
/// ```
///
/// ## Iterator
///
/// The iterator walks buckets 0..bucket_count-1, and within each non-empty bucket walks
/// the FamList elements. Dereferencing returns a `FamMapEntry{key, value}` by value.
/// Iterators are safe during concurrent modifications via FamList's marked-node skipping.

#pragma once

#include <cstddef>
#include <functional>
#include <iosfwd>
#include <optional>
#include <string>
#include <string_view>
#include <utility>

#include "eckit/io/Buffer.h"
#include "eckit/io/fam/FamList.h"
#include "eckit/io/fam/FamMapEntry.h"
#include "eckit/io/fam/FamMapIterator.h"
#include "eckit/io/fam/FamObject.h"
#include "eckit/io/fam/FamProperty.h"
#include "eckit/io/fam/FamRegion.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

/// @brief Hash functor for FamMap keys.
template <typename T>
struct FamHash {
    std::size_t operator()(const T& key) const noexcept { return std::hash<std::string>{}(key.asString()); }
};

//----------------------------------------------------------------------------------------------------------------------

/// @brief Concurrent-safe, FAM-resident unordered associative container.
///
/// Hash table with FamList buckets. Fixed key type `FixedString<KeySize>`, variable-length
/// `Buffer` values. Supports concurrent insert, find, erase, and iteration.
/// Iterators are forward-only and safe during concurrent modifications.
/// @tparam T Must be `FamMapEntry<KeySize>`, which defines key and value types and encoding.
template <typename T>
class FamMap {
    static_assert(IsFamMapEntry<T>::value, "FamMap only supports T = FamMapEntry<...>");

public:  // types

    using entry_type = T;
    using key_type   = typename entry_type::key_type;
    using value_type = typename entry_type::value_type;
    using hash_type  = FamHash<key_type>;
    using size_type  = fam::size_t;

    using iterator       = FamMapIterator<entry_type>;
    using const_iterator = FamMapConstIterator<entry_type>;

public:  // constants

    /// Key size in bytes (e.g. 32 for FixedString<32>).
    static constexpr auto key_size = entry_type::key_size;

    /// Number of buckets in the hash table. Chosen as a power of two for efficient modulo.
    static constexpr std::size_t bucket_count = 1024;

    /// needed for preventing concurrent double-init of buckets, as sentinel value in bucket head during creation
    static constexpr fam::size_t creating = ~fam::size_t{0};

    static constexpr auto table_suffix = "-map-table";
    static constexpr auto count_suffix = "-map-count";

public:  // methods

    /// Construct or open a FamMap in the given region with the given name.
    FamMap(std::string name, FamRegion region);

    /// rules
    FamMap(const FamMap&)            = delete;
    FamMap& operator=(const FamMap&) = delete;
    FamMap(FamMap&&)                 = default;
    FamMap& operator=(FamMap&&)      = default;

    ~FamMap() = default;

    // ---- capacity ----

    /// Return total number of entries across all buckets (atomic read).
    size_type size() const;

    /// Check if the map has no entries.
    [[nodiscard]]
    bool empty() const;

    // ---- iterators ----

    /// Return iterator to the first entry (across all buckets).
    iterator begin() const;

    const_iterator cbegin() const;

    /// Return past-the-end iterator.
    iterator end() const;

    const_iterator cend() const;

    // ---- lookup ----

    /// Find entry by key. Returns end() if not found.
    iterator find(const key_type& key) const;

    /// Check if an entry with the given key exists.
    bool contains(const key_type& key) const;

    // ---- modifiers (concurrent-safe) ----

    /// Insert a key-value pair. If the key already exists, no insertion is performed.
    /// Returns {iterator, true} on success, {iterator_to_existing, false} if key exists.
    /// @important: This check-then-insert sequence is not atomic.
    /// concurrent inserts of the same key may both insert resulting duplicates.
    std::pair<iterator, bool> insert(const key_type& key, const void* data, size_type length);

    /// Insert with string_view value.
    std::pair<iterator, bool> insert(const key_type& key, std::string_view data) {
        return insert(key, data.data(), data.size());
    }

    /// Insert with Buffer value.
    std::pair<iterator, bool> insert(const key_type& key, const Buffer& data) { return insert(key, data.view()); }

    /// Erase the entry with the given key. Returns 1 if erased, 0 if not found.
    size_type erase(const key_type& key);

    /// Remove all entries from all buckets.
    void clear();

private:  // methods

    friend class FamMapIterator<T>;

    /// Return the bucket index for a given key.
    static std::size_t bucketIndex(const key_type& key) { return hash_type{}(key) % bucket_count; }

    /// Find the list iterator pointing to the entry with given key in a bucket.
    /// Returns the bucket's end() if not found.
    static FamListIterator findInBucket(const FamList& bucket, const key_type& key);

    /// Get the head offset of the bucket at the given index.
    fam::size_t getBucketHead(std::size_t index) const;

    /// Read bucket descriptor at given index from the table.
    FamList::Descriptor getBucketDescriptor(std::size_t index) const;

    /// Get or create the FamList for a given bucket index. Thread-safe via CAS.
    FamList getOrCreateBucket(std::size_t index);

    /// Get existing bucket as FamList, or return nullopt if bucket is empty.
    std::optional<FamList> getBucket(std::size_t index) const;

    void print(std::ostream& out) const;

    friend std::ostream& operator<<(std::ostream& out, const FamMap<T>& map) {
        map.print(out);
        return out;
    }

private:  // members

    std::string name_;  ///< Map name (used for bucket list naming)
    FamRegion region_;  ///< FAM region holding all map objects
    FamObject table_;   ///< Flat array of FamList::Descriptor, one per bucket
    FamObject count_;   ///< Atomic uint64 tracking total element count
};

//----------------------------------------------------------------------------------------------------------------------

using FamMap32  = FamMap<FamMapEntry<32>>;
using FamMap64  = FamMap<FamMapEntry<64>>;
using FamMap128 = FamMap<FamMapEntry<128>>;

}  // namespace eckit
