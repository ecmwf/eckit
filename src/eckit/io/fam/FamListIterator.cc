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

namespace {}

//----------------------------------------------------------------------------------------------------------------------
// ITERATOR

FamListIterator::FamListIterator(value_type object) : object_{std::move(object)} {}

FamListIterator& FamListIterator::operator++() {
    if (const auto next = FamListNode::getNext(object_); next.offset > 0) {
        object_.replaceWith(next);
        buffer_.reset();
    }
    return *this;
}

FamListIterator& FamListIterator::operator--() {
    if (const auto prev = FamListNode::getPrev(object_); prev.offset > 0) {
        object_.replaceWith(prev);
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

auto FamListIterator::operator*() -> data_type& {
    if (!buffer_) {
        FamListNode::getData(object_, buffer_.emplace());
    }
    return *buffer_;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
