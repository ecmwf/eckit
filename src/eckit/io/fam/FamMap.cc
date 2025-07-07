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

#include "detail/FamMapNode.h"

#include <iostream>
#include <string>

#include "eckit/exception/Exceptions.h"
#include "eckit/io/fam/FamObject.h"
#include "eckit/io/fam/FamRegion.h"
#include "eckit/io/fam/FamRegionName.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

FamMap::FamMap(const FamRegion& region, const std::string& table_name) :
    region_{region},
    root_{initSentinel(table_name + "-map-root", sizeof(FamMapNode))},
    table_{initSentinel(table_name + "-map-table", capacity * sizeof(FamMapNode))},
    count_{initSentinel(table_name + "-map-count", sizeof(size_type))} {}

auto FamMap::initSentinel(const std::string& object_name, const size_type object_size) const -> FamObject {
    try {
        return region_.allocateObject(object_size, object_name);
    }
    catch (const AlreadyExists&) {
        auto object = region_.lookupObject(object_name);
        ASSERT(object.size() == object_size);
        return object;
    }
}

//----------------------------------------------------------------------------------------------------------------------
// iterators

auto FamMap::begin() const -> iterator {
    return {region_, FamMapNode::getNextOffset(root_)};
}

auto FamMap::cbegin() const -> const_iterator {
    return {region_, FamMapNode::getNextOffset(root_)};
}

auto FamMap::end() const -> iterator {
    return {region_, 0};
}

auto FamMap::cend() const -> const_iterator {
    return {region_, 0};
}

//----------------------------------------------------------------------------------------------------------------------
// lookup

auto FamMap::at(const key_type& /* key */) -> reference {
    NOTIMP;
}

auto FamMap::at(const key_type& /* key */) const -> const_reference {
    NOTIMP;
}

auto FamMap::find(const key_type& /* key */) -> iterator {
    NOTIMP;
}

auto FamMap::find(const key_type& /* key */) const -> const_iterator {
    NOTIMP;
}

auto FamMap::contains(const key_type& /* key */) const -> bool {
    NOTIMP;
}

// auto FamMap::front() const -> Buffer {
//     return std::move(*begin());
// }
//
// auto FamMap::back() const -> Buffer {
//     return std::move(*--end());
// }

//----------------------------------------------------------------------------------------------------------------------
// modifiers

auto FamMap::insert(const value_type& /* value */) -> iterator {
    NOTIMP;
}

auto FamMap::insert(value_type&& /* value */) -> iterator {
    NOTIMP;
}

// void FamMap::push_back(const void* data, const fam::size_t length) {
//     // allocate an object
//     auto newObject = region_.allocateObject(sizeof(FamNode) + length);
//
//     // set new object's next to tail
//     newObject.put(tail_.descriptor(), offsetof(FamNode, next));
//
//     // set tail's prev to new object
//     const auto prevOffset = tail_.swap(offsetof(FamNode, prev.offset), newObject.offset());
//
//     const auto oldObject = region_.proxyObject(prevOffset);
//
//     // set old object's next to new object
//     oldObject.put(newObject.descriptor(), offsetof(FamNode, next));
//
//     // set new object's prev to old object
//     newObject.put(oldObject.descriptor(), offsetof(FamNode, prev));
//
//     // finally the data
//     newObject.put(length, offsetof(FamNode, length));
//     newObject.put(data, sizeof(FamNode), length);
//
//     // increment size
//     size_.add(0, 1UL);
// }

//----------------------------------------------------------------------------------------------------------------------
// capacity

auto FamMap::size() const -> size_type {
    return count_.get<size_type>();
}

auto FamMap::empty() const -> bool {
    return size() == 0;
}

//----------------------------------------------------------------------------------------------------------------------

void FamMap::print(std::ostream& out) const {
    out << "FamMap[capacity=" << capacity << ",region=" << region_ << ",root=" << root_ << ",count=" << count_ << ']';
}

std::ostream& operator<<(std::ostream& out, const FamMap& list) {
    list.print(out);
    return out;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
