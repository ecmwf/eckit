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

#include "eckit/geo/PointsContainer.h"


namespace eckit::geo::container {


struct PointsLonLatReference : PointsContainer {
    explicit PointsLonLatReference(const std::vector<double>& longitudes, const std::vector<double>& latitudes);

    Point get(size_t index) const override { return PointLonLat{longitudes.at(index), latitudes.at(index)}; }
    size_t size() const override { return longitudes.size(); }

    [[nodiscard]] std::vector<Point> to_points() const override;
    [[nodiscard]] std::pair<std::vector<double>, std::vector<double>> to_latlons() const override {
        return {latitudes, longitudes};
    }

private:
    const std::vector<double>& longitudes;
    const std::vector<double>& latitudes;
};


}  // namespace eckit::geo::container
