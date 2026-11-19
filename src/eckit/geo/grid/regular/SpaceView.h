// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <cstddef>

#include "eckit/geo/grid/regular/RegularXY.h"


namespace eckit::geo::grid::regular {


/**
 * @brief Space view (geostationary satellite) grid, as GRIB (template 3.90), (x, y) as scanning angles
 * @details Parameters, in grid lengths along the scanning directions (order) unless stated:
 * - nr: satellite distance from the centre, in Earth (equatorial) radii, above lon_0 on the equator;
 * - dx, dy: apparent Earth diameter;
 * - xp, yp: sub-satellite point;
 * - xo, yo: first grid point.
 * The figure is as given (by default, the Earth).
 */
class SpaceView final : public RegularXY {
public:

    // -- Types

    struct Parameters {
        size_t nx;
        size_t ny;
        double dx;
        double dy;
        double xp;
        double yp;
        double xo;
        double yo;
        double nr;
        double lon_0;

        static Parameters make_from_spec(const Spec&);
    };

    // -- Constructors

    explicit SpaceView(const Spec&);

    // -- Methods

    const Parameters& parameters() const { return parameters_; }

    // -- Overridden methods

    const std::string& type() const override;

private:

    // -- Types

    struct Geometry;

    // -- Constructors

    SpaceView(const Parameters&, const Spec&);
    SpaceView(const Parameters&, const Spec&, Geometry&&);

    // -- Class methods

    static Geometry make_geometry(const Parameters&, const Spec&);

    // -- Members

    const Parameters parameters_;

    // -- Overridden methods

    void fill_spec(spec::Custom&) const override;
};


}  // namespace eckit::geo::grid::regular
