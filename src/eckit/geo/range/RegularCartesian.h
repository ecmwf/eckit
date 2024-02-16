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

#include "eckit/geo/range/Regular.h"


namespace eckit::geo::range {


class RegularCartesian final : public Regular {
public:
    // -- Constructors

    RegularCartesian(size_t n, double a, double b, double eps = 0.) :
        Regular(n, a, b, eps) {}

    // -- Overridden methods

    Range* crop(double crop_a, double crop_b) const override;
    Range* flip() const override;
};


}  // namespace eckit::geo::range