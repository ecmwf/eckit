// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#ifndef UnitSphere_H
#define UnitSphere_H

#include "eckit/geometry/SphereT.h"

//------------------------------------------------------------------------------------------------------

namespace eckit::geometry {

//------------------------------------------------------------------------------------------------------

/// Definition of a unit datum
struct DatumUnit {

    /* C++-11:
    static constexpr double radius() {
        return 1.;
    }
    */

    static double radius() { return 1.; }
};

//------------------------------------------------------------------------------------------------------

/// Definition of a unit sphere
using UnitSphere = SphereT<DatumUnit>;

//------------------------------------------------------------------------------------------------------

}  // namespace eckit::geometry

#endif
