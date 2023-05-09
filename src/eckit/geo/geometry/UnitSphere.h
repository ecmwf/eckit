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

#include "grit/geometry/SphereT.h"


namespace grit::geometry {


/// Definition of a unit datum
struct DatumUnit {
    static constexpr double radius() { return 1.; }
};


/// Definition of a unit sphere
using UnitSphere = SphereT<DatumUnit>;


}  // namespace grit::geometry
