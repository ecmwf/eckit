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

#include <cstddef>
#include <ostream>
#include <string>
#include <utility>

#include "eckit/exception/Exceptions.h"
#include "eckit/io/Buffer.h"
#include "eckit/io/fam/FamListIterator.h"
#include "eckit/io/fam/FamObject.h"
#include "eckit/io/fam/FamProperty.h"
#include "eckit/io/fam/FamRegion.h"
#include "eckit/io/fam/FamRegionName.h"
#include "eckit/io/fam/detail/FamListNode.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

namespace {

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

FamList::FamList(const FamRegion& region, const Descriptor& desc) :
    region_{region},
    head_{region_.proxyObject(desc.head)},
    tail_{region_.proxyObject(desc.tail)},
    size_{region_.proxyObject(desc.size)} {
    ASSERT(region.index() == desc.region);
}

FamList::FamList(const FamRegion& region, const std::string& list_name) :
    region_{region},
    head_{initSentinel(region_, list_name + "-list-head", sizeof(FamListNode))},
    tail_{initSentinel(region_, list_name + "-list-tail", sizeof(FamListNode))},
    size_{initSentinel(region_, list_name + "-list-size", sizeof(size_type))} {
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

FamList::Descriptor FamList::descriptor() const {
    return {region_.index(), head_.offset(), tail_.offset(), size_.offset()};
}

//----------------------------------------------------------------------------------------------------------------------
// iterators

FamList::iterator FamList::begin() const {
    return region_.proxyObject(FamListNode::getNextOffset(head_));
}

FamList::const_iterator FamList::cbegin() const {
    return region_.proxyObject(FamListNode::getNextOffset(head_));
}

FamList::iterator FamList::end() const {
    return region_.proxyObject(tail_.offset());
}

FamList::const_iterator FamList::cend() const {
    return region_.proxyObject(tail_.offset());
}

//----------------------------------------------------------------------------------------------------------------------
// accessors

Buffer FamList::front() const {
    ASSERT(!empty());
    return std::move(*begin());
}

Buffer FamList::back() const {
    ASSERT(!empty());
    return std::move(*--end());
}

//----------------------------------------------------------------------------------------------------------------------
// modifiers

void FamList::pushFront(const void* data, const size_type length) {
    // allocate an object
    auto new_object = region_.allocateObject(sizeof(FamListNode) + length);

    // set new object's previous to head
    new_object.put(head_.descriptor(), offsetof(FamListNode, prev));

    // set head's next to new object
    const auto prev_offset = head_.swap(offsetof(FamListNode, next.offset), new_object.offset());
    const auto old_object  = region_.proxyObject(prev_offset);

    // set old object's prev to new object
    old_object.put(new_object.descriptor(), offsetof(FamListNode, prev));
    // set new object's next to old object
    new_object.put(old_object.descriptor(), offsetof(FamListNode, next));

    // finally put the data
    new_object.put(length, offsetof(FamListNode, length));
    new_object.put(data, sizeof(FamListNode), length);

    // increment size
    size_.add(0, 1UL);
}

void FamList::pushBack(const void* data, const size_type length) {
    // allocate an object
    auto new_object = region_.allocateObject(sizeof(FamListNode) + length);

    // set new object's next to tail
    new_object.put(tail_.descriptor(), offsetof(FamListNode, next));

    // set tail's prev to new object
    const auto prev_offset = tail_.swap(offsetof(FamListNode, prev.offset), new_object.offset());
    const auto old_object  = region_.proxyObject(prev_offset);

    // set old object's next to new object
    old_object.put(new_object.descriptor(), offsetof(FamListNode, next));
    // set new object's prev to old object
    new_object.put(old_object.descriptor(), offsetof(FamListNode, prev));

    // finally put the data
    new_object.put(length, offsetof(FamListNode, length));
    new_object.put(data, sizeof(FamListNode), length);

    // increment size
    size_.add(0, 1UL);
}

void FamList::popFront() {
    NOTIMP;
}

void FamList::popBack() {
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
