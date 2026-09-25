// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include "eckit/geo/Projection.h"


namespace eckit::geo::projection {


class None : public Projection {
public:

    // -- Constructors

    explicit None() : Projection(nullptr, PointLonLat{}, PointLonLat{}) {}
    explicit None(const Spec&) : None() {}

    // -- Overridden methods

    inline Point fwd(const Point& p) const override { return p; }
    inline Point inv(const Point& q) const override { return q; }

    const std::string& type() const override;

private:

    // -- Overridden methods

    void fill_spec(spec::Custom&) const override {}
};


}  // namespace eckit::geo::projection
