// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>
#include <string>
#include <vector>

#include "eckit/geo/grid/Reduced.h"
#include "eckit/geo/order/Scan.h"
#include "eckit/geo/range/GaussianLatitude.h"
#include "eckit/geo/util.h"


namespace eckit::geo::grid::reduced {


class ReducedGaussian : public Reduced {
public:

    // -- Constructors

    explicit ReducedGaussian(const Spec&);
    explicit ReducedGaussian(const pl_type&, const BoundingBox& = BoundingBox::bounding_box_default(),
                             Projection* = nullptr);
    explicit ReducedGaussian(size_t N, const BoundingBox& = BoundingBox::bounding_box_default(), Projection* = nullptr);

    // -- Methods

    size_t N() const { return N_; }
    const pl_type& pl() const { return pl_; }
    bool octahedral() const;

    // -- Overridden methods

    std::string name() const override;

    iterator cbegin() const override;
    iterator cend() const override;

    size_t size() const override { return nxacc().back(); }
    size_t nxj(size_t j) const override { return longitude_.at(j)->size(); }
    const Range& lat() const override { return latitude_; };

    const order_type& order() const override { return scan_.order(); }
    renumber_type reorder(const order_type& to) const override { return scan_.reorder(to, pl_); }

    [[nodiscard]] const std::vector<double>& latitudes() const override { return latitude_.values(); }
    [[nodiscard]] const std::vector<double>& longitudes(size_t j) const override { return longitude_.at(j)->values(); }

    // -- Class members

    static const order::Scan& scan_default();

private:

    // -- Members

    const size_t N_;
    const pl_type pl_;
    const range::GaussianLatitude latitude_;

    std::vector<std::unique_ptr<Range>> longitude_;
    order::Scan scan_;

    // -- Overridden methods

    void fill_spec(spec::Custom&) const override;
    const std::string& type() const override;

    [[nodiscard]] Grid* make_grid_cropped(const Area&) const override;
};


}  // namespace eckit::geo::grid::reduced
