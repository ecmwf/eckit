// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "RustPoint.h"
#include "RustVec.h"
#include "eckit-sys/src/geo.rs.h"

#include "eckit/geo/Exceptions.h"

namespace eckit_bridge {

//----------------------------------------------------------------------------------------------------------------------

RawPoint RustPoint::to_raw(const eckit::geo::Point& point) {
    using namespace eckit::geo;

    if (const auto* p = std::get_if<PointXY>(&point)) {
        return {PointKind::Xy, {p->X(), p->Y(), 0.}};
    }
    if (const auto* p = std::get_if<PointXYZ>(&point)) {
        return {PointKind::Xyz, {p->X(), p->Y(), p->Z()}};
    }
    if (const auto* p = std::get_if<PointLonLat>(&point)) {
        return {PointKind::LonLat, {p->lon(), p->lat(), 0.}};
    }
    if (const auto* p = std::get_if<PointLonLatR>(&point)) {
        return {PointKind::LonLatR, {p->lonr(), p->latr(), 0.}};
    }
    throw exception::GridError("RustPoint: point holds no value", Here());
}

eckit::geo::Point RustPoint::from_raw(const RawPoint& raw) {
    using namespace eckit::geo;

    const auto& c = raw.coords;
    switch (raw.kind) {
        case PointKind::Xy:
            return PointXY{c[0], c[1]};
        case PointKind::Xyz:
            return PointXYZ{c[0], c[1], c[2]};
        case PointKind::LonLat:
            return PointLonLat{c[0], c[1]};
        case PointKind::LonLatR:
            return PointLonLatR{c[0], c[1]};
    }
    throw exception::GridError("RustPoint: unknown point kind", Here());
}

rust::Vec<rust::String> RustPoint::names(const std::vector<std::string>& names) {
    return RustVec::strings(names);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit_bridge
