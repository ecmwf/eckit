// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include "eckit/geo/area/BoundingBoxXY.h"
#include "eckit/geo/grid/Regular.h"
#include "eckit/geo/range/Regular.h"


namespace eckit::geo::grid::regular {


class RegularXY : public Regular {
public:

    // -- Types

    struct Increments : std::array<double, 2> {
        Increments(value_type dx, value_type dy);

        using array::array;

        bool operator==(const Increments&) const;
        bool operator!=(const Increments& other) const { return !operator==(other); }

        value_type dx() const { return operator[](0); }
        value_type dy() const { return operator[](1); }

        static Increments make_from_spec(const Spec&);
    };

    struct RangeXY : range::RegularXY {
        RangeXY(double start, double stop_included, double step);
        static RangeXY make_from_spec(const Spec&, const std::string& key);
    };

    using BoundingBoxXY = area::BoundingBoxXY;

    // -- Constructors

    explicit RegularXY(const Spec&);
    explicit RegularXY(const Increments&, BoundingBoxXY, order::Scan = scan_default(), const Projection* = nullptr);
    explicit RegularXY(const RangeXY& x, const RangeXY& y, const Projection* = nullptr);

    // -- Overridden methods

    const std::string& type() const override;

    [[nodiscard]] Point first_point() const override;
    [[nodiscard]] Point last_point() const override;

    [[nodiscard]] BoundingBox* calculate_bbox() const override;

    const Range& x() const override { return x_; }
    const Range& y() const override { return y_; }

private:

    // -- Overriden methods

    void fill_spec(spec::Custom&) const override;

    // -- Members

    const range::RegularXY x_;
    const range::RegularXY y_;

    // -- Friends

    friend class geo::iterator::Regular;
};


}  // namespace eckit::geo::grid::regular
