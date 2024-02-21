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


class LambertAzimuthalEqualArea final : public Regular {
public:
    // -- Constructors

    explicit LambertAzimuthalEqualArea(const Spec&);

    LambertAzimuthalEqualArea(size_t Nx, size_t Ny, double Dx, double Dy);

    // -- Overridden methods

    iterator cbegin() const override;
    iterator cend() const override;

    size_t ni() const override { return Ny_; }
    size_t nj() const override { return Nx_; }

private:
    // -- Types

    struct Internal {
        explicit Internal(const Spec&);
        size_t Nx;
        size_t Ny;
        double Dx;
        double Dy;
    };

    // -- Constructors

    explicit LambertAzimuthalEqualArea(Internal&&);

    // -- Members

    const size_t Nx_;
    const size_t Ny_;
    const double Dx_;
    const double Dy_;

    // -- Overridden methods

    const std::vector<double>& longitudes() const override;
    const std::vector<double>& latitudes() const override;

    void spec(spec::Custom&) const override;
};


}  // namespace eckit::geo::grid::regular
