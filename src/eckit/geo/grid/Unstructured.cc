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

#include "eckit/geo/container/LonLatReference.h"
#include "eckit/geo/container/PointsMove.h"
#include "eckit/geo/container/PointsReference.h"
#include "eckit/geo/iterator/Unstructured.h"
#include "eckit/geo/spec/Custom.h"


namespace eckit::geo::grid {


Grid::iterator Unstructured::cbegin() const {
    return iterator{new geo::iterator::Unstructured(*this, 0, container_)};
}


Grid::iterator Unstructured::cend() const {
    return iterator{new geo::iterator::Unstructured(*this)};
}


Unstructured::Unstructured(const std::vector<double>& latitudes, const std::vector<double>& longitudes) :
    container_{new container::LonLatReference{longitudes, latitudes}} {}


Unstructured::Unstructured(const std::vector<Point>& points) : container_{new container::PointsReference{points}} {}


Unstructured::Unstructured(std::vector<Point>&& points) : container_{new container::PointsMove{std::move(points)}} {}


size_t Unstructured::size() const {
    return container_->size();
}


std::vector<Point> Unstructured::to_points() const {
    return container_->to_points();
}


std::pair<std::vector<double>, std::vector<double> > Unstructured::to_latlons() const {
    return container_->to_latlons();
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
