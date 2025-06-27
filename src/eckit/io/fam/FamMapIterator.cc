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

#include "eckit/io/fam/FamMapIterator.h"

#include "detail/FamMapNode.h"

// #include "detail/FamSessionDetail.h"
// #include "eckit/exception/Exceptions.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

FamMapIterator::FamMapIterator(const FamRegion& region, const fam::index_t offset) :
    region_{region}, node_{region_.proxyObject(offset)} {}

auto FamMapIterator::operator++() -> FamMapIterator& {
    if (const auto next = FamMapNode::getNext(node_); next.region > 0) {
        node_.replaceWith(next);
        list_.reset();
    }
    return *this;
}

auto FamMapIterator::operator->() -> pointer {
    if (list_) {
        list_ = FamMapNode::getList(region_, node_);
    }
    return list_.get();
}

auto FamMapIterator::operator*() -> reference {
    if (list_) {
        list_ = FamMapNode::getList(region_, node_);
    }
    return *list_;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
