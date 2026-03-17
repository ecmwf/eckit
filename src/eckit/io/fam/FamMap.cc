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
#include "eckit/io/fam/FamProperty.h"
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
        // Use short name to stay within OpenFAM dataitem name limits (~40 chars).
        // Format: "{map_name}-b{index}"
        const auto bucket_name = name_ + "-b" + std::to_string(index);
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
    auto head = old_head;
    while (head == 0 || head == creating) {
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

    auto bucket_list = getBucket(index);
    if (!bucket_list) {
        return end();
    }

    auto iter = findInBucket(*bucket_list, key);
    if (iter == bucket_list->end()) {
        return end();
    }

    return {*this, index, std::move(iter), std::move(*bucket_list)};
}

template <typename T>
bool FamMap<T>::contains(const key_type& key) const {
    const auto bucket_list = getBucket(bucketIndex(key));
    if (bucket_list) {
        return findInBucket(*bucket_list, key) != bucket_list->end();
    }
    return false;
}

//----------------------------------------------------------------------------------------------------------------------
// Modifiers

template <typename T>
auto FamMap<T>::insert(const key_type& key, const void* data, const size_type length) -> std::pair<iterator, bool> {
    const auto index = bucketIndex(key);
    auto bucket      = getOrCreateBucket(index);

    // Check if key already exists
    auto iter = findInBucket(bucket, key);
    if (iter != bucket.end()) {
        return {iterator{*this, index, std::move(iter), std::move(bucket)}, false};
    }

    // Encode and insert into bucket list (lock-free via FamList::pushBack)
    auto payload = entry_type::encode(key, data, length);
    bucket.pushBack(payload);

    // Atomically increment total count
    count_.add(0, 1UL);

    // Re-find the entry to return a valid iterator
    auto new_it = findInBucket(bucket, key);
    return {iterator{*this, index, std::move(new_it), std::move(bucket)}, true};
}

template <typename T>
auto FamMap<T>::erase(const key_type& key) -> size_type {
    const auto index = bucketIndex(key);
    auto bucket      = getBucket(index);
    if (!bucket) {
        return 0;
    }

    auto iter = findInBucket(*bucket, key);
    if (iter == bucket->end()) {
        return 0;
    }

    bucket->erase(std::move(iter));
    count_.subtract(0, 1UL);
    return 1;
}

template <typename T>
void FamMap<T>::clear() {
    for (std::size_t i = 0; i < bucket_count; ++i) {
        auto bucket_list = getBucket(i);
        if (bucket_list) {
            while (!bucket_list->empty()) {
                bucket_list->popFront();
            }
        }
    }
    count_.set(0, size_type{0});
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
