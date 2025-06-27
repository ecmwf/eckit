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

#pragma once

#include "eckit/io/fam/FamHashTable.h"
// #include "eckit/io/fam/FamMapIterator.h"

#include <iosfwd>
#include <string>

namespace eckit {

class FamRegionName;

//----------------------------------------------------------------------------------------------------------------------

/// @brief FamMap is an associative key-value container on FAM. Each element is organized depending on the
/// hash value of its key.
class FamMap {
    static constexpr auto keySize = 32;  // template?

    static constexpr auto capacity = 1024;

public:  // types

    using key_type   = FixedString<keySize>;
    using hash_type  = FamHash<key_type>;
    using value_type = char;
    // using key_equal  = key_equal;
    using size_type       = fam::size_t;
    using difference_type = size_type;

    // using mapped_type    = mapped_type;
    // using allocator_type = allocator_type;
    // using pointer        = pointer;
    // using const_pointer  = const_pointer;

    using reference       = value_type&;
    using const_reference = const value_type&;

    using iterator       = FamMapIterator;
    using const_iterator = FamMapConstIterator;

    // using local_iterator       = local_iterator;
    // using const_local_iterator = const_local_iterator;

    using node_type = FamList;

    // using insert_return_type = std::pair<iterator, bool>;

public:  // methods

    FamMap(const FamRegionName& regionName, const std::string& tableName);

    ~FamMap() = default;

    // capacity

    auto size() const -> size_type;

    auto empty() const -> bool;

    auto max_size() const noexcept -> size_type { return capacity; }

    // iterators

    auto begin() const -> iterator;

    auto cbegin() const -> const_iterator;

    auto end() const -> iterator;

    auto cend() const -> const_iterator;

    // lookup

    // Returns reference to the element with specified key.
    // throws std::out_of_range if not found
    auto at(const key_type& key) -> reference;
    auto at(const key_type& key) const -> const_reference;

    // operator[] ?

    // size_type count( const Key& key ) const;

    auto find(const key_type& key) -> iterator;
    auto find(const key_type& key) const -> const_iterator;

    auto contains(const key_type& key) const -> bool;

    // modifiers

    auto insert(const value_type& value) -> iterator;
    auto insert(value_type&& value) -> iterator;

    // void push_back(const void* data, size_type length);
    // void push_front(const void* data, size_type length);
    // void pop_front();
    // void pop_back();

    // void clear() noexcept;

private:  // methods

    auto initSentinel(const std::string& name, size_type size) const -> FamObject;

    void print(std::ostream& out) const;

    friend std::ostream& operator<<(std::ostream& out, const FamMap& list);

private:  // members

    FamRegion region_;
    FamObject root_;
    FamObject table_;
    FamObject count_;

    //     FamHashTable table_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
