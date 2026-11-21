// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/projection/LonLatToXYZ.h"

#include "eckit/geo/Exceptions.h"
#include "eckit/spec/Custom.h"


namespace eckit::geo::projection {


static const std::string TYPE("ll-to-xyz");
static ProjectionRegisterType<LonLatToXYZ> PROJECTION(TYPE);


LonLatToXYZ::LonLatToXYZ(Figure* figure_ptr) : ToFromXYZ(figure_ptr, PointLonLat{}) {}


LonLatToXYZ::LonLatToXYZ(const Spec& spec) : LonLatToXYZ(FigureFactory::build(spec)) {}


PointLonLat LonLatToXYZ::inv(const PointXYZ& q) const {
    if (!figure().spherical()) {
        NOTIMP;
    }

    return to_lonlat(q);
}


const std::string& LonLatToXYZ::type() const {
    static const std::string type{TYPE};
    return type;
}


void LonLatToXYZ::fill_spec(spec::Custom& custom) const {
    Projection::fill_spec(custom);

    custom.set("type", TYPE);
}


std::vector<std::vector<double>> LonLatToXYZ::fwd_vector(const std::vector<double>& lon, const std::vector<double>& lat,
                                                         const std::vector<double>&) const {
    return to_xyz(lon, lat, {}, 0.);
}


}  // namespace eckit::geo::projection
