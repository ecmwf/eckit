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

namespace eckit {

constexpr const auto sessionName = "FamList-Iterator";

//----------------------------------------------------------------------------------------------------------------------

FamListIterator::FamListIterator(FamObject::UPtr object): obj_ {std::move(object)} { }

auto FamListIterator::operator++() -> FamListIterator& {
    const auto next = getNext(*obj_);

    if (next.offset > 0) { obj_->replace(next); }

    return *this;
}

auto FamListIterator::operator--() -> FamListIterator& {
    const auto prev = getPrev(*obj_);

    if (prev.offset > 0) { obj_->replace(prev); }

    return *this;
}

auto FamListIterator::operator->() const -> FamObject* {
    return obj_.get();
}

auto FamListIterator::operator*() const -> Buffer {
    const auto length = obj_->get<fam::size_t>(offsetof(FamNode, length));

    auto buffer = Buffer(length);

    obj_->get(buffer.data(), sizeof(FamNode), buffer.size());

    return buffer;
}

auto FamListIterator::operator==(const FamListIterator& other) const -> bool {
    return *obj_ == *other.obj_;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
