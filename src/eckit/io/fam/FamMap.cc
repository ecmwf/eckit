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

#include "eckit/io/fam/FamMap.h"

#include <cstddef>
#include <cstring>
#include <ostream>
#include <string>
#include <thread>
#include <utility>

#include "eckit/exception/Exceptions.h"
#include "eckit/io/Buffer.h"
#include "eckit/io/fam/FamList.h"
#include "eckit/io/fam/FamMapIterator.h"
#include "eckit/io/fam/FamObject.h"
#include "eckit/io/fam/FamRegion.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------
// Helpers

namespace {

/// Offset of the head field within a FamList::Descriptor.
constexpr fam::size_t bucketOffset(std::size_t index) {
    return static_cast<fam::size_t>(index * sizeof(FamList::Descriptor));
}

/// Offset of the head field within a FamList::Descriptor.
constexpr fam::size_t bucketHeadOffset(std::size_t index) {
    return bucketOffset(index) + offsetof(FamList::Descriptor, head);
}

FamObject initSentinel(const FamRegion& region, const std::string& object_name, const fam::size_t object_size) {
    try {
        return region.allocateObject(object_size, object_name);
    }
    catch (const AlreadyExists&) {
        auto object = region.lookupObject(object_name);
        ASSERT(object.size() == object_size);
        return object;
    }
}

}  // namespace

//----------------------------------------------------------------------------------------------------------------------

template <typename T>
FamMap<T>::FamMap(std::string name, FamRegion region) :
    name_{std::move(name)},
    region_{std::move(region)},
    table_{initSentinel(region_, name_ + table_suffix, bucket_count * sizeof(FamList::Descriptor))},
    count_{initSentinel(region_, name_ + count_suffix, sizeof(size_type))} {}

//----------------------------------------------------------------------------------------------------------------------
// Bucket management

template <typename T>
FamListIterator FamMap<T>::findInBucket(const FamList& bucket, const key_type& key) {
    for (auto iter = bucket.begin(); iter != bucket.end(); ++iter) {
        const auto& buffer = *iter;
        if (buffer.size() >= key_size && entry_type::decodeKey(buffer) == key) {
            return iter;
        }
    }
    return bucket.end();
}

template <typename T>
fam::size_t FamMap<T>::getBucketHead(const std::size_t index) const {
    return table_.get<fam::size_t>(bucketHeadOffset(index));
}

template <typename T>
FamList::Descriptor FamMap<T>::getBucketDescriptor(const std::size_t index) const {
    return table_.get<FamList::Descriptor>(bucketOffset(index));
}

template <typename T>
std::optional<FamList> FamMap<T>::getBucket(const std::size_t index) const {
    if (const auto head = getBucketHead(index); head == 0 || head == creating) {
        return {};
    }
    return FamList{region_, getBucketDescriptor(index)};
}

template <typename T>
FamList FamMap<T>::getOrCreateBucket(const std::size_t index) {
    // bucket already exists
    if (auto bucket = getBucket(index)) {
        return std::move(*bucket);
    }

    // Try to claim the bucket via CAS: 0 -> CREATING
    const auto old_head = table_.compareSwap(bucketHeadOffset(index), fam::size_t{0}, creating);

    if (old_head == 0) {
        // We claimed the bucket. Create a new FamList bucket.
        // Use short name to stay within OpenFAM dataitem name limits (40 chars).
        // Format: "{map_name}.{index}"
        const auto bucket_name = name_ + "." + std::to_string(index);
        auto bucket            = FamList{region_, bucket_name};
        auto desc              = bucket.descriptor();

        // Write remaining descriptor fields FIRST (tail, size, epoch)
        const auto offset = static_cast<fam::size_t>(index * sizeof(FamList::Descriptor));
        table_.put(desc.region, offset + offsetof(FamList::Descriptor, region));
        table_.put(desc.tail, offset + offsetof(FamList::Descriptor, tail));
        table_.put(desc.size, offset + offsetof(FamList::Descriptor, size));
        table_.put(desc.epoch, offset + offsetof(FamList::Descriptor, epoch));

        // Write head LAST to "publish" the bucket (transitions from CREATING → real offset)
        table_.put(desc.head, bucketHeadOffset(index));

        return bucket;
    }

    // Another proc/thread is creating this bucket. Spin until head is valid.
    // Limit retries to detect a crashed creator (avoid infinite spin).
    static constexpr int max_spin = 100000;
    auto head                     = old_head;
    for (int spin = 0; head == 0 || head == creating; ++spin) {
        ASSERT_MSG(spin < max_spin, "FamMap::getOrCreateBucket: bucket creation stalled (creator may have crashed)");
        std::this_thread::yield();
        head = table_.get<fam::size_t>(bucketHeadOffset(index));
    }

    return {region_, getBucketDescriptor(index)};
}

//----------------------------------------------------------------------------------------------------------------------
// Iterators

template <typename T>
auto FamMap<T>::begin() const -> iterator {
    return {*this, 0, true};
}

template <typename T>
auto FamMap<T>::cbegin() const -> const_iterator {
    return {*this, 0, true};
}

template <typename T>
auto FamMap<T>::end() const -> iterator {
    return {*this, bucket_count, false};
}

template <typename T>
auto FamMap<T>::cend() const -> const_iterator {
    return {*this, bucket_count, false};
}

//----------------------------------------------------------------------------------------------------------------------
// Lookup

template <typename T>
auto FamMap<T>::find(const key_type& key) const -> iterator {
    const auto index = bucketIndex(key);

    auto bucket = getBucket(index);
    if (!bucket) {
        return end();
    }

    auto iter = findInBucket(*bucket, key);
    if (iter == bucket->end()) {
        return end();
    }

    return {*this, index, std::move(iter), std::move(*bucket)};
}

