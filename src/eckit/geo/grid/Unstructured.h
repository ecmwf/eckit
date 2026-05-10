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

#include <cstddef>
#include <vector>

#include "eckit/geo/Grid.h"


namespace eckit::geo::iterator {
class Unstructured;
}


namespace eckit::geo::grid {


class Unstructured : public Grid {
public:

    // -- Constructors

    explicit Unstructured(const Spec&);
    explicit Unstructured(const std::vector<double>& longitudes, const std::vector<double>& latitudes);

    // -- Overridden methods

    iterator cbegin() const override;
    iterator cend() const override;

    std::vector<size_t> shape() const override;

    BoundingBox* calculate_bbox() const override;

    [[nodiscard]] std::vector<Point> to_points() const override;
    [[nodiscard]] std::pair<std::vector<double>, std::vector<double>> to_latlons() const override;

    // -- Class methods

    [[nodiscard]] static Spec* spec(const std::string& uid);
    [[nodiscard]] static Unstructured* make_from_latlon(const std::vector<double>&, const std::vector<double>&);

protected:

    // -- Constructors

    Unstructured();

    virtual const std::vector<double>& longitudes() const { return longitudes_; }
    virtual const std::vector<double>& latitudes() const { return latitudes_; }

    // -- Overridden methods

    void fill_spec(spec::Custom&) const override;

private:

    // -- Members

    std::vector<double> longitudes_;
    std::vector<double> latitudes_;

    // -- Friends

    friend class geo::iterator::Unstructured;
};


}  // namespace eckit::geo::grid
