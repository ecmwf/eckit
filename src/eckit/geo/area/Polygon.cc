// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/area/Polygon.h"

#include <algorithm>
#include <memory>
#include <numeric>

#include "eckit/geo/Exceptions.h"
#include "eckit/spec/Custom.h"


namespace eckit::geo::area {


static const AreaRegisterType<Polygon> AREATYPE("polygon");


Polygon::Polygon(const Area::Spec& spec) : Polygon(*std::unique_ptr<Polygon>(make_from_spec(spec))) {}


bool Polygon::intersects(BoundingBox&) const {
    NOTIMP;
}


bool Polygon::contains(const Point& p) const {
    // NOTE This implementation follows that polygon rings alternatingly include/exclude the point (typical in GeoJSON);
    // In shapefile polygons (not here) the first ring tests inclusion and subsequent rings test exclusion, but point
    // ordering is assumed to be respected (ccw for inclusion, cw for exclusion) which isn't always the case
    const auto& q = std::get<PointLonLat>(p);

    auto c = std::count_if(begin(), end(), [&q](const auto& poly) { return poly.contains(q); });
    return c % 2 != 0;
}


double Polygon::area() const {
    // NOTE this is "signed area", meaning it respects the point ordering
    return std::accumulate(begin(), end(), 0., [](auto a, const auto& poly) { return a + poly.area(); });
}


const std::string& Polygon::type() const {
    static const std::string type{"polygon"};
    return type;
}


void Polygon::fill_spec(spec::Custom& custom) const {
    custom.set("type", type());
    custom.set(type(), "NOTIMP");
}


Polygon* Polygon::make_from_spec(const Area::Spec& spec) {
    NOTIMP;
}


}  // namespace eckit::geo::area
