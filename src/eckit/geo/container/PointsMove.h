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

#include "eckit/geo/Container.h"


namespace eckit::geo::container {


struct PointsMove : Container {
    explicit PointsMove(std::vector<Point>&& points) : points_(points) {}

    Point get(size_t index) const override { return points_.at(index); }
    size_t size() const override { return points_.size(); }

    [[nodiscard]] std::vector<Point> to_points() const override { return points_; }
    [[nodiscard]] std::pair<std::vector<double>, std::vector<double>> to_latlon() const override;

private:
    const std::vector<Point> points_;
};


}  // namespace eckit::geo::container
