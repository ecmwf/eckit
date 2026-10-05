// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <utility>
#include <vector>

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

    template <typename... Args>
    explicit Reverse(Args&&... args) : P(std::forward<Args>(args)...) {
        this->reverse_point_types();
    }

    // -- Overridden methods

    inline Point fwd(const Point& p) const override { return P::inv(p); }
    inline Point inv(const Point& p) const override { return P::fwd(p); }

protected:

    // -- Overridden methods

    // NOTE: the default (point by point, using the reversed fwd/inv above), as a specialised P::fwd_vector/inv_vector
    // is not reversed
    std::vector<std::vector<double>> fwd_vector(const std::vector<double>& v1, const std::vector<double>& v2,
                                                const std::vector<double>& v3) const override {
        return Projection::fwd_vector(v1, v2, v3);
    }

    std::vector<std::vector<double>> inv_vector(const std::vector<double>& v1, const std::vector<double>& v2,
                                                const std::vector<double>& v3) const override {
        return Projection::inv_vector(v1, v2, v3);
    }

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
