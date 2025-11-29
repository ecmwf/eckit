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

#include "eckit/geo/util.h"
#include "eckit/spec/Custom.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo::projection {


static ProjectionRegisterType<EquidistantCylindrical> PROJECTION1("eqc");
static ProjectionRegisterType<EquidistantCylindrical> PROJECTION2("plate-carree");


EquidistantCylindrical::EquidistantCylindrical(const Spec& spec) :
    EquidistantCylindrical(spec.get_double("lat_ts", 0.), spec.get_double("lat_0", 0.)) {}


EquidistantCylindrical::EquidistantCylindrical(double lat_ts, double lat_0) :
    lat_ts_(types::is_approximately_equal(lat_ts, 0., PointLonLat::EPS) ? 0. : lat_ts), lat_0_(lat_0) {
    auto cos_lat_ts = std::cos(util::DEGREE_TO_RADIAN * lat_ts_);
    ASSERT(!types::is_approximately_equal(0., cos_lat_ts));

    struct General final : Implementation {
        explicit General(double cos_lat_ts, double lat_0) :
            cos_lat_ts_(cos_lat_ts), inv_cos_lat_ts_(1. / cos_lat_ts_), lat_0_(lat_0) {}
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

    impl_.reset(types::is_approximately_equal(lat_ts_, 0., PointLonLat::EPS)
                    ? static_cast<Implementation*>(new Specific(lat_0_))
                    : new General{cos_lat_ts, lat_0_});
}


const std::string& EquidistantCylindrical::type() const {
    static const std::string type{"eqc"};
    return type;
}


void EquidistantCylindrical::fill_spec(spec::Custom& custom) const {
    // this is the default projection so spec is only set with non-default parameters
    if (auto set_lat_ts = !types::is_approximately_equal(lat_ts_, 0., PointLonLat::EPS),
        set_lat_0       = !types::is_approximately_equal(lat_0_, 0., PointLonLat::EPS);
        set_lat_ts || set_lat_0) {
        if (set_lat_ts) {
            custom.set("lat_ts", lat_ts_);
        }

        if (set_lat_0) {
            custom.set("lat_0", lat_0_);
        }

        custom.set("type", type());
    }
}


}  // namespace eckit::geo::projection
