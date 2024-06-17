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


struct SpaceView final : public RegularXY {
    explicit SpaceView(const Spec& spec) :
        RegularXY(RegularXY::make_xy_ranges_from_spec(spec), area::BoundingBox{spec}) {}

    void fill_spec(spec::Custom& custom) const override;
};


}  // namespace eckit::geo::grid::regularxy
