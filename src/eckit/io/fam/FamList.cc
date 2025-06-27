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

#include "detail/FamListNode.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/io/Buffer.h"
#include "eckit/io/fam/FamObject.h"
#include "eckit/io/fam/FamProperty.h"
#include "eckit/io/fam/FamRegion.h"
#include "eckit/io/fam/FamRegionName.h"
#include "eckit/io/fam/detail/FamNode.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

namespace {

auto initSentinel(const FamRegion& region, const std::string& objectName, const fam::size_t objectSize) -> FamObject {
    try {
        return region.allocateObject(objectSize, objectName);
    }
    catch (const AlreadyExists&) {
        auto object = region.lookupObject(objectName);
        ASSERT(object.size() == objectSize);
        return object;
    }
}

}  // namespace

//----------------------------------------------------------------------------------------------------------------------

FamList::FamList(const FamRegion& region, const Descriptor& desc) :
    region_{region},
    head_{region_.proxyObject(desc.head)},
    tail_{region_.proxyObject(desc.tail)},
    size_{region_.proxyObject(desc.size)} {
    ASSERT(region.index() == desc.region);
}

FamList::FamList(const FamRegion& region, const std::string& listName) :
    region_{region},
    head_{initSentinel(region_, listName + "-list-head", sizeof(FamListNode))},
    tail_{initSentinel(region_, listName + "-list-tail", sizeof(FamListNode))},
    size_{initSentinel(region_, listName + "-list-size", sizeof(size_type))} {
    // set head's next to tail's prev
    if (FamListNode::getNextOffset(head_) == 0) {
        head_.put(tail_.descriptor(), offsetof(FamListNode, next));
    }
    // set tail's prev to head's next
    if (FamListNode::getPrevOffset(tail_) == 0) {
        tail_.put(head_.descriptor(), offsetof(FamListNode, prev));
    }
}

FamList::FamList(const FamRegionName& name) : FamList(name.lookup(), name.path().objectName) {}

auto FamList::descriptor() const -> Descriptor {
    return {region_.index(), head_.offset(), tail_.offset(), size_.offset()};
}

//----------------------------------------------------------------------------------------------------------------------
// iterators

auto FamList::begin() const -> iterator {
    return {region_.proxyObject(FamListNode::getNextOffset(head_))};
}

auto FamList::cbegin() const -> const_iterator {
    return {region_.proxyObject(FamListNode::getNextOffset(head_))};
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

void FamList::push_front(const void* data, const size_type length) {
    // allocate an object
    auto newObject = region_.allocateObject(sizeof(FamListNode) + length);

    // set new object's previous to head
    newObject.put(head_.descriptor(), offsetof(FamListNode, prev));

    // set head's next to new object
    const auto prevOffset = head_.swap(offsetof(FamListNode, next.offset), newObject.offset());
    const auto oldObject  = region_.proxyObject(prevOffset);

    // set old object's prev to new object
    oldObject.put(newObject.descriptor(), offsetof(FamListNode, prev));
    // set new object's next to old object
    newObject.put(oldObject.descriptor(), offsetof(FamListNode, next));

    // finally put the data
    newObject.put(length, offsetof(FamListNode, length));
    newObject.put(data, sizeof(FamListNode), length);

    // increment size
    size_.add(0, 1UL);
}

void FamList::push_back(const void* data, const size_type length) {
    // allocate an object
    auto newObject = region_.allocateObject(sizeof(FamListNode) + length);

    // set new object's next to tail
    newObject.put(tail_.descriptor(), offsetof(FamListNode, next));

    // set tail's prev to new object
    const auto prevOffset = tail_.swap(offsetof(FamListNode, prev.offset), newObject.offset());
    const auto oldObject  = region_.proxyObject(prevOffset);

    // set old object's next to new object
    oldObject.put(newObject.descriptor(), offsetof(FamListNode, next));
    // set new object's prev to old object
    newObject.put(oldObject.descriptor(), offsetof(FamListNode, prev));

    // finally put the data
    newObject.put(length, offsetof(FamListNode, length));
    newObject.put(data, sizeof(FamListNode), length);

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

auto FamList::size() const -> size_type {
    return size_.get<size_type>();
}

auto FamList::empty() const -> bool {
    return (FamListNode::getNextOffset(head_) == tail_.offset());
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
