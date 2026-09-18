// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <array>
#include <cmath>


namespace eckit::geo::util {


struct sincos_t final : std::array<double, 2> {
    explicit sincos_t(value_type r) : array{std::sin(r), std::cos(r)} {}

    value_type sin() const { return operator[](0); }
    value_type cos() const { return operator[](1); }
};


}  // namespace eckit::geo::util
