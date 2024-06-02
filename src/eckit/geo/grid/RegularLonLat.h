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

#include "eckit/geo/PointLonLat.h"
#include "eckit/geo/grid/Regular.h"


namespace eckit::geo::grid {


class RegularLonLat : public Regular {
public:
    // -- Constructors

    using Regular::Regular;

    // -- Methods

    double dlon() const { return dx(); }
    double dlat() const { return dy(); }

    size_t nlon() const { return x().size(); }
    size_t nlat() const { return y().size(); }

    // -- Overridden methods

    void spec(spec::Custom&) const override;
};


}  // namespace eckit::geo::grid
