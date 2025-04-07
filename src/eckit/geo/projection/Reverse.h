/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#pragma once

#include "eckit/geo/Projection.h"
#include "eckit/geo/spec/Custom.h"


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

    // -- Methods

    using P::figure;
    using P::make_figure;

    using P::spec;
    using P::spec_str;

    // -- Overridden methods

    inline Point fwd(const Point& p) const override { return P::inv(p); }
    inline Point inv(const Point& p) const override { return P::fwd(p); }

private:

    // -- Overridden methods

    void fill_spec(spec::Custom& custom) const override {
        P::fill_spec(custom);
        custom.set("projection", "reverse_" + custom.get_string("projection"));
    }
};

}  // namespace eckit::geo::projection
