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

#include "eckit/io/fam/FamMapIterator.h"

#include <cstring>
#include <utility>

#include "eckit/exception/Exceptions.h"
#include "eckit/io/fam/FamMap.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

template <typename T>
FamMapIterator<T>::FamMapIterator(const FamMap& map, const std::size_t bucket, const bool advance) :
    map_{&map}, bucket_{bucket} {
    if (advance) {
        advanceToNextBucket();
    }
}

template <typename T>
FamMapIterator<T>::FamMapIterator(const FamMap& map, const std::size_t bucket, FamListIterator iter, FamList list) :
    map_{&map}, bucket_{bucket}, list_{std::move(list)}, iter_{std::move(iter)} {}

//----------------------------------------------------------------------------------------------------------------------

template <typename T>
bool FamMapIterator<T>::hasMoreBuckets() const {
    return bucket_ < FamMap::bucket_count;
}

template <typename T>
bool FamMapIterator<T>::loadBucket() {
    auto bucket = map_->getBucket(bucket_);
    if (!bucket || bucket->empty()) {
        list_.reset();
        iter_.reset();
        return false;
    }
    list_ = std::move(*bucket);
    iter_ = list_->begin();
    // empty: false, non-empty: true
    return iter_ != list_->end();
}

template <typename T>
void FamMapIterator<T>::advanceToNextBucket() {
    while (hasMoreBuckets()) {
        if (loadBucket()) {
            return;  // found a non-empty bucket with entries
        }
        ++bucket_;
    }
    // we reached the end
    list_.reset();
    iter_.reset();
}

template <typename T>
FamMapIterator<T>& FamMapIterator<T>::operator++() {
    ASSERT(hasMoreBuckets());
    ASSERT(iter_.has_value());

    // advance within current bucket
    ++(*iter_);

    if (*iter_ != list_->end()) {
        return *this;  // entry found in this bucket
    }

    // move to next bucket
    ++bucket_;
    advanceToNextBucket();

    return *this;
}

template <typename T>
bool FamMapIterator<T>::operator==(const FamMapIterator& other) const {
    // Same bucket and same position within bucket
    if (bucket_ != other.bucket_) {
        return false;
    }
    // Neither has a list iterator — both are at the same (empty or past-end) position
    if (!iter_.has_value() && !other.iter_.has_value()) {
        return true;
    }
    // Both have list iterators — compare underlying FAM objects
    if (iter_.has_value() && other.iter_.has_value()) {
        return iter_->object() == other.iter_->object();
    }
    return false;
}

template <typename T>
T FamMapIterator<T>::operator*() {
    ASSERT(iter_.has_value());
    return T{**iter_};
}

//----------------------------------------------------------------------------------------------------------------------

// Explicit instantiations
template class FamMapIterator<FamMapEntry<32>>;
template class FamMapConstIterator<FamMapEntry<32>>;

template class FamMapIterator<FamMapEntry<64>>;
template class FamMapConstIterator<FamMapEntry<64>>;

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
