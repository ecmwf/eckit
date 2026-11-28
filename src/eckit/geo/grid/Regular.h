// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <cstddef>
#include <memory>
#include <vector>

#include "eckit/geo/Grid.h"
#include "eckit/geo/Projection.h"
#include "eckit/geo/order/RegularScan.h"


namespace eckit::geo::grid {


class Regular : public Grid {
public:

    // -- Overridden methods

    [[nodiscard]] Point point(size_t index) const final;

    size_t size() const final { return nx() * ny(); }
    std::vector<size_t> shape() const final { return {ny(), nx()}; }

    const order_type& order() const final;
    renumber_type reorder(const order_type& to) const final;

    /// Cropped by a bounding box (unprojected), of the same type and scanning mode if supported, otherwise unstructured
    [[nodiscard]] Grid* make_grid_cropped(const Area&) const final;

    /// Indices as the cropped grid points (see make_grid_cropped), of the same scanning mode (eg. alternating rows
    /// start in the same direction, whatever the number of rows cropped out)
    renumber_type crop(const Area&) const final;

    // -- Class methods

    static const order::RegularScan& scan_default();

protected:

    // -- Constructors

    explicit Regular(order::RegularScan scan = scan_default(), std::unique_ptr<const Projection> = nullptr);

    // -- Overridden methods

    void fill_spec(spec::Custom&) const override;
    std::unique_ptr<Iterator::State> make_state() const override;

    // -- Methods

    /// Point of grid coordinates (x, y)
    virtual Point point_xy(double x, double y) const { return projection().from_grid_xy(x, y); }

    /// Index of the point of the i-th x and j-th y values, in the grid order (scanning mode)
    size_t index(size_t i, size_t j) const;

    /// Cropped to a bounding box (within the grid's), of the same type and scanning mode; nullptr if not supported
    [[nodiscard]] virtual Regular* make_regular_cropped(const BoundingBox&) const { return nullptr; }

    void scan(const order_type& to);
    void scan(order::RegularScan);
    const order::RegularScan& scan() const { return scan_; }

private:

    // -- Types

    struct State;

    // -- Members

    order::RegularScan scan_;

    // -- Methods

    [[nodiscard]] Regular* regular_cropped(const Area&) const;
};


}  // namespace eckit::geo::grid
