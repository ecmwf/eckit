// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "eckit/geo/Point.h"
#include "eckit/spec/Custom.h"


namespace eckit::geo::projection {


/**
 * @brief Reverse class
 * @details Used to reverse the forward and inverse methods of a projection.
 */
template <class P>
class Reverse : public P {
public:

    // -- Constructors

    using P::P;

    // -- Overridden methods

    inline Point fwd(const Point& p) const override { return P::inv(p); }
    inline Point inv(const Point& p) const override { return P::fwd(p); }

private:

    // -- Overridden methods

    void fill_spec(spec::Custom& custom) const override {
        P::fill_spec(custom);

        // spec is only set of non-default projection, or default projection with non-default parameters
        if (!custom.empty()) {
            custom.set("type", "reverse-" + custom.get_string("type"));
        }
    }
};

}  // namespace eckit::geo::projection
