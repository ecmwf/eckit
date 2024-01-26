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

#include "eckit/geo/spec/Custom.h"


namespace eckit::geo::projection {


static ProjectionBuilder<None> __projection1("");
static ProjectionBuilder<None> __projection2("none");
static ProjectionBuilder<None> __projection3("equirectangular");
static ProjectionBuilder<PlateCaree> __projection4("plate-carree");


None::None() :
    fwd_(new NoPointConversion), inv_(new NoPointConversion) {}


None::None(const std::string& source, const std::string& target) :
    fwd_(new_point_conversion(source, target)), inv_(new_point_conversion(target, source)) {}


None::None(const Spec& spec) :
    None(spec.get_string("source", "xy"), spec.get_string("target", "xy")) {}


Spec* None::spec() const {
    return new spec::Custom({{"type", "none"}, {"source", "xy"}, {"target", "lonlat"}});
}


None::Implementation* None::new_point_conversion(const std::string& source, const std::string& target) {
    ASSERT(source == "xy" || source == "lonlat");
    return source == "xy" && target == "lonlat"   ? static_cast<Implementation*>(new Point2ToPointLonLat)
           : source == "lonlat" && target == "xy" ? static_cast<Implementation*>(new PointLonLatToPoint2)
                                                  : new NoPointConversion;
}


Spec* PlateCaree::spec() const {
    return new spec::Custom({{"type", "plate-carree"}});
}


}  // namespace eckit::geo::projection
