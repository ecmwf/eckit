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

#include <array>

#include "eckit/geo/grid/Regular.h"


namespace eckit::geo::grid {


class XYToLonLat : public Regular {
public:
    // -- Types
    // None

    // -- Exceptions
    // None

    // -- Constructors

    explicit XYToLonLat(const Spec&);

    // -- Destructor
    // None

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods

    double dx() const { return grid_[0]; };
    double dy() const { return grid_[1]; };

    size_t nx() const override { return shape_[1]; }
    size_t ny() const override { return shape_[0]; }

    // -- Overridden methods

    iterator cbegin() const override;
    iterator cend() const override;

    // -- Class members
    // None

    // -- Class methods
    // None

protected:
    // -- Members
    // None

    // -- Methods
    // None

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

private:
    // -- Members

    std::array<double, 2> grid_;
    std::array<size_t, 2> shape_;

    // -- Methods
    // None

    // -- Overridden methods

    const std::vector<double>& longitudes() const override;
    const std::vector<double>& latitudes() const override;

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None
};


struct Mercator final : public XYToLonLat {
    explicit Mercator(const Spec& spec) :
        XYToLonLat(spec) {}
    void spec(spec::Custom&) const override;
};

struct LambertAzimuthalEqualArea final : public XYToLonLat {
    explicit LambertAzimuthalEqualArea(const Spec& spec) :
        XYToLonLat(spec) {}
    void spec(spec::Custom&) const override;
};


}  // namespace eckit::geo::grid
