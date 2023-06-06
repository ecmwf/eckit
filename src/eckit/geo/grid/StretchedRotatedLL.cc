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


#include "eckit/geo/grid/StretchedRotatedLL.h"

#include <ostream>


namespace eckit::geo::grid {


StretchedRotatedLL::StretchedRotatedLL(const Configuration& /*config*/) {}


void StretchedRotatedLL::print(std::ostream& out) const {
    out << "StretchedRotatedLL["
        << "]";
}


static const GridBuilder<StretchedRotatedLL> __repres("stretched_rotated_ll");


}  // namespace eckit::geo::grid
