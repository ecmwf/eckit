// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include "eckit/geo/Grid.h"


namespace eckit::geo::cache {


class Grid final {
public:

    // -- Class methods

    static Grid& instance();

    static void save(const geo::Grid::uid_type&, const geo::Grid::Spec&);

private:

    // -- Constructors

    explicit Grid() = default;
};


}  // namespace eckit::geo::cache
