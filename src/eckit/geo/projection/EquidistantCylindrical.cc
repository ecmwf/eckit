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


#include "eckit/geo/projection/EquidistantCylindrical.h"

#include <cmath>

#include "eckit/geo/spec/Custom.h"
#include "eckit/geo/util.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo::projection {


static ProjectionRegisterType<EquidistantCylindrical> PROJECTION("eqc");


EquidistantCylindrical::EquidistantCylindrical(const Spec& spec) :
    EquidistantCylindrical(spec.get_double("lat_ts", 0.), spec.get_double("lat_0", 0.)) {}


EquidistantCylindrical::EquidistantCylindrical(double lat_ts, double lat_0) : lat_ts_(lat_ts), lat_0_(lat_0) {
    auto validate_latitude = [](double lat) {
        return types::is_strictly_greater(NORTH_POLE.lat, lat, PointLonLat::EPS) &&
               types::is_strictly_greater(lat, SOUTH_POLE.lat, PointLonLat::EPS);
    };

    ASSERT(validate_latitude(lat_ts_));
    ASSERT(validate_latitude(lat_0_));

    struct General final : Implementation {
        explicit General(double lat_ts, double lat_0) :
            cos_lat_ts_(std::cos(util::DEGREE_TO_RADIAN * lat_ts)), inv_cos_lat_ts_(1. / cos_lat_ts_), lat_0_(lat_0) {}
        PointXY fwd(const PointLonLat& p) const override { return {p.lon * cos_lat_ts_, p.lat - lat_0_}; }
        PointLonLat inv(const PointXY& q) const override { return {q.X * inv_cos_lat_ts_, q.Y + lat_0_}; }
        const double cos_lat_ts_;
        const double inv_cos_lat_ts_;
        const double lat_0_;
    };

    struct Specific final : Implementation {
        explicit Specific(double lat_0) : lat_0_(lat_0) {}
        PointXY fwd(const PointLonLat& p) const override { return {p.lon, p.lat - lat_0_}; }
        PointLonLat inv(const PointXY& q) const override { return {q.X, q.Y + lat_0_}; }
        const double lat_0_;
    };

    impl_.reset(types::is_approximately_equal(lat_ts_, 0.) ? static_cast<Implementation*>(new Specific(lat_0_))
                                                           : new General(lat_ts_, lat_0_));
}


const std::string& EquidistantCylindrical::type() const {
    static const std::string type{"eqc"};
    return type;
}


void EquidistantCylindrical::fill_spec(spec::Custom& custom) const {
    auto custom_angle = [&custom](const std::string& key, double angle) {
        if (!types::is_approximately_equal(angle, 0., PointLonLat::EPS)) {
            custom.set(key, angle);
        }
    };

    custom_angle("lat_ts", lat_ts_);
    custom_angle("lat_0", lat_0_);
    custom.set("type", type());
}


}  // namespace eckit::geo::projection
