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

#include "eckit/geo/grid/RegularXY.h"


namespace eckit::geo::grid::regularxy {


class LambertAzimuthalEqualArea final : public RegularXY {
public:

    // -- Constructors

    explicit LambertAzimuthalEqualArea(const Spec& spec) : RegularXY(RegularXY::make_ranges_from_spec(spec)) {}

    // -- Overridden methods

    void fill_spec(spec::Custom& custom) const override;
    const std::string& type() const override;
};


}  // namespace eckit::geo::grid::regularxy
