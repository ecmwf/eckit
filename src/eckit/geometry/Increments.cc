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


#include "eckit/geometry/Increments.h"

#include "eckit/config/Configuration.h"


namespace eckit::geometry {


Increments::Increments(const Configuration& config) :
    Increments(config.getDouble("west_east_increment"), config.getDouble("south_north_increment")) {
}


}  // namespace eckit::geometry