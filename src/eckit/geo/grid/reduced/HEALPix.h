// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include "eckit/geo/Point.h"
#include "eckit/geo/Projection.h"
#include "eckit/geo/Range.h"
#include "eckit/geo/grid/Reduced.h"
#include "eckit/geo/order/HEALPix.h"


namespace eckit::geo::grid::reduced {


class HEALPix final : public Reduced {
public:

    // -- Constructors

    explicit HEALPix(const Spec&);
    explicit HEALPix(size_t Nside, order_type = order::HEALPix::order_default(), BoundingBox* = nullptr,
                     const Projection* = nullptr);

    // -- Methods

    size_t Nside() const { return Nside_; }

    // -- Overridden methods

    std::string name() const override { return "H" + std::to_string(Nside_); }

    BoundingBox* calculate_bbox() const override;

    iterator cbegin() const override;
    iterator cend() const override;

    size_t size() const override;

    size_t nxj(size_t j) const override;
    const Range& lat() const override { return *y_; }

    [[nodiscard]] std::vector<Point> to_points() const override;

    const order_type& order() const override { return order_.order(); }
    renumber_type reorder(const order_type& to) const override { return order_.reorder(to, Nside_); }

    [[nodiscard]] Grid* make_grid_reordered(const order_type& order) const override {
        return new HEALPix(Nside_, order);
    }

    [[nodiscard]] const std::vector<double>& latitudes() const override;
    [[nodiscard]] const std::vector<double>& longitudes(size_t j) const override;

    // -- Class members

    [[nodiscard]] static Spec* spec(const std::string& name);

    // -- Class methods

    static size_t size_from_nside(size_t);
    static size_t nside_from_size(size_t);

private:

    // -- Members

    const size_t Nside_;
    order::HEALPix order_;

    const std::unique_ptr<Range> y_;

    mutable std::vector<double> nested_latitudes_;
    mutable std::vector<double> nested_longitudes_;

    // -- Overridden methods

    void fill_spec(spec::Custom&) const override;
    const std::string& type() const override;
};


}  // namespace eckit::geo::grid::reduced
