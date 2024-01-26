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

static const std::string LONLAT("lonlat");
static const std::string XY("xy");


None::None() :
    fwd_(new NoPointConversion), inv_(new NoPointConversion) {}


None::None(const std::string& source, const std::string& target) :
    fwd_(new_point_conversion(source, target)), inv_(new_point_conversion(target, source)) {}


None::None(const Spec& spec) :
    None(spec.get_string("source", XY), spec.get_string("target", XY)) {}


Spec* None::spec() const {
    return new spec::Custom({{"type", "none"}, {"source", XY}, {"target", LONLAT}});
}


None::Implementation* None::new_point_conversion(const std::string& source, const std::string& target) {
    ASSERT(source == XY || source == LONLAT);
    return source == XY && target == LONLAT   ? static_cast<Implementation*>(new Point2ToPointLonLat)
           : source == LONLAT && target == XY ? static_cast<Implementation*>(new PointLonLatToPoint2)
                                              : new NoPointConversion;
}


PlateCaree::PlateCaree() :
    None(XY, LONLAT) {}


PlateCaree::PlateCaree(const Spec&) :
    PlateCaree() {}


Spec* PlateCaree::spec() const {
    return new spec::Custom({{"type", "plate-carree"}});
}


}  // namespace eckit::geo::projection
