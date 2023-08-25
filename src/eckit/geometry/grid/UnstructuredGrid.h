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

#include <vector>

#include "eckit/geometry/Grid.h"


namespace eckit::geometry::iterator {
class ListI;
}


namespace eckit::geometry::grid {


class UnstructuredGrid : public Grid {
public:
    // -- Exceptions
    // None

    // -- Constructors

    explicit UnstructuredGrid(const Configuration&);
    explicit UnstructuredGrid(const Grid&);
    UnstructuredGrid(const std::vector<double>& latitudes, const std::vector<double>& longitudes);

    // -- Destructor

    ~UnstructuredGrid() override;

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods

    Renumber list_duplicates() const;

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

private:
    // -- Constructors

    explicit UnstructuredGrid(std::pair<std::vector<double>, std::vector<double>>&&);

    // -- Members

    std::vector<double> latitudes_;
    std::vector<double> longitudes_;

    // -- Methods
    // None

    // -- Overridden methods

    iterator cbegin() const override;
    iterator cend() const override;

    size_t size() const override;

    bool isPeriodicWestEast() const override { return true; }
    bool includesNorthPole() const override { return true; }
    bool includesSouthPole() const override { return true; }

    std::vector<Point> to_points() const override;
    std::pair<std::vector<double>, std::vector<double>> to_latlon() const override;

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends

    friend class geometry::iterator::ListI;
};


}  // namespace eckit::geometry::grid
