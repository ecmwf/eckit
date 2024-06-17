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

#include "eckit/geo/Grid.h"


namespace eckit::geo::iterator {
class Unstructured;
}


namespace eckit::geo::grid {


class Unstructured final : public Grid {
public:
    // -- Constructors

    explicit Unstructured(std::vector<Point>&&);

    // -- Overridden methods

    iterator cbegin() const override;
    iterator cend() const override;

    size_t size() const override { return points_.size(); }

    bool includesNorthPole() const override { return true; }
    bool includesSouthPole() const override { return true; }
    bool isPeriodicWestEast() const override { return true; }

    std::vector<Point> to_points() const override { return points_; }

    // -- Class methods

    [[nodiscard]] static Spec* spec(const std::string& name);

private:
    // -- Members

    const std::vector<Point> points_;

    // -- Overridden methods

    void fill_spec(spec::Custom&) const override;

    // -- Friends

    friend class geo::iterator::Unstructured;
};


}  // namespace eckit::geo::grid
