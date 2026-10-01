// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/*
 * This software was developed as part of the Horizon Europe programme funded project OpenCUBE
 * (Grant agreement: 101092984) horizon-opencube.eu
 */

#include "eckit/io/fam/FamList.h"

#include <cstddef>
#include <ostream>
#include <string>
#include <utility>

#include "eckit/config/LibEcKit.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/io/fam/FamListIterator.h"
#include "eckit/io/fam/FamObject.h"
#include "eckit/io/fam/FamRegion.h"
#include "eckit/io/fam/FamRegionName.h"
#include "eckit/io/fam/FamTypes.h"
#include "eckit/io/fam/detail/FamBackoff.h"
#include "eckit/io/fam/detail/FamListNode.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

FamList::FamList(FamRegion region, const Descriptor& desc) :
    region_{std::move(region)},
    head_{region_.proxyObject(FamDescriptor::unpack(desc.head))},
    tail_{region_.proxyObject(FamDescriptor::unpack(desc.tail))},
    size_{region_.proxyObject(FamDescriptor::unpack(desc.size))} {
    // A zero link is the null address, so a descriptor carrying one was never published.
    ASSERT_MSG(desc.head != 0 && desc.tail != 0 && desc.size != 0, "FamList: incomplete descriptor");
}

FamList::FamList(FamRegion region, const std::string& list_name) :
    region_{std::move(region)},
    head_{region_.ensureObject(sizeof(FamListNode), list_name + "h")},
    tail_{region_.ensureObject(sizeof(FamListNode), list_name + "t")},
    size_{region_.ensureObject(sizeof(size_type), list_name + "s")} {
    // set head's next to tail's prev (idempotent)
    if (FamListNode::getNextPacked(head_) == 0) {
        head_.put(tail_.descriptor().pack(), FamListNode::nextOff());
    }
    // set tail's prev to head's next (idempotent)
    if (FamListNode::getPrevPacked(tail_) == 0) {
        tail_.put(head_.descriptor().pack(), FamListNode::prevOff());
    }
}

auto FamList::descriptor() const -> Descriptor {
    return {head_.descriptor().pack(), tail_.descriptor().pack(), size_.descriptor().pack()};
}

//----------------------------------------------------------------------------------------------------------------------
// iterators

auto FamList::begin() const -> iterator {
    return region_.proxyObject(FamListNode::getNext(head_));
}

auto FamList::cbegin() const -> const_iterator {
    return region_.proxyObject(FamListNode::getNext(head_));
}

auto FamList::end() const -> iterator {
    return region_.proxyObject(tail_.descriptor());
}

auto FamList::cend() const -> const_iterator {
    return region_.proxyObject(tail_.descriptor());
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

    const auto new_address  = new_object.descriptor().pack();
    const auto head_address = head_.descriptor().pack();

    // 2. Link into list: use CAS-loop (Compare-And-Swap) to atomically update head.next
    //    This ensures the new node becomes visible to other readers
    fam::detail::CasBackoff backoff;
    while (true) {
        // Get current first node (what head.next points to)
        const auto first_address = FamListNode::getNextPacked(head_);
        auto first_object        = region_.proxyObject(FamDescriptor::unpack(first_address));

        // Point new node backward to head
        new_object.put(head_address, FamListNode::prevOff());

        // Point new node forward to current first node
        new_object.put(first_address, FamListNode::nextOff());

        // Atomically update head.next to new node.
        // On success, we become the new first node.
        const auto old_address = head_.compareSwap(FamListNode::nextOff(), first_address, new_address);
        if (old_address == first_address) {
            // Success! Update old first node's prev to point to us.
            // Use CAS instead of plain put to avoid overwriting a concurrent
            // pushBack's CAS on tail.prev (when first_object is the tail sentinel).
            first_object.compareSwap(FamListNode::prevOff(), head_address, new_address);

            // Atomically increment size
            size_.add(0, size_type{1});

            return;
        }
        // CAS failed, another thread modified head.next. Back off before retry.
        backoff();
    }
}

