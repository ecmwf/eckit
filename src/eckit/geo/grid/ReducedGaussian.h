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
#include <vector>

#include "eckit/geo/Range.h"
#include "eckit/geo/grid/Reduced.h"
#include "eckit/geo/order/Scan.h"
#include "eckit/geo/util.h"


namespace eckit::geo::grid {


class ReducedGaussian : public Reduced {
public:

    // -- Constructors

    explicit ReducedGaussian(const Spec&);
    explicit ReducedGaussian(const pl_type&, area::BoundingBox* = nullptr, projection::Rotation* = nullptr);
    explicit ReducedGaussian(size_t N, const pl_type&, area::BoundingBox* = nullptr, projection::Rotation* = nullptr);
    explicit ReducedGaussian(size_t N, area::BoundingBox* = nullptr, projection::Rotation* = nullptr);

    // -- Methods

    size_t N() const { return N_; }
    const pl_type& pl() const { return pl_; }

    // -- Overridden methods

    iterator cbegin() const override;
    iterator cend() const override;

    size_t size() const override;
    size_t ni(size_t j) const override;
    size_t nj() const override;

    const order_type& order() const override { return order_.order(); }
    Reordering reorder(const order_type& to) const override { return order_.reorder(to); }

private:

    // -- Members

    const size_t N_;
    const pl_type pl_;
    size_t j_;
    size_t Nj_;

    std::vector<std::unique_ptr<Range>> x_;
    std::unique_ptr<Range> y_;
    order::Scan order_;

    // -- Overridden methods

    void fill_spec(spec::Custom&) const override;
    const std::string& type() const override;

    const std::vector<double>& latitudes() const override;
    std::vector<double> longitudes(size_t j) const override;

    [[nodiscard]] Grid* make_grid_cropped(const Area&) const override;
};


}  // namespace eckit::geo::grid
