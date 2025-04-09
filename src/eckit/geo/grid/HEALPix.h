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

#include "eckit/geo/grid/Reduced.h"
#include "eckit/geo/order/HEALPix.h"


namespace eckit::geo::grid {


class HEALPix final : public Reduced {
public:

    // -- Constructors

    explicit HEALPix(const Spec&);
    explicit HEALPix(size_t Nside, order_type = order::HEALPix::ring);

    // -- Methods

    size_t Nside() const { return Nside_; }

    // -- Overridden methods

    iterator cbegin() const override;
    iterator cend() const override;

    size_t size() const override;

    size_t ni(size_t j) const override;
    size_t nj() const override;

    std::vector<Point> to_points() const override;
    std::pair<std::vector<double>, std::vector<double>> to_latlons() const override;

    const order_type& order() const override { return order_.order(); }
    Reordering reorder(const order_type& to) const override { return order_.reorder(to); }

    [[nodiscard]] Grid* make_grid_reordered(const order_type& order) const override {
        return new HEALPix(Nside_, order);
    }

    // -- Class members

    [[nodiscard]] static Spec* spec(const std::string& name);

private:

    // -- Members

    const size_t Nside_;
    order::HEALPix order_;

    mutable std::vector<double> latitudes_;

    // -- Overridden methods

    bool includesNorthPole() const override { return true; }
    bool includesSouthPole() const override { return true; }
    bool isPeriodicWestEast() const override { return true; }

    void fill_spec(spec::Custom&) const override;
    const std::string& type() const override;

    const std::vector<double>& latitudes() const override;
    std::vector<double> longitudes(size_t j) const override;
};


}  // namespace eckit::geo::grid
