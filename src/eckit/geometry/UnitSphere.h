/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

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
typedef SphereT<DatumUnit> UnitSphere;

//------------------------------------------------------------------------------------------------------

}  // namespace eckit::geometry

#endif
