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
#include "eckit/io/fam/FamListIterator.h"
#include "eckit/io/fam/FamObject.h"
#include "eckit/io/fam/FamProperty.h"
#include "eckit/io/fam/FamRegion.h"
#include "eckit/io/fam/FamRegionName.h"
#include "eckit/io/fam/detail/FamListNode.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

FamList::FamList(FamRegion region, const Descriptor& desc) :
    region_{std::move(region)},
    head_{region_.proxyObject(desc.head)},
    tail_{region_.proxyObject(desc.tail)},
    size_{region_.proxyObject(desc.size)},
    epoch_{region_.proxyObject(desc.epoch)} {
    ASSERT(region_.index() == desc.region);
}

FamList::FamList(FamRegion region, const std::string& list_name) :
    region_{std::move(region)},
    head_{region_.ensureObject(sizeof(FamListNode), list_name + "h")},
    tail_{region_.ensureObject(sizeof(FamListNode), list_name + "t")},
    size_{region_.ensureObject(sizeof(size_type), list_name + "s")},
    epoch_{region_.ensureObject(sizeof(std::uint64_t), list_name + "e")} {
    // set head's next to tail's prev (idempotent)
    if (FamListNode::getNextOffset(head_) == 0) {
        head_.put(tail_.descriptor(), offsetof(FamListNode, next));
    }
    // set tail's prev to head's next (idempotent)
    if (FamListNode::getPrevOffset(tail_) == 0) {
        tail_.put(head_.descriptor(), offsetof(FamListNode, prev));
    }
}

auto FamList::descriptor() const -> Descriptor {
    return {region_.index(), head_.offset(), tail_.offset(), size_.offset(), epoch_.offset()};
}

//----------------------------------------------------------------------------------------------------------------------
// iterators

auto FamList::begin() const -> iterator {
    return region_.proxyObject(FamListNode::getNextOffset(head_));
}

auto FamList::cbegin() const -> const_iterator {
    return region_.proxyObject(FamListNode::getNextOffset(head_));
}

auto FamList::end() const -> iterator {
    return region_.proxyObject(tail_.offset());
}

auto FamList::cend() const -> const_iterator {
    return region_.proxyObject(tail_.offset());
}

//----------------------------------------------------------------------------------------------------------------------
// accessors

auto FamList::front() const -> value_type {
    ASSERT(!empty());
    return std::move(*begin());
}

auto FamList::back() const -> value_type {
    ASSERT(!empty());
    return std::move(*--end());
}

//----------------------------------------------------------------------------------------------------------------------
// Lock-Free Insertion

void FamList::pushFront(const void* data, const size_type length) {
    // 1. Allocate new node with data
    auto new_object = region_.allocateObject(sizeof(FamListNode) + length);
    new_object.put(length, offsetof(FamListNode, length));
    new_object.put(data, sizeof(FamListNode), length);

    // 2. Link into list: use CAS-loop (Compare-And-Swap) to atomically update head.next
    //    This ensures the new node becomes visible to other readers
    while (true) {
        // Get current first node (what head.next points to)
        const auto first_offset = FamListNode::getNextOffset(head_);
        auto first_object       = region_.proxyObject(first_offset);

        // Point new node backward to head
        new_object.put(head_.descriptor(), offsetof(FamListNode, prev));

        // Point new node forward to current first node
        new_object.put(first_object.descriptor(), offsetof(FamListNode, next));

        // Atomically update head.next to new node.
        // On success, we become the new first node.
        const auto old_offset =
            head_.compareSwap(offsetof(FamListNode, next.offset), first_offset, new_object.offset());
        if (old_offset == first_offset) {
            // Success! Update old first node's prev to point to us.
            // Use CAS instead of plain put to avoid overwriting a concurrent
            // pushBack's CAS on tail.prev (when first_object is the tail sentinel).
            first_object.compareSwap(offsetof(FamListNode, prev.offset), head_.offset(), new_object.offset());

            // Atomically increment size
            size_.add(0, size_type{1});

            // Increment epoch to invalidate old iterators (optional, for ABA safety)
            epoch_.add(0, std::uint64_t{1});
            return;
        }
        // CAS failed, another thread modified head.next. Retry with updated first offset.
    }
}

