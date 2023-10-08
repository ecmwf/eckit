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


#include "eckit/geo/projection/None.h"

#include "eckit/config/MappedConfiguration.h"


namespace eckit::geo::projection {


static ProjectionBuilder<None> __projection1("");
static ProjectionBuilder<None> __projection2("none");
static ProjectionBuilder<None> __projection3("equirectangular");
static ProjectionBuilder<None> __projection4("plate-carree");


None::None(const Configuration&) {}


Projection::Spec None::spec() const {
    return Spec{};
}


}  // namespace eckit::geo::projection
