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
#include "eckit/io/fam/FamObject.h"
#include "eckit/io/fam/FamRegion.h"

#include <fam/fam.h>

#include <iostream>
#include <list>
#include <string>

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

FamList::FamList(FamRegion::SPtr region, const std::string& name):
    region_ {std::move(region)}, head_ {initSentinel(name + "-head")}, tail_ {initSentinel(name + "-tail")} {
    // set head's next to tail's prev
    if (getNext(*head_).offset == 0) { head_->put(tail_->descriptor(), offsetof(FamNode, next)); }
    // set tail's prev to head's next
    if (getPrev(*tail_).offset == 0) { tail_->put(head_->descriptor(), offsetof(FamNode, prev)); }
}

FamList::~FamList() = default;

auto FamList::initSentinel(const std::string& name) -> FamObject::UPtr {
    try {
        return region_->allocateObject(sizeof(FamNode), name);
    } catch (const AlreadyExists&) { return region_->lookupObject(name); }
}

//----------------------------------------------------------------------------------------------------------------------

auto FamList::begin() -> iterator {
    return iterator(region_->proxyObject(getNext(*head_).offset));
}

auto FamList::end() -> iterator {
    return iterator(region_->proxyObject(tail_->descriptor().offset));
}

auto FamList::cbegin() -> const_iterator {
    return iterator(region_->proxyObject(getNext(*head_).offset));
}

auto FamList::cend() -> const_iterator {
    return iterator(region_->proxyObject(tail_->descriptor().offset));
}

void FamList::push_back(const void* data, const fam::size_t length) {
    // allocate an object
    auto newObject = region_->allocateObject(sizeof(FamNode) + length);

    // set new object's next to tail
    newObject->put(tail_->descriptor(), offsetof(FamNode, next));

    // set tail's prev to new object
    const auto prevOffset = tail_->swap(offsetof(FamNode, prev.offset), newObject->offset());

    const auto oldObject = region_->proxyObject(prevOffset);

    // set old object's next to new object
    oldObject->put(newObject->descriptor(), offsetof(FamNode, next));

    // set new object's prev to old object
    newObject->put(oldObject->descriptor(), offsetof(FamNode, prev));

    // finally the data
    newObject->put(length, offsetof(FamNode, length));
    newObject->put(data, sizeof(FamNode), length);
}

void FamList::pop_back() {
    NOTIMP;
}

void FamList::push_front(const void* /* data */, const fam::size_t /* length */) {
    NOTIMP;
}

void FamList::pop_front() {
    NOTIMP;
}

//----------------------------------------------------------------------------------------------------------------------

auto FamList::size() const -> fam::size_t {
    NOTIMP;
}

auto FamList::empty() const -> bool {
    return (head_->offset() == getPrev(*tail_).offset);
}

//----------------------------------------------------------------------------------------------------------------------

void FamList::print(std::ostream& out) const {
    out << *region_;
}

std::ostream& operator<<(std::ostream& out, const FamList& list) {
    list.print(out);
    return out;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
