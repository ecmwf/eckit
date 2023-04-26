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


#include "grit/projection/PROJ.h"

#include "grit/exception.h"


namespace grit::projection {


PROJ::PROJ(const std::string& source, const std::string& target) :
    proj_(nullptr), ctx_(PJ_DEFAULT_CTX), source_(source), target_(target) {
    ASSERT(!source.empty());

    // projection, normalised
    pj_t p(proj_create_crs_to_crs(ctx_.get(), source_.c_str(), target_.c_str(), nullptr));
    ASSERT(p);

    proj_.reset(proj_normalize_for_visualization(ctx_.get(), p.get()));
    ASSERT(proj_);
}


std::string PROJ::ellipsoid() const {
    pj_t identity(proj_create_crs_to_crs(ctx_.get(), target_.c_str(), target_.c_str(), nullptr));
    pj_t proj(proj_get_target_crs(ctx_.get(), identity.get()));

    if (pj_t ellipsoid(proj_get_ellipsoid(ctx_.get(), proj.get())); ellipsoid) {
        double a = 0;
        double b = 0;
        ASSERT(proj_ellipsoid_get_parameters(ctx_.get(), ellipsoid.get(), &a, &b, nullptr, nullptr));
        ASSERT(0 < b && b <= a);
        return b < a ? "+proj=lonlat +a=" + std::to_string(a) + " +b=" + std::to_string(b)
                     : "+proj=lonlat +R=" + std::to_string(a);
    }

    // EPSG:4326 -> WGS84 (lat,lon), EPSG:4978 -> WGS84 (x,y,z)
    return "EPSG:4326";
}


template <class T>
PJ_COORD __from(const T&) {
    NOTIMP;
}


template <>
PJ_COORD __from(const PointLatLon& p) {
    return proj_coord(p.lon, p.lat, 0, 0);
}


template <>
PJ_COORD __from(const PointXY& p) {
    return proj_coord(p.x, p.y, 0, 0);
}


template <>
PJ_COORD __from(const PointXYZ& p) {
    return proj_coord(p.x, p.y, p.z, 0);
}


template <class T>
T __to(const PJ_COORD&) {
    NOTIMP;
}


template <>
PointLatLon __to(const PJ_COORD& c) {
    return PointLatLon::make(c.enu.n, c.enu.e);
}


template <>
PointXY __to(const PJ_COORD& c) {
    return {c.xy.x, c.xy.y};
}


template <>
PointXYZ __to(const PJ_COORD& c) {
    return {c.xyz.x, c.xyz.y, c.xyz.z};
}


template <class PointSource, class PointTarget>
PointTarget PROJTT<PointSource, PointTarget>::fwd(const PointSource& p) const {
    return __to<PointTarget>(proj_trans(PROJ::proj().get(), PJ_FWD, __from(p)));
}


template <class PointSource, class PointTarget>
PointSource PROJTT<PointSource, PointTarget>::inv(const PointTarget& q) const {
    return __to<PointSource>(proj_trans(PROJ::proj().get(), PJ_INV, __from(q)));
}


ProjectionBuilder<PROJ_LatLon_to_LatLon> __PROJ_LatLon_to_LatLon("PROJ_LatLon_to_LatLon");
ProjectionBuilder<PROJ_LatLon_to_XY> __PROJ_LatLon_to_XY("PROJ_LatLon_to_XY ");
ProjectionBuilder<PROJ_LatLon_to_XYZ> __PROJ_LatLon_to_XYZ("PROJ_LatLon_to_XYZ");
ProjectionBuilder<PROJ_XY_to_LatLon> __PROJ_XY_to_LatLon("PROJ_XY_to_LatLon");
ProjectionBuilder<PROJ_XY_to_XY> __PROJ_XY_to_XY("PROJ_XY_to_XY ");
ProjectionBuilder<PROJ_XY_to_XYZ> __PROJ_XY_to_XYZ("PROJ_XY_to_XYZ");
ProjectionBuilder<PROJ_XYZ_to_LatLon> __PROJ_XYZ_to_LatLon("PROJ_XYZ_to_LatLon");
ProjectionBuilder<PROJ_XYZ_to_XY> __PROJ_XYZ_to_XY("PROJ_XYZ_to_XY ");
ProjectionBuilder<PROJ_XYZ_to_XYZ> __PROJ_XYZ_to_XYZ("PROJ_XYZ_to_XYZ");


}  // namespace grit::projection
