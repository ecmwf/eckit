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


#pragma once

#include "eckit/exception/Exceptions.h"
#include "eckit/geo/Container.h"


namespace eckit::geo::container {


struct LonLatReference : Container {
    explicit LonLatReference(const std::vector<double>& longitudes, const std::vector<double>& latitudes) :
        longitudes(longitudes), latitudes(latitudes) {
        ASSERT(longitudes.size() == latitudes.size());
    }

    Point get(size_t index) const override { return PointLonLat{longitudes.at(index), latitudes.at(index)}; }
    size_t size() const override { return longitudes.size(); }

    const std::vector<double>& longitudes;
    const std::vector<double>& latitudes;
};


}  // namespace eckit::geo::container