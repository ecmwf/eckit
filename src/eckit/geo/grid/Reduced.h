// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <cstddef>
#include <vector>

#include "eckit/geo/Grid.h"
#include "eckit/geo/Point.h"
#include "eckit/geo/area/BoundingBox.h"
#include "eckit/geo/order/ReducedScan.h"
#include "eckit/geo/util.h"
#include "eckit/geo/util/mutex.h"


namespace eckit::geo::grid {


class Reduced : public Grid {
public:

    // -- Methods

    /// Latitudes of the rows, in the default order (north to south) whatever the order of the points
    virtual const std::vector<double>& latitudes() const = 0;

    /// Longitudes of a row (rows in the default order), west to east whatever the order of the points
    virtual const std::vector<double>& longitudes(size_t) const = 0;

    const order::ReducedScan& scan() const { return scan_; }

    /// Number of points of each row (rows in the default order), as pl of these grid points (e.g. if cropped)
    pl_type points_per_row() const;

    // -- Overridden methods

    [[nodiscard]] Point point(size_t index) const override;

    size_t size() const override { return nxacc().back(); }
    std::vector<size_t> shape() const override { return {size()}; }

    const order_type& order() const override { return scan_.order(); }
    renumber_type reorder(const order_type& to) const override { return scan_.reorder(to, points_per_row()); }

protected:

    // -- Constructors

    using Grid::Grid;

    explicit Reduced(order::ReducedScan, const Projection* = nullptr);

    // -- Methods

    /// Points before each row (rows in the default order), then the number of points
    const std::vector<size_t>& nxacc() const;

    /// Row of the point of this index, in the default order (some rows can be empty)
    size_t row(size_t index) const;

    /// Number of points of a row (rows in the default order)
    virtual size_t nxj(size_t) const = 0;

    // -- Overridden methods

    [[nodiscard]] Iterator::State* make_state() const override;
    void fill_spec(spec::Custom&) const override;

private:

    // -- Types

    struct State;

    // -- Members

    order::ReducedScan scan_;
    mutable std::vector<size_t> nxacc_;

    mutable renumber_type canonical_;  ///< index in the default order of each point, if not in the default order
    mutable util::once_flag canonical_once_;

    // -- Methods

    const renumber_type& canonical() const;
};


}  // namespace eckit::geo::grid
