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

#include <utility>
#include <vector>

#include "eckit/geo/Container.h"
#include "eckit/geo/Point.h"


namespace eckit {
class MD5;
}


namespace eckit::geo::container {


class PointsContainer : public Container<Point> {
protected:

    PointsContainer() = default;

    static std::pair<std::vector<double>, std::vector<double>> to_latlons(const std::vector<Point>&);

public:

    [[nodiscard]] virtual std::vector<Point> to_points() const                                   = 0;
    [[nodiscard]] virtual std::pair<std::vector<double>, std::vector<double>> to_latlons() const = 0;

    virtual void hash(MD5&) const = 0;
};


struct PointsInstance : PointsContainer {
    explicit PointsInstance(std::vector<Point>&& points) : points_(points) {}

    Point get(size_t index) const override { return points_.at(index); }
    size_t size() const override { return points_.size(); }

    [[nodiscard]] std::vector<Point> to_points() const override { return points_; }
    [[nodiscard]] std::pair<std::vector<double>, std::vector<double>> to_latlons() const override;

    void hash(MD5&) const override;

private:

    const std::vector<Point> points_;
};


struct PointsLonLatReference : PointsContainer {
    explicit PointsLonLatReference(const std::vector<double>& longitudes, const std::vector<double>& latitudes);

    Point get(size_t index) const override { return PointLonLat{longitudes.at(index), latitudes.at(index)}; }
    size_t size() const override { return longitudes.size(); }

    [[nodiscard]] std::vector<Point> to_points() const override;
    [[nodiscard]] std::pair<std::vector<double>, std::vector<double>> to_latlons() const override;

    void hash(MD5&) const override;

private:

    const std::vector<double>& longitudes;
    const std::vector<double>& latitudes;
};


struct PointsReference : PointsContainer {
    explicit PointsReference(const std::vector<Point>& points) : points_(points) {}

    Point get(size_t index) const override { return points_.at(index); }
    size_t size() const override { return points_.size(); }

    [[nodiscard]] std::vector<Point> to_points() const override { return points_; }
    [[nodiscard]] std::pair<std::vector<double>, std::vector<double>> to_latlons() const override;

    void hash(MD5&) const override;

private:

    const std::vector<Point>& points_;
};


}  // namespace eckit::geo::container
