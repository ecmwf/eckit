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


#include "eckit/geo/NextIterator.h"


namespace eckit::geo {


NextIterator::NextIterator(Grid::Iterator&& current, Grid::Iterator&& end) :
    current_ptr_(current.release()),
    end_ptr_(end.release()),
    current_(*current_ptr_),
    end_(*end_ptr_),
    index_(current_.index()) {}


bool NextIterator::next(Point& point) const {
    if (current_ != end_) {
        point  = *current_;
        index_ = current_.index();
        ++current_;
        return true;
    }

    return false;
}


}  // namespace eckit::geo
