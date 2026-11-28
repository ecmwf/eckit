// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <cstddef>
#include <memory>
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
    virtual double longitude(size_t j, size_t i) const { return longitudes(j).at(i); }

    const order::ReducedScan& scan() const { return scan_; }

    // -- Overridden methods

    /// Number of points of each row (rows in the default order), whatever the order of the points
    const pl_type& pl() const final { return pl_; }

    [[nodiscard]] Point point(size_t index) const override;

    size_t size() const override { return nxacc().back(); }
    std::vector<size_t> shape() const override { return {size()}; }

    const order_type& order() const override { return scan_.order(); }
    renumber_type reorder(const order_type& to) const override { return scan_.reorder(to, nxacc()); }

protected:

    // -- Types

    /// A pl owned by the grid, or a reference to a cached pl (e.g. util::reduced_octahedral_pl), which outlives the
    /// grid as such caches are not purged (see LibEcKitGeo::purgeCache)
    class PL {
    public:

        static PL own(pl_type pl) { return {std::move(pl), nullptr}; }
        static PL cached(const pl_type& pl) { return {{}, &pl}; }
        static PL cached(pl_type&&) = delete;  // not of a temporary

    private:

        PL(pl_type own, const pl_type* cached) : own_(std::move(own)), cached_(cached) {}

        pl_type own_;
        const pl_type* cached_;

        friend class Reduced;
    };

    // -- Constructors

    explicit Reduced(PL, order::ReducedScan, std::unique_ptr<const Projection> = nullptr);
    explicit Reduced(PL, std::unique_ptr<BoundingBox>, std::unique_ptr<const Projection> = nullptr);

    // -- Methods

    /// Points before each row (rows in the default order), then the number of points
    const std::vector<size_t>& nxacc() const;

    /// Row of the point of this index, in the default order (some rows can be empty)
    size_t row(size_t index) const;

    /// Number of points of a row (rows in the default order)
    virtual size_t nxj(size_t) const = 0;

    // -- Overridden methods

    std::unique_ptr<Iterator::State> make_state() const override;
    void fill_spec(spec::Custom&) const override;

private:

    // -- Types

    struct State;

    // -- Members

    const pl_type pl_owned_;
    const pl_type& pl_;  // pl_owned_, or a cached pl

    order::ReducedScan scan_;

    mutable std::vector<size_t> nxacc_;
    mutable util::once_flag nxacc_once_;

    // -- Methods

    /// Index in the default order, and its row j (a guess on input)
    size_t canonical(size_t index, size_t& j) const;
};


}  // namespace eckit::geo::grid