void FamList::pushBack(const void* data, const size_type length) {
    // 1. Allocate new node with data
    auto new_object = region_.allocateObject(sizeof(FamListNode) + length);
    new_object.put(length, offsetof(FamListNode, length));
    new_object.put(data, sizeof(FamListNode), length);

    const auto new_address  = new_object.descriptor().pack();
    const auto tail_address = tail_.descriptor().pack();

    // 2. Link into list: use CAS-loop to atomically update tail.prev
    //    This ensures new node becomes visible to other readers
    fam::detail::CasBackoff backoff;
    while (true) {
        // Get current last node (what tail.prev points to)
        const auto last_address = FamListNode::getPrevPacked(tail_);
        auto last_object        = region_.proxyObject(FamDescriptor::unpack(last_address));

        // Point new node forward to tail
        new_object.put(tail_address, FamListNode::nextOff());

        // Point new node backward to current last node
        new_object.put(last_address, FamListNode::prevOff());

        // Atomically update tail.prev to new node.
        // On success, we become the new last node.
        const auto old_address = tail_.compareSwap(FamListNode::prevOff(), last_address, new_address);
        if (old_address == last_address) {
            // Success! Now link new_object into the forward chain.
            // Use CAS-loop: walk forward from last_object to find the node whose
            // next is tail, then CAS its next to new_object.
            // This prevents the plain-put race with concurrent pushFront on head.next.
            fam::detail::CasBackoff inner_backoff;
            auto current = std::move(last_object);
            while (true) {
                const auto cur_next = FamListNode::getNextPacked(current);
                if (cur_next == tail_address) {
                    const auto old = current.compareSwap(FamListNode::nextOff(), tail_address, new_address);
                    if (old == tail_address) {
                        break;  // Successfully linked into forward chain
                    }
                    // CAS failed — another node was inserted. Follow the new link.
                    current.replaceWith(FamDescriptor::unpack(old));
                    inner_backoff();
                }
                else {
                    // Follow forward chain to find the node just before tail
                    current.replaceWith(FamDescriptor::unpack(cur_next));
                }
            }

            // Atomically increment size
            size_.add(0, size_type{1});

            return;
        }
        // CAS failed, another thread modified tail.prev. Back off before retry.
        backoff();
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Wait-Free Deletion (Logical + Physical)

void FamList::popFront() {
    ASSERT(!empty());

    const auto head_address = head_.descriptor().pack();
    const auto tail_address = tail_.descriptor().pack();

    fam::detail::CasBackoff backoff;
    while (true) {
        // Get the first node to delete
        const auto first_address = FamListNode::getNextPacked(head_);
        auto first_object        = region_.proxyObject(FamDescriptor::unpack(first_address));

        // Safety check: don't delete the tail sentinel
        if (first_address == tail_address) {
            return;  // Already empty
        }

        // 1. Logically mark the node as deleted (wait-free flag)
        FamListNode::mark(first_object);

        // 2. Get the next node after the one we're deleting
        const auto next_address = FamListNode::getNextPacked(first_object);

        // 3. Atomically update head.next to skip over the marked node
        const auto old_address = head_.compareSwap(FamListNode::nextOff(), first_address, next_address);
        if (old_address == first_address) {
            // Success! We've removed the node from the list.
            // Update the next node's prev pointer to point to head
            auto next_object = region_.proxyObject(FamDescriptor::unpack(next_address));
            next_object.put(head_address, FamListNode::prevOff());

            // Decrement size
            size_.subtract(0, size_type{1});

            // Node is marked and unlinked but NOT deallocated.
            // Concurrent iterators may still hold the node's address and
            // follow its next/prev pointers, which remain valid.
            // Physical reclamation happens on region wipe or clear().
            return;
        }
        // CAS failed, another thread modified head.next. Back off before retry.
        backoff();
    }
}

void FamList::popBack() {
    ASSERT(!empty());

    const auto head_address = head_.descriptor().pack();
    const auto tail_address = tail_.descriptor().pack();

    fam::detail::CasBackoff backoff;
    while (true) {
        // Get the last node to delete
        const auto last_address = FamListNode::getPrevPacked(tail_);
        auto last_object        = region_.proxyObject(FamDescriptor::unpack(last_address));

        // Safety check: don't delete the head sentinel
        if (last_address == head_address) {
            return;  // Already empty
        }

        // 1. Logically mark the node as deleted (wait-free flag)
        FamListNode::mark(last_object);

        // 2. Get the previous node
        const auto prev_address = FamListNode::getPrevPacked(last_object);

        // 3. Atomically update tail.prev to point before the marked node
        const auto old_address = tail_.compareSwap(FamListNode::prevOff(), last_address, prev_address);
        if (old_address == last_address) {
            // Success! We've removed the node from the list.
            // Update the previous node's next pointer to point to tail
            auto prev_object = region_.proxyObject(FamDescriptor::unpack(prev_address));
            prev_object.put(tail_address, FamListNode::nextOff());

            // Decrement size
            size_.subtract(0, size_type{1});

            // Node is marked and unlinked but NOT deallocated.
            // See popFront() for rationale.
            return;
        }
        // CAS failed, another thread modified tail.prev. Back off before retry.
        backoff();
    }
}

auto FamList::erase(iterator pos) -> iterator {
    const auto& object        = pos.object();
    const auto object_address = object.descriptor().pack();
    ASSERT(object_address != tail_.descriptor().pack());

    fam::detail::CasBackoff backoff;
    while (true) {
        // 1. Mark the node for deletion
        FamListNode::mark(object);

        // 2. Get next and prev pointers
        const auto next_address = FamListNode::getNextPacked(object);
        const auto prev_address = FamListNode::getPrevPacked(object);

        auto next_object = region_.proxyObject(FamDescriptor::unpack(next_address));
        auto prev_object = region_.proxyObject(FamDescriptor::unpack(prev_address));

        // 3. Atomically update prev.next to skip over marked node
        const auto old_next = prev_object.compareSwap(FamListNode::nextOff(), object_address, next_address);
        if (old_next == object_address) {
            // Success! Update next.prev as well
            next_object.put(prev_address, FamListNode::prevOff());

            // Update size
            size_.subtract(0, size_type{1});

            // Node is marked and unlinked but NOT deallocated.
            // See popFront() for rationale.

            return region_.proxyObject(FamDescriptor::unpack(next_address));
        }
        // CAS failed, back off before retry.
        backoff();
    }
}

//----------------------------------------------------------------------------------------------------------------------

void FamList::clear() {
    const auto head_address = head_.descriptor().pack();
    const auto tail_address = tail_.descriptor().pack();

    while (true) {
        const auto first_address = FamListNode::getNextPacked(head_);
        if (first_address == tail_address) {
            break;  // empty
        }

        auto first_object       = region_.proxyObject(FamDescriptor::unpack(first_address));
        const auto next_address = FamListNode::getNextPacked(first_object);
        auto next_object        = region_.proxyObject(FamDescriptor::unpack(next_address));

        // this is single-threaded, no CAS / mark needed
        head_.put(next_address, FamListNode::nextOff());
        next_object.put(head_address, FamListNode::prevOff());

        first_object.deallocate();
        size_.subtract(0, size_type{1});
    }
}

void FamList::deallocate() {
    clear();
    head_.deallocate();
    tail_.deallocate();
    size_.deallocate();
}

//----------------------------------------------------------------------------------------------------------------------
// capacity

auto FamList::size() const -> size_type {
    return size_.get<size_type>();
}

bool FamList::empty() const {
    // A node is the first real element if it's the next of head and not tail
    return FamListNode::getNextPacked(head_) == tail_.descriptor().pack();
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
