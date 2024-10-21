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

#include "detail/FamNode.h"
#include "detail/FamSessionDetail.h"
#include "eckit/exception/Exceptions.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------
// ITERATOR

FamListIterator::FamListIterator(const value_type& object): object_ {object} { }

auto FamListIterator::operator++() -> FamListIterator& {
    if (const auto next = FamNode::getNext(object_); next.offset > 0) {
        invalid_ = true;
        object_.replaceWith(next);
    }
    return *this;
}

auto FamListIterator::operator--() -> FamListIterator& {
    if (const auto prev = FamNode::getPrev(object_); prev.offset > 0) {
        invalid_ = true;
        object_.replaceWith(prev);
    }
    return *this;
}

auto FamListIterator::operator==(const FamListIterator& other) const -> bool {
    return other.object_ == object_;
}

auto FamListIterator::operator->() -> pointer {
    return &object_;
}

auto FamListIterator::operator*() -> reference {
    if (invalid_) { FamNode::getData(object_, data_); }
    return data_;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
