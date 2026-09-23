// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "eckit/geo/Grid.h"
#include "eckit/geo/area/BoundingBox.h"


namespace eckit::geo::iterator {
class Reduced;
}


namespace eckit::geo::grid {


class Reduced : public Grid {
public:

    // -- Methods

    size_t size() const override { return nxacc().back(); }
    std::vector<size_t> shape() const override { return {size()}; }

    // -- Overridden methods

    [[nodiscard]] Point first_point() const override;
    [[nodiscard]] Point last_point() const override;

    // Methods

    virtual const std::vector<double>& latitudes() const        = 0;
    virtual const std::vector<double>& longitudes(size_t) const = 0;

protected:

    // -- Constructors

    using Grid::Grid;

    // -- Methods

    const std::vector<size_t>& nxacc() const;

    virtual size_t nxj(size_t) const = 0;

private:

    // -- Members

    mutable std::vector<size_t> nxacc_;

    // -- Friends

    friend class geo::iterator::Reduced;
};


}  // namespace eckit::geo::grid
