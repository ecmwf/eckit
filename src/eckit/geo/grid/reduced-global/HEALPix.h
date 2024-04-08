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

#include "eckit/geo/grid/ReducedGlobal.h"


namespace eckit::geo::grid::reducedglobal {


class HEALPix final : public ReducedGlobal {
public:
    // -- Types
    // None

    // -- Exceptions
    // None

    // -- Constructors

    explicit HEALPix(const Spec&);
    explicit HEALPix(size_t Nside, Ordering = Ordering::healpix_ring);

    // -- Destructor
    // None

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods

    size_t Nside() const { return Nside_; }

    // -- Overridden methods

    iterator cbegin() const override;
    iterator cend() const override;

    size_t size() const override;

    std::vector<Point> to_points() const override;

    size_t ni(size_t j) const override;
    size_t nj() const override;

    Ordering order() const override { return ordering_; }
    Renumber reorder(Ordering) const override;
    Grid* make_grid_reordered(Ordering ordering) const override { return new HEALPix(Nside_, ordering); }

    // -- Class members

    static Spec* spec(const std::string& name);

    // -- Class methods
    // None

private:
    // -- Members

    const size_t Nside_;
    const Ordering ordering_;

    mutable std::vector<double> latitudes_;

    // -- Methods
    // None

    // -- Overridden methods

    area::BoundingBox boundingBox() const override;

    bool includesNorthPole() const override { return true; }
    bool includesSouthPole() const override { return true; }
    bool isPeriodicWestEast() const override { return true; }

    std::pair<std::vector<double>, std::vector<double>> to_latlon() const override;

    const std::vector<double>& latitudes() const override;
    std::vector<double> longitudes(size_t j) const override;

    void spec(spec::Custom&) const override;

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None
};


}  // namespace eckit::geo::grid::reducedglobal
