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

#include <memory>

#include "eckit/geo/grid/Reduced.h"
#include "eckit/geo/order/HEALPix.h"


namespace eckit::geo::grid::reduced {


class HEALPix final : public Reduced {
public:

    // -- Constructors

    explicit HEALPix(const Spec&);
    explicit HEALPix(size_t Nside, order_type = order::HEALPix::order_default());

    // -- Methods

    size_t Nside() const { return Nside_; }

    // -- Overridden methods

    iterator cbegin() const override;
    iterator cend() const override;

    size_t size() const override;

    size_t nx(size_t j) const override;
    size_t ny() const override;

    [[nodiscard]] std::vector<Point> to_points() const override;

    const order_type& order() const override { return healpix_.order(); }
    renumber_type reorder(const order_type& to) const override { return healpix_.reorder(to, Nside_); }

    [[nodiscard]] Grid* make_grid_reordered(const order_type& order) const override {
        return new HEALPix(Nside_, order);
    }

    const std::vector<double>& latitudes() const override;
    const std::vector<double>& longitudes(size_t j) const override;

    // -- Class members

    [[nodiscard]] static Spec* spec(const std::string& name);

    // -- Class methods

    static size_t size_from_nside(size_t);
    static size_t nside_from_size(size_t);

private:

    // -- Members

    const size_t Nside_;
    order::HEALPix healpix_;

    mutable std::vector<double> healpix_latitudes_;
    mutable std::vector<double> nested_latitudes_;
    mutable std::vector<double> nested_longitudes_;

    // -- Overridden methods

    void fill_spec(spec::Custom&) const override;
    const std::string& type() const override;
};


}  // namespace eckit::geo::grid::reduced
