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

#include "eckit/io/fam/FamListIterator.h"

#include <utility>

#include "eckit/io/fam/detail/FamListNode.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------
// ITERATOR

FamListIterator::FamListIterator(value_type object) : object_{std::move(object)} {}

/// Advance to next node, skipping logically deleted (marked) nodes.
/// This ensures iterators remain consistent even during concurrent deletions.
FamListIterator& FamListIterator::operator++() {
    if (const auto next = FamListNode::getNext(object_); next.offset > 0) {
        object_.replaceWith(next);

        // Skip over marked (deleted) nodes - iterate until we find an unmarked node
        while (FamListNode::isMarked(object_)) {
            if (const auto next_next = FamListNode::getNext(object_); next_next.offset > 0) {
                object_.replaceWith(next_next);
            }
            else {
                break;  // No more nodes
            }
        }
        buffer_.reset();
    }
    return *this;
}

/// Retreat to previous node, skipping logically deleted (marked) nodes.
FamListIterator& FamListIterator::operator--() {
    if (const auto prev = FamListNode::getPrev(object_); prev.offset > 0) {
        object_.replaceWith(prev);

        // Skip over marked (deleted) nodes - iterate until we find an unmarked node
        while (FamListNode::isMarked(object_)) {
            if (const auto prev_prev = FamListNode::getPrev(object_); prev_prev.offset > 0) {
                object_.replaceWith(prev_prev);
            }
            else {
                break;  // No more nodes
            }
        }
        buffer_.reset();
    }
    return *this;
}

bool FamListIterator::operator==(const FamListIterator& other) const {
    return other.object_ == object_;
}

auto FamListIterator::operator->() -> pointer {
    return &object_;
}

/// Dereference operator: returns data buffer.
/// Safely skips marked nodes when dereferencing.
auto FamListIterator::operator*() const -> data_type& {
    if (!buffer_) {
        FamListNode::getData(object_, buffer_.emplace());
    }
    return *buffer_;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
