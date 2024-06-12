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

#include "eckit/io/fam/FamList.h"

#include "detail/FamNode.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/io/fam/FamName.h"
#include "eckit/io/fam/FamObject.h"
#include "eckit/io/fam/FamRegion.h"

#include <iostream>
#include <string>

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

FamList::FamList(const FamRegion& region, const std::string& listName):
    region_ {region}, head_ {initSentinel(listName + "-head", sizeof(FamNode))},
    tail_ {initSentinel(listName + "-tail", sizeof(FamNode))},
    size_ {initSentinel(listName + "-size", sizeof(fam::size_t))} {
    // set head's next to tail's prev
    if (FamNode::getNextOffset(head_) == 0) { head_.put(tail_.descriptor(), offsetof(FamNode, next)); }
    // set tail's prev to head's next
    if (FamNode::getPrevOffset(tail_) == 0) { tail_.put(head_.descriptor(), offsetof(FamNode, prev)); }
}

FamList::FamList(const FamName& name): FamList(name.lookupRegion(), name.nameObject()) { }

FamList::~FamList() = default;

auto FamList::initSentinel(const std::string& name, const fam::size_t size) const -> FamObject {
    try {
        return region_.allocateObject(size, name);
    } catch (const AlreadyExists&) { return region_.lookupObject(name); }
}

//----------------------------------------------------------------------------------------------------------------------
// iterators

auto FamList::begin() const -> iterator {
    return {region_.proxyObject(FamNode::getNextOffset(head_))};
}

auto FamList::cbegin() const -> const_iterator {
    return {region_.proxyObject(FamNode::getNextOffset(head_))};
}

auto FamList::end() const -> iterator {
    return {region_.proxyObject(tail_.offset())};
}

auto FamList::cend() const -> const_iterator {
    return {region_.proxyObject(tail_.offset())};
}

//----------------------------------------------------------------------------------------------------------------------
// accessors

auto FamList::front() const -> Buffer {
    return std::move(*begin());
}

auto FamList::back() const -> Buffer {
    return std::move(*--end());
}

//----------------------------------------------------------------------------------------------------------------------
// modifiers

void FamList::push_front(const void* /* data */, const fam::size_t /* length */) {
    NOTIMP;
}

void FamList::push_back(const void* data, const fam::size_t length) {
    // allocate an object
    auto newObject = region_.allocateObject(sizeof(FamNode) + length);

    // set new object's next to tail
    newObject.put(tail_.descriptor(), offsetof(FamNode, next));

    // set tail's prev to new object
    const auto prevOffset = tail_.swap(offsetof(FamNode, prev.offset), newObject.offset());

    const auto oldObject = region_.proxyObject(prevOffset);

    // set old object's next to new object
    oldObject.put(newObject.descriptor(), offsetof(FamNode, next));

    // set new object's prev to old object
    newObject.put(oldObject.descriptor(), offsetof(FamNode, prev));

    // finally the data
    newObject.put(length, offsetof(FamNode, length));
    newObject.put(data, sizeof(FamNode), length);

    // increment size
    size_.add(0, 1UL);
}

void FamList::pop_front() {
    NOTIMP;
}

void FamList::pop_back() {
    NOTIMP;
}

//----------------------------------------------------------------------------------------------------------------------
// capacity

auto FamList::size() const -> fam::size_t {
    return size_.get<fam::size_t>(0);
}

auto FamList::empty() const -> bool {
    return (FamNode::getNextOffset(head_) == tail_.offset());
}

//----------------------------------------------------------------------------------------------------------------------

void FamList::print(std::ostream& out) const {
    out << "FamList[size=" << size() << ", region=" << region_ << ", head=" << head_ << ", tail=" << tail_ << "]";
}

std::ostream& operator<<(std::ostream& out, const FamList& list) {
    list.print(out);
    return out;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
