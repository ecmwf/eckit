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


#include "eckit/geo/grid/unstructured/Unstructured.h"


namespace eckit::geo::grid::unstructured {


Unstructured::Unstructured(const std::vector<Point>& points) : grid::Unstructured(points) {}


Unstructured* Unstructured::make_from_latlon(const std::vector<double>& latitudes,
                                             const std::vector<double>& longitudes) {}


}  // namespace eckit::geo::grid::unstructured
