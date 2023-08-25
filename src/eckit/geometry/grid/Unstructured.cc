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


#include "eckit/geometry/grid/Unstructured.h"

#include "eckit/geometry/iterator/Unstructured.h"


namespace eckit::geometry::grid {


Grid::iterator Unstructured::cbegin() const {
    return iterator{new geometry::iterator::Unstructured(*this)};
}


Grid::iterator Unstructured::cend() const {
    return iterator{new geometry::iterator::Unstructured(*this, size())};
}


Unstructured::Unstructured(const Configuration& config) :
    Grid(config) {
}


}  // namespace eckit::geometry::grid
