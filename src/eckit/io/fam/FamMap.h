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

#include <iosfwd>
#include <string>
#include <utility>

#include "eckit/io/fam/FamHashTable.h"
#include "eckit/io/fam/FamMapIterator.h"
#include "eckit/io/fam/FamObject.h"
#include "eckit/io/fam/FamProperty.h"
#include "eckit/io/fam/FamRegion.h"
#include "eckit/types/FixedString.h"

namespace eckit {

class FamRegionName;

//----------------------------------------------------------------------------------------------------------------------

/// @brief FamMap is an associative key-value container on FAM.
/// Each element is organized depending on the hash value of its key.
class FamMap {
    static constexpr auto key_size = 32;  // template?

    static constexpr auto capacity = 1024;

public:  // types

    using key_type    = FixedString<key_size>;
    using mapped_type = char;

    using value_type = std::pair<const key_type, mapped_type>;
    // using key_equal  = key_equal;
    using size_type       = fam::size_t;
    using difference_type = size_type;

    using hash_type = FamHash<key_type>;

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

    FamMap(FamRegion region, const std::string& name);

    /// TODO: check the rules
    FamMap(const FamMap&)            = delete;
    FamMap& operator=(const FamMap&) = delete;
    FamMap(FamMap&&)                 = delete;
    FamMap& operator=(FamMap&&)      = delete;

    ~FamMap() = default;

    // capacity

    size_type size() const;

    bool empty() const;

    // size_type maxSize() const noexcept { return capacity; }

    // iterators

    iterator begin() const;

    const_iterator cbegin() const;

    iterator end() const;

    const_iterator cend() const;

    // accessors

    // Returns reference to the element with specified key.
    // throws std::out_of_range if not found
    reference at(const key_type& key);
    const_reference at(const key_type& key) const;

    // operator[] ?

    // size_type count( const Key& key ) const;

    iterator find(const key_type& key);
    const_iterator find(const key_type& key) const;

    bool contains(const key_type& key) const;

    // modifiers

    iterator insert(const value_type& value);
    iterator insert(value_type&& value);

    size_type erase(const key_type& key);

    void clear() noexcept;

private:  // methods

    FamObject initSentinel(const std::string& name, size_type size) const;

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
