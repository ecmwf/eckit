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


#include "eckit/geo/grid/Unstructured.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/geo/iterator/Unstructured.h"
#include "eckit/geo/spec/Custom.h"


namespace eckit::geo::grid {


Unstructured::Unstructured(std::vector<Point>&& points) : Grid(area::BoundingBox{}), points_(points) {}


Grid::iterator Unstructured::cbegin() const {
    return iterator{new geo::iterator::Unstructured(*this, 0, points_)};
}


Grid::iterator Unstructured::cend() const {
    return iterator{new geo::iterator::Unstructured(*this)};
}


Spec* Unstructured::spec(const std::string& name) {
    return SpecByUID::instance().get(name).spec();
}


void Unstructured::fill_spec(spec::Custom& custom) const {
    Grid::fill_spec(custom);

    custom.set("type", "unstructured");
    custom.set("uid", uid());
}


}  // namespace eckit::geo::grid
