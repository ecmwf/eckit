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

#include "eckit/geo/grid/Regular.h"


namespace eckit::geo::grid::regular {


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

    double di() const { return Dy_; };
    double dj() const { return Dx_; };

    // -- Overridden methods

    iterator cbegin() const override;
    iterator cend() const override;

    size_t ni() const override { return Ny_; }
    size_t nj() const override { return Nx_; }

    // -- Class members
    // None

    // -- Class methods
    // None

protected:
    // -- Types

    struct Internal {
        explicit Internal(const Spec&);
        Internal(size_t Nx, size_t Ny, double Dx, double Dy) :
            Nx(Nx), Ny(Ny), Dx(Dx), Dy(Dy) {}

        size_t Nx;
        size_t Ny;
        double Dx;
        double Dy;
    };

    // -- Constructors

    explicit XYToLonLat(Internal&&);

    XYToLonLat(size_t Nx, size_t Ny, double Dx, double Dy);

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

    size_t Nx_;
    size_t Ny_;
    const double Dx_;
    const double Dy_;

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


}  // namespace eckit::geo::grid::regular
