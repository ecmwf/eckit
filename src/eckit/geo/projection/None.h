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


namespace eckit::geo::projection {


class None : public Projection {
public:
    // -- Constructors

    explicit None() = default;
    explicit None(const Spec&) {}

    // -- Overridden methods

    inline Point fwd(const Point& p) const override { return p; }
    inline Point inv(const Point& q) const override { return q; }

    void fill_spec(spec::Custom&) const override {}
};


}  // namespace eckit::geo::projection
