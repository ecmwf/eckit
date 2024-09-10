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
#include "eckit/geo/util.h"


namespace eckit::geo::grid {


class ReducedGaussian : public Reduced {
public:
    // -- Constructors

    explicit ReducedGaussian(const Spec&);
    explicit ReducedGaussian(const pl_type&, const area::BoundingBox& = {}, projection::Rotation* = nullptr);
    explicit ReducedGaussian(size_t N, const area::BoundingBox& = {}, projection::Rotation* = nullptr);

    // -- Methods

    size_t N() const { return N_; }

    // -- Overridden methods

    iterator cbegin() const override;
    iterator cend() const override;

    size_t size() const override;
    size_t ni(size_t j) const override;
    size_t nj() const override;

private:
    // -- Members

    const size_t N_;
    const pl_type pl_;
    size_t j_;
    size_t Nj_;

    std::vector<std::unique_ptr<Range>> x_;
    std::unique_ptr<Range> y_;

    // -- Overridden methods

    void fill_spec(spec::Custom&) const override;

    const std::vector<double>& latitudes() const override;
    std::vector<double> longitudes(size_t i) const override;

    [[nodiscard]] Grid* make_grid_cropped(const Area&) const override;
};


}  // namespace eckit::geo::grid
