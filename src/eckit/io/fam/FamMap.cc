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

#include <iostream>
#include <string>
#include <utility>

#include "eckit/exception/Exceptions.h"
#include "eckit/io/fam/FamObject.h"
#include "eckit/io/fam/FamRegion.h"
#include "eckit/io/fam/FamRegionName.h"
#include "eckit/io/fam/detail/FamMapNode.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

FamMap::FamMap(FamRegion region, const std::string& name) :
    region_{std::move(region)},
    root_{initSentinel(name + "-map-root", sizeof(FamMapNode))},
    table_{initSentinel(name + "-map-table", capacity * sizeof(FamMapNode))},
    count_{initSentinel(name + "-map-count", sizeof(size_type))} {}

FamObject FamMap::initSentinel(const std::string& object_name, const size_type object_size) const {
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

bool FamMap::contains(const key_type& /* key */) const {
    NOTIMP;
}

// Buffer FamMap::front() const {
//     return std::move(*begin());
// }
//
// Buffer FamMap::back() const {
//     return std::move(*--end());
// }

//----------------------------------------------------------------------------------------------------------------------
// modifiers

auto FamMap::insert(const value_type& value) -> iterator {}

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

auto FamMap::insert(value_type&& /* value */) -> iterator {
    NOTIMP;
}

//----------------------------------------------------------------------------------------------------------------------
// capacity

auto FamMap::size() const -> size_type {
    return count_.get<size_type>();
}

bool FamMap::empty() const {
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
