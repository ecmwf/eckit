// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once


namespace eckit::geo::projection {


/**
 * @brief Functions of the latitude [radian] for conformal projections, on an ellipsoid of eccentricity e (a sphere if
 * e = 0), as PROJ (pj_msfn, pj_tsfn and pj_phi2)
 * @ref Map Projections: A Working Manual, John P. Snyder (1987)
 */
class Conformal {
public:

    // -- Constructors

    explicit Conformal(double e) : e_(e) {}

    // -- Methods

    /// Snyder (14-15)
    double m(double phi) const;

    /// Snyder (15-9)
    double t(double phi) const;

    /// The latitude of t, Snyder (7-9)
    double phi(double t) const;

private:

    // -- Members

    const double e_;
};


}  // namespace eckit::geo::projection
