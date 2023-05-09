/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/geo/iterator/IteratorComposer.h"

#include "eckit/exception/Exceptions.h"


namespace eckit::geo::iterator {


IteratorComposer::IteratorComposer(Scanner* scanner, const std::vector<Projection*>& projections) :
    scanner_(scanner) {
    ASSERT(scanner_);

    projections_.reserve(projections.size());
    for (auto* p : projections) {
        ASSERT(p != nullptr);
        projections_.emplace_back(p);
    }
}


bool IteratorComposer::operator++() {
    return scanner_->operator++();
}


size_t IteratorComposer::size() const {
    return scanner_->size();
}


}  // namespace eckit::geo::iterator
