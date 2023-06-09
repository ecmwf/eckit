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

#include "eckit/geo/Grid.h"


namespace eckit {
class PathName;
}


namespace eckit::geo::grid {


class UnstructuredGrid : public Grid {
public:
    // -- Exceptions
    // None

    // -- Constructors

    explicit UnstructuredGrid(const PathName&);
    explicit UnstructuredGrid(const Configuration&);
    UnstructuredGrid(const std::vector<double>& latitudes, const std::vector<double>& longitudes,
                     const BoundingBox& = BoundingBox());

    UnstructuredGrid(const UnstructuredGrid&) = delete;
    UnstructuredGrid(UnstructuredGrid&&)      = delete;

    // -- Destructor

    ~UnstructuredGrid() override;

    // -- Convertors
    // None

    // -- Operators

    void operator=(const UnstructuredGrid&)  = delete;
    void operator=(const UnstructuredGrid&&) = delete;

    // -- Methods
    // None

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

protected:
    // -- Members
    // None

    // -- Methods

    void print(std::ostream&) const override;

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

private:
    // -- Members

    std::vector<double> latitudes_;
    std::vector<double> longitudes_;

    // -- Methods
    // None

    // -- Overridden methods

    Domain domain() const override;
    Iterator* iterator() const override;


    // Domain operations
    bool isPeriodicWestEast() const override;
    bool includesNorthPole() const override;
    bool includesSouthPole() const override;

    size_t numberOfPoints() const override;

    Renumber crop(BoundingBox&) const override;
    Renumber reorder(long scanningMode) const override;

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None
};


}  // namespace eckit::geo::grid
