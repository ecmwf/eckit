// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <cstddef>
#include <vector>

#include "eckit/geo/Grid.h"
#include "eckit/geo/Point.h"
#include "eckit/geo/area/BoundingBox.h"


namespace eckit::geo::grid {


class Reduced : public Grid {
public:

    // -- Methods

    size_t size() const override { return nxacc().back(); }
    std::vector<size_t> shape() const override { return {size()}; }

    // -- Overridden methods

    [[nodiscard]] Point point(size_t index) const override;

    // -- Methods

    virtual const std::vector<double>& latitudes() const        = 0;
    virtual const std::vector<double>& longitudes(size_t) const = 0;

protected:

    // -- Constructors

    using Grid::Grid;

    // -- Methods

    const std::vector<size_t>& nxacc() const;

    /// Row of the point of this index (some rows can be empty)
    size_t row(size_t index) const;

    virtual size_t nxj(size_t) const = 0;

    // -- Overridden methods

    [[nodiscard]] Iterator::State* make_state() const override;

private:

    // -- Types

    struct State;

    // -- Members

    mutable std::vector<size_t> nxacc_;
};


}  // namespace eckit::geo::grid
