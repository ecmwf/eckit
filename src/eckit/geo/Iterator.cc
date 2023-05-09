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


#include "eckit/geo/Iterator.h"

#include "eckit/geo/Projection.h"
#include "eckit/geo/Scanner.h"
#include "eckit/exception/Exceptions.h"


namespace eckit::geo {


Iterator::Iterator(Scanner* scanner, Projection* projection) : scanner_(scanner), projection_(projection) {
    ASSERT(scanner_);
    ASSERT(projection_);
}


bool Iterator::operator++() {
    return ++(*scanner_);
}


size_t Iterator::size() const {
    ASSERT(scanner_);
    return scanner_->size();
}


}  // namespace grit
