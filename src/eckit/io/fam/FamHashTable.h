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

/// @file   FamHashTable.h
/// @author Metin Cakircali
/// @date   Jul 2024

#pragma once

#include "eckit/io/fam/FamObject.h"
#include "eckit/io/fam/FamRegion.h"
// #include "eckit/io/fam/FamRegionName.h"
// #include "eckit/io/fam/FamVector.h"
#include "eckit/io/fam/FamMapIterator.h"
#include "eckit/types/FixedString.h"

#include <array>
#include <string>

namespace eckit {

class FamList;
// class FamRegion;
class FamRegionName;

//----------------------------------------------------------------------------------------------------------------------
// FAM HASHER

/// @brief Hash functor. Override this to make a specialized hasher
template<typename key_type>
struct FamHash {
    auto operator()(const key_type& key) const noexcept -> std::size_t {
        return std::hash<std::string> {}(key.asString());
        /// @note example for a 3-level key
        // const auto l1 = std::hash<std::string> {}(key.firstLevel);
        // const auto l2 = std::hash<std::string> {}(key.secondLevel);
        // const auto l3 = std::hash<std::string> {}(key.thirdLevel);
        // return l1 ^ (l2 ^ (l3 << 1));
    }
};

//----------------------------------------------------------------------------------------------------------------------

/// @todo template: initial table size, key size, (also equal and/or hasher ?)

/// @brief data structure is array of lists: FamVector< FamList >
//     unsigned int index = key % table->size;

class FamHashTable {
    static constexpr auto keySize = 32;  // template?

    static constexpr auto capacity = 1024;

public:  // types
    using key_type  = FixedString<keySize>;
    using hash_type = FamHash<key_type>;
    /// @todo char array ?
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
    using const_iterator = const FamMapIterator;

    // using local_iterator       = local_iterator;
    // using const_local_iterator = const_local_iterator;

    using node_type = FamList;

public:  // methods
    FamHashTable(const FamRegionName& regionName, const std::string& tableName);

private:  // methods
    auto initSentinel(const std::string& name, fam::size_t size) const -> FamObject;

private:  // members
    FamRegion region_;

    FamObject begin_;
    FamObject count_;

    std::array<node_type, capacity> table_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
