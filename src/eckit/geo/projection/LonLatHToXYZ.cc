// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/projection/LonLatHToXYZ.h"

#include <cmath>

#include "eckit/geo/Exceptions.h"
#include "eckit/spec/Custom.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo::projection {


static const std::string TYPE("llh-to-xyz");
static ProjectionRegisterType<LonLatHToXYZ> PROJECTION(TYPE);


LonLatHToXYZ::LonLatHToXYZ(Figure* figure_ptr, double height_scale_factor) :
    ToFromXYZ(figure_ptr, PointLonLatH{}),
    height_scale_factor_(std::isfinite(height_scale_factor) && height_scale_factor >= 0.
                             ? height_scale_factor
                             : throw exception::ProjectionError("LonLatHToXYZ: height_scale_factor >= 0", Here())) {}


LonLatHToXYZ::LonLatHToXYZ(const Spec& spec) :
    LonLatHToXYZ(FigureFactory::build(spec), spec.get_double("height_scale_factor", 1.)) {}


PointLonLatH LonLatHToXYZ::inv(const PointXYZ& q) const {
    if (!figure().spherical() || !(height_scale_factor_ > 0.)) {
        NOTIMP;
    }

    const auto p = to_lonlath(figure().a(), q);
    return {p.lonlat(), p.height() / height_scale_factor_};
}


const std::string& LonLatHToXYZ::type() const {
    static const std::string type{TYPE};
    return type;
}


void LonLatHToXYZ::fill_spec(spec::Custom& custom) const {
    Projection::fill_spec(custom);

    custom.set("type", TYPE);
    if (!types::is_approximately_equal(height_scale_factor_, 1.)) {
        custom.set("height_scale_factor", height_scale_factor_);
    }
}


std::vector<std::vector<double>> LonLatHToXYZ::fwd_vector(const std::vector<double>& lon,
                                                          const std::vector<double>& lat,
                                                          const std::vector<double>& height) const {
    return to_xyz(lon, lat, height, height_scale_factor_);
}


}  // namespace eckit::geo::projection
