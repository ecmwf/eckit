// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <string>

#include "eckit/geo/Point.h"
#include "eckit/geo/Projection.h"
#include "eckit/geo/grid/Regular.h"
#include "eckit/geo/range/Regular.h"


namespace eckit::geo::grid::regular {


/**
 * @brief Regular grid on a plane, of (x, y) points, as they are or projected (projection on top)
 * @details Spec: x and y as [start, stop, step] (stop included), optionally false: [x, y] (false easting/northing) and
 * projection (from (x, y), applied to the points less the false easting/northing).
 */
class RegularXY : public Regular {
public:

    // -- Types

    struct RangeXY : range::RegularXY {
        RangeXY(double start, double stop_included, double step);
        static RangeXY make_from_spec(const Spec&, const std::string& key);
    };

    // -- Constructors

    explicit RegularXY(const Spec&);
    RegularXY(const RangeXY& x, const RangeXY& y, const Projection* = nullptr, const PointXY& false_xy = {});

    // -- Overridden methods

    const std::string& type() const override;

    [[nodiscard]] Point first_point() const override;
    [[nodiscard]] Point last_point() const override;
    [[nodiscard]] BoundingBox* calculate_bbox() const override;

    const Figure& figure() const override;

    const Range& x() const override { return x_; }
    const Range& y() const override { return y_; }

private:

    // -- Members

    const range::RegularXY x_;
    const range::RegularXY y_;
    const PointXY false_;
    const bool projected_;

    // -- Overridden methods

    Point point(double x, double y) const override;
    void fill_spec(spec::Custom&) const override;
};


}  // namespace eckit::geo::grid::regular
