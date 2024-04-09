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


#include "eckit/geo/grid/Regular.h"


namespace eckit::geo::grid {


Regular::Regular(const Spec& spec) :
    Grid(spec) {}


Regular::Regular(const area::BoundingBox& bbox) :
    Grid(bbox) {}


}  // namespace eckit::geo::grid
