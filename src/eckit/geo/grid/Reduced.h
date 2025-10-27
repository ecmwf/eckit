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

#include "eckit/geo/Grid.h"


namespace eckit::geo::iterator {
class Reduced;
}


namespace eckit::geo::grid {


class Reduced : public Grid {
public:

    // -- Methods

    size_t size() const override { return nxacc().back(); }
    std::vector<size_t> shape() const override { return {size()}; }

    // -- Overridden methods

    [[nodiscard]] Point first_point() const override;
    [[nodiscard]] Point last_point() const override;

    // Methods

    virtual const std::vector<double>& latitudes() const = 0;
    virtual std::vector<double> longitudes(size_t) const = 0;

protected:

    // -- Constructors

    explicit Reduced(area::BoundingBox* bbox = nullptr, Projection* projection = nullptr) : Grid(bbox, projection) {}

    // -- Methods

    const std::vector<size_t>& nxacc() const;

    virtual size_t nx(size_t) const = 0;
    virtual size_t ny() const       = 0;

private:

    // -- Members

    mutable std::vector<size_t> nxacc_;

    // -- Friends

    friend class geo::iterator::Reduced;
};


}  // namespace eckit::geo::grid