template <typename T>
bool FamMap<T>::contains(const key_type& key) const {
    const auto bucket = getBucket(bucketIndex(key));
    if (bucket) {
        return findInBucket(*bucket, key) != bucket->end();
    }
    return false;
}

template <typename T>
auto FamMap<T>::count(const key_type& key) const -> size_type {
    if (const auto bucket = getBucket(bucketIndex(key))) {
        size_type result = 0;
        for (const auto& buffer : *bucket) {
            if (buffer.size() >= key_size && entry_type::decodeKey(buffer) == key) {
                ++result;
            }
        }
        return result;
    }
    return 0;
}

//----------------------------------------------------------------------------------------------------------------------
// Modifiers

template <typename T>
auto FamMap<T>::insert(const key_type& key, const void* data, const size_type length) -> std::pair<iterator, bool> {
    const auto index = bucketIndex(key);
    auto bucket      = getOrCreateBucket(index);

    /// Check if key already exists
    /// @note: This check-then-insert sequence is not atomic.
    /// concurrent inserts of the same key may both insert resulting duplicates.
    /// A per-bucket CAS lock would be needed for full MRMW uniqueness guarantees.
    auto iter = findInBucket(bucket, key);
    if (iter != bucket.end()) {
        return {iterator{*this, index, std::move(iter), std::move(bucket)}, false};
    }

    // Encode and insert into bucket list (lock-free via FamList::pushFront).
    // pushFront ensures the new entry is found first by findInBucket (head→tail scan),
    // consistent with forceInsert() and insertOrAssign().
    auto payload = entry_type::encode(key, data, length);
    bucket.pushFront(payload);

    // Atomically increment total count
    count_.add(0, size_type{1});

    // Re-find the entry to return a valid iterator
    auto new_it = findInBucket(bucket, key);
    return {iterator{*this, index, std::move(new_it), std::move(bucket)}, true};
}

template <typename T>
auto FamMap<T>::insertOrAssign(const key_type& key, const void* data, const size_type length)
    -> std::pair<iterator, bool> {
    const auto index = bucketIndex(key);
    auto bucket      = getOrCreateBucket(index);

    // 1. Insert new entry at the FRONT of the bucket
    // so, concurrent find() (which iterate head→tail) sees it
    auto payload = entry_type::encode(key, data, length);
    bucket.pushFront(payload);
    count_.add(0, size_type{1});

    // 2. Scan for old entry with the same key (the second occurrence) and erase it.
    //    findInBucket returns the first match, which is the entry we just inserted.
    //    We need to find and erase the next match, if any.
    bool found_first = false;
    bool replaced    = false;
    for (auto iter = bucket.begin(); iter != bucket.end(); ++iter) {
        const auto& buffer = *iter;
        if (buffer.size() >= key_size && entry_type::decodeKey(buffer) == key) {
            if (!found_first) {
                found_first = true;  // skip the newly inserted entry (first match)
                continue;
            }
            // This is the old entry — erase it and adjust count.
            bucket.erase(std::move(iter));
            count_.subtract(0, size_type{1});
            replaced = true;
            break;
        }
    }

    // 3. Return iterator to the new entry (first match from head).
    //    Second element: true if inserted (no prior entry), false if replaced.
    auto new_it = findInBucket(bucket, key);
    return {iterator{*this, index, std::move(new_it), std::move(bucket)}, !replaced};
}

template <typename T>
auto FamMap<T>::forceInsert(const key_type& key, const void* data, const size_type length) -> iterator {
    const auto index = bucketIndex(key);
    auto bucket      = getOrCreateBucket(index);

    auto payload = entry_type::encode(key, data, length);
    bucket.pushFront(payload);

    count_.add(0, size_type{1});

    auto new_it = findInBucket(bucket, key);
    return {*this, index, std::move(new_it), std::move(bucket)};
}

template <typename T>
auto FamMap<T>::erase(const key_type& key) -> size_type {
    const auto index = bucketIndex(key);
    auto bucket      = getBucket(index);
    if (!bucket) {
        return 0;
    }

    size_type removed = 0;
    for (auto iter = bucket->begin(); iter != bucket->end();) {
        const auto& buffer = *iter;
        if (buffer.size() >= key_size && entry_type::decodeKey(buffer) == key) {
            iter = bucket->erase(std::move(iter));
            ++removed;
        }
        else {
            ++iter;
        }
    }

    if (removed > 0) {
        count_.subtract(0, removed);
    }
    return removed;
}

template <typename T>
void FamMap<T>::clear() {
    for (std::size_t i = 0; i < bucket_count; ++i) {
        if (auto bucket = getBucket(i)) {
            while (!bucket->empty()) {
                bucket->popFront();
            }
        }
    }
    count_.set(0, size_type{0});
}

template <typename T>
void FamMap<T>::merge(const FamMap& other) {
    for (const auto& [key, value] : other) {
        insert(key, value);
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Capacity

template <typename T>
auto FamMap<T>::size() const -> size_type {
    return count_.get<size_type>();
}

template <typename T>
bool FamMap<T>::empty() const {
    return size() == 0;
}

template <typename T>
float FamMap<T>::loadFactor() const {
    return static_cast<float>(size()) / static_cast<float>(bucket_count);
}

//----------------------------------------------------------------------------------------------------------------------
// Output

template <typename T>
void FamMap<T>::print(std::ostream& out) const {
    out << "FamMap[name=" << name_ << ",key_size=" << key_size << ",size=" << size() << ",region=" << region_ << ']';
}

//----------------------------------------------------------------------------------------------------------------------

// Explicit instantiations
template class FamMap<FamMapEntry<32>>;
template class FamMap<FamMapEntry<64>>;
template class FamMap<FamMapEntry<128>>;

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
