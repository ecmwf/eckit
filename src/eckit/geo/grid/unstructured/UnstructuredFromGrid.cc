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


#include "eckit/geo/grid/unstructured/UnstructuredFromGrid.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/geo/iterator/Unstructured.h"
#include "eckit/geo/spec/Custom.h"


namespace eckit::geo::grid::unstructured {


UnstructuredFromGrid::UnstructuredFromGrid(std::vector<Point>&& points) :
    Unstructured(area::BoundingBox::make_global_prime()), points_(points) {}


Grid::iterator UnstructuredFromGrid::cbegin() const {
    return iterator{new geo::iterator::Unstructured(*this, 0, points_)};
}


Grid::iterator UnstructuredFromGrid::cend() const {
    return iterator{new geo::iterator::Unstructured(*this)};
}


Spec* UnstructuredFromGrid::spec(const std::string& name) {
    return SpecByUID::instance().get(name).spec();
}


void UnstructuredFromGrid::spec(spec::Custom& custom) const {
    custom.set("type", "unstructured");
    custom.set("uid", uid());
}


Grid::uid_t UnstructuredFromGrid::uid() const {
    NOTIMP;
}


}  // namespace eckit::geo::grid::unstructured
