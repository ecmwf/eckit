// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "eckit/geo/Grid.h"


namespace eckit::geo {
namespace cache {
class LatitudeLongitude;
}
namespace iterator {
class Unstructured;
}
}  // namespace eckit::geo


namespace eckit::geo::grid {


class Unstructured : public Grid {
public:

    // -- Constructors

    explicit Unstructured(const Spec&);
    explicit Unstructured(const std::vector<double>& longitudes, const std::vector<double>& latitudes,
                          const std::string& name = "");

    using Grid::Grid;

    // -- Methods

    std::string name() const override { return name_; }
    std::string arrangement() const override { return arrangement_; }

    // -- Overridden methods

    iterator cbegin() const override;
    iterator cend() const override;

    std::vector<size_t> shape() const override;

    BoundingBox* calculate_bbox() const override;

    [[nodiscard]] std::vector<Point> to_points() const override;
    [[nodiscard]] std::pair<std::vector<double>, std::vector<double>> to_latlons() const override;

    uid_type calculate_uid() const override;
    const std::string& type() const override;
    void cache() const override;

    // -- Class methods

    [[nodiscard]] static uid_type uid_from_latlons(const std::vector<double>&, const std::vector<double>&);

protected:

    // -- Constructors

    explicit Unstructured(const uid_type&, const std::string& name = "", BoundingBox* = nullptr, Projection* = nullptr);
    explicit Unstructured(const uid_type&, const std::string& name, const std::string& arrangement,
                          BoundingBox* = nullptr, Projection* = nullptr);

    // -- Methods

    [[nodiscard]] const cache::LatitudeLongitude& record() const;

private:

    // -- Members

    std::string name_;
    std::string arrangement_;

    // -- Overridden methods

    void fill_spec(spec::Custom&) const override;

    // -- Friends

    friend class geo::iterator::Unstructured;
};


}  // namespace eckit::geo::grid
