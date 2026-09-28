// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <string>

#include "eckit/geo/Area.h"


namespace eckit::geo::area {


class None final : public Area {
public:

    // -- Constructors

    None() = default;

    // -- Overriden methods

    const std::string& type() const override;

    bool intersects(area::BoundingBox&) const override { return false; }

private:

    // -- Overriden methods

    void fill_spec(spec::Custom&) const override;
};


}  // namespace eckit::geo::area
