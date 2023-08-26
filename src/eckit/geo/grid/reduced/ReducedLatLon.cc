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


#include "eckit/geo/grid/reduced/ReducedLatLon.h"

#include "eckit/geo/iterator/Reduced.h"


namespace eckit::geo::grid::reduced {


ReducedLatLon::ReducedLatLon(const Configuration& config) :
    Reduced(config) {
}


Grid::iterator ReducedLatLon::cbegin() const {
    return iterator{new geo::iterator::Reduced(*this, 0)};
}


Grid::iterator ReducedLatLon::cend() const {
    return iterator{new geo::iterator::Reduced(*this, size())};
}


size_t ReducedLatLon::ni(size_t j) const {
    NOTIMP;
}


size_t ReducedLatLon::nj() const {
    NOTIMP;
}


const std::vector<double>& ReducedLatLon::latitudes() const {
    NOTIMP;
}


std::vector<double> ReducedLatLon::longitudes(size_t j) const {
    NOTIMP;
}


}  // namespace eckit::geo::grid::reduced
