// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <vector>

#include "eckit/geo/grid/Regular.h"
#include "eckit/geo/range/Irregular.h"


namespace eckit::geo::grid::regular {


/// Irregular (rectilinear) lat/lon grid: the points at the latitudes and longitudes given, each strictly monotonic
/// (increasing or decreasing, as the scanning order), {type: irregular_ll, latitudes: [...], longitudes: [...]}
class IrregularLL : public Regular {
public:

    // -- Constructors

    explicit IrregularLL(const Spec&);
    IrregularLL(const std::vector<double>& longitudes, const std::vector<double>& latitudes);

    // -- Overridden methods

    const std::string& type() const override;

    [[nodiscard]] BoundingBox* calculate_bbox() const override;

    const Range& lon() const override { return x_; }
    const Range& lat() const override { return y_; }

protected:

    // -- Overridden methods

    [[nodiscard]] Regular* make_regular_cropped(const BoundingBox&) const override;

private:

    // -- Members

    const range::Irregular x_;
    const range::Irregular y_;

    // -- Overridden methods

    void fill_spec(spec::Custom&) const override;
};


}  // namespace eckit::geo::grid::regular