void FamList::pushBack(const void* data, const size_type length) {
    // 1. Allocate new node with data
    auto new_object = region_.allocateObject(sizeof(FamListNode) + length);
    new_object.put(length, offsetof(FamListNode, length));
    new_object.put(data, sizeof(FamListNode), length);

    // 2. Link into list: use CAS-loop to atomically update tail.prev
    //    This ensures new node becomes visible to other readers
    while (true) {
        // Get current last node (what tail.prev points to)
        const auto last_offset = FamListNode::getPrevOffset(tail_);
        auto last_object       = region_.proxyObject(last_offset);

        // Point new node forward to tail
        new_object.put(tail_.descriptor(), offsetof(FamListNode, next));

        // Point new node backward to current last node
        new_object.put(last_object.descriptor(), offsetof(FamListNode, prev));

        // Atomically update tail.prev to new node.
        // On success, we become the new last node.
        const auto old_offset = tail_.compareSwap(offsetof(FamListNode, prev.offset), last_offset, new_object.offset());
        if (old_offset == last_offset) {
            // Success! Now link new_object into the forward chain.
            // Use CAS-loop: walk forward from last_object to find the node whose
            // next is tail, then CAS its next to new_object.
            // This prevents the plain-put race with concurrent pushFront on head.next.
            auto current = std::move(last_object);
            while (true) {
                const auto cur_next = FamListNode::getNextOffset(current);
                if (cur_next == tail_.offset()) {
                    const auto old = current.compareSwap(
                        offsetof(FamListNode, next.offset), tail_.offset(), new_object.offset());
                    if (old == tail_.offset()) {
                        break;  // Successfully linked into forward chain
                    }
                    // CAS failed — another node was inserted. Follow the new link.
                    current.replaceWith({region_.index(), old});
                }
                else {
                    // Follow forward chain to find the node just before tail
                    current.replaceWith({region_.index(), cur_next});
                }
            }

            // Atomically increment size
            size_.add(0, size_type{1});

            // Increment epoch (for iterator validation)
            epoch_.add(0, std::uint64_t{1});
            return;
        }
        // CAS failed, another thread modified tail.prev. Retry with updated last offset.
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Wait-Free Deletion (Logical + Physical)

void FamList::popFront() {
    ASSERT(!empty());

    while (true) {
        // Get the first node to delete
        const auto first_offset = FamListNode::getNextOffset(head_);
        auto first_object       = region_.proxyObject(first_offset);

        // Safety check: don't delete the tail sentinel
        if (first_offset == tail_.offset()) {
            return;  // Already empty
        }

        // 1. Logically mark the node as deleted (wait-free flag)
        FamListNode::mark(first_object);

        // 2. Get the next node after the one we're deleting
        const auto next_offset = FamListNode::getNextOffset(first_object);

        // 3. Atomically update head.next to skip over the marked node
        const auto old_offset = head_.compareSwap(offsetof(FamListNode, next.offset), first_offset, next_offset);
        if (old_offset == first_offset) {
            // Success! We've removed the node from the list.
            // Update the next node's prev pointer to point to head
            auto next_object = region_.proxyObject(next_offset);
            next_object.put(head_.descriptor(), offsetof(FamListNode, prev));

            // Decrement size
            size_.subtract(0, size_type{1});

            // Increment epoch for iterator validation
            epoch_.add(0, std::uint64_t{1});

            // Now deallocate the marked node (safe since we've unlinked it)
            first_object.deallocate();
            return;
        }
        // CAS failed, another thread modified head.next. Retry
    }
}

void FamList::popBack() {
    ASSERT(!empty());

    while (true) {
        // Get the last node to delete
        const auto last_offset = FamListNode::getPrevOffset(tail_);
        auto last_object       = region_.proxyObject(last_offset);

        // Safety check: don't delete the head sentinel
        if (last_offset == head_.offset()) {
            return;  // Already empty
        }

        // 1. Logically mark the node as deleted (wait-free flag)
        FamListNode::mark(last_object);

        // 2. Get the previous node
        const auto prev_offset = FamListNode::getPrevOffset(last_object);

        // 3. Atomically update tail.prev to point before the marked node
        const auto old_offset = tail_.compareSwap(offsetof(FamListNode, prev.offset), last_offset, prev_offset);
        if (old_offset == last_offset) {
            // Success! We've removed the node from the list.
            // Update the previous node's next pointer to point to tail
            auto prev_object = region_.proxyObject(prev_offset);
            prev_object.put(tail_.descriptor(), offsetof(FamListNode, next));

            // Decrement size
            size_.subtract(0, size_type{1});

            // Increment epoch
            epoch_.add(0, std::uint64_t{1});

            // Deallocate the marked node
            last_object.deallocate();
            return;
        }
        // CAS failed, another thread modified tail.prev. Retry
    }
}

auto FamList::erase(iterator pos) -> iterator {
    const auto& object = pos.object();
    ASSERT(object.offset() != tail_.offset());

    while (true) {
        // 1. Mark the node for deletion
        FamListNode::mark(object);

        // 2. Get next and prev pointers
        const auto next_offset = FamListNode::getNextOffset(object);
        const auto prev_offset = FamListNode::getPrevOffset(object);

        auto next_object = region_.proxyObject(next_offset);
        auto prev_object = region_.proxyObject(prev_offset);

        // 3. Atomically update prev.next to skip over marked node
        const auto old_next = prev_object.compareSwap(offsetof(FamListNode, next.offset), object.offset(), next_offset);
        if (old_next == object.offset()) {
            // Success! Update next.prev as well
            next_object.put(prev_object.descriptor(), offsetof(FamListNode, prev));

            // Update size and epoch
            size_.subtract(0, size_type{1});
            epoch_.add(0, std::uint64_t{1});

            // Deallocate marked node
            object.deallocate();

            return region_.proxyObject(next_offset);
        }
        // CAS failed, retry
    }
}

//----------------------------------------------------------------------------------------------------------------------
// capacity

auto FamList::size() const -> size_type {
    return size_.get<size_type>();
}

bool FamList::empty() const {
    // A node is the first real element if it's the next of head and not tail
    const auto first_offset = FamListNode::getNextOffset(head_);
    return first_offset == tail_.offset();
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
