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

#include "eckit/geo/grid/Unstructured.h"


namespace eckit {
class PathName;
}


namespace eckit::geo::grid::unstructured {


class UnstructuredFromGrid final : public Unstructured {
public:
    // -- Types
    // None

    // -- Exceptions
    // None

    // -- Constructors

    explicit UnstructuredFromGrid(std::vector<Point>&&);

    // -- Destructor
    // None

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods
    // None

    // -- Overridden methods

    iterator cbegin() const override;
    iterator cend() const override;

    size_t size() const override { return points_.size(); }
    uid_t uid() const override;

    bool includesNorthPole() const override { return true; }
    bool includesSouthPole() const override { return true; }
    bool isPeriodicWestEast() const override { return true; }

    std::vector<Point> to_points() const override { return points_; }

    // -- Class members
    // None

    // -- Class methods

    static Spec* spec(const std::string& name);

private:
    // -- Members

    const std::vector<Point> points_;

    // -- Methods
    // None

    // -- Overridden methods

    void spec(spec::Custom&) const override;

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None
};


}  // namespace eckit::geo::grid::unstructured
