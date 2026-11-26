// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/area/Polygon.h"

#include <algorithm>
#include <cstddef>
#include <memory>
#include <numeric>
#include <utility>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/Grid.h"
#include "eckit/geo/grid/Unstructured.h"
#include "eckit/spec/Custom.h"
#include "eckit/utils/MD5.h"


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


namespace {


std::pair<std::vector<double>, std::vector<double>> ring_latlons(const polygon::Polygon& ring) {
    std::vector<double> latitudes;
    std::vector<double> longitudes;
    latitudes.reserve(ring.size());
    longitudes.reserve(ring.size());

    for (const auto& p : ring) {
        latitudes.push_back(p.lat());
        longitudes.push_back(p.lon());
    }

    return {latitudes, longitudes};
}


}  // namespace


std::string Polygon::uid() const {
    MD5 md5;
    for (const auto& ring : *this) {
        const auto [latitudes, longitudes] = ring_latlons(ring);
        md5.add(grid::Unstructured::uid_from_latlons(latitudes, longitudes));
    }
    return md5.digest();
}


void Polygon::fill_spec(spec::Custom& custom) const {
    std::vector<std::string> uids;
    uids.reserve(size());

    for (const auto& ring : *this) {
        const auto [latitudes, longitudes] = ring_latlons(ring);
        uids.emplace_back(grid::Unstructured(longitudes, latitudes).uid());
    }

    custom.set("type", type());
    if (uids.size() == 1) {
        custom.set("uid", uids.front());
    }
    else {
        custom.set("uid", uids);
    }
}


Polygon* Polygon::make_from_spec(const Area::Spec& spec) {
    std::vector<std::unique_ptr<const Grid>> grids;

    if (spec.has("latitudes") || spec.has("longitudes")) {
        std::vector<double> latitudes;
        std::vector<double> longitudes;
        if (!spec.get("latitudes", latitudes) || !spec.get("longitudes", longitudes) ||
            latitudes.size() != longitudes.size()) {
            throw exception::AreaError("Polygon: 'latitudes' and 'longitudes' expected, of the same size", Here());
        }

        grids.emplace_back(new grid::Unstructured(longitudes, latitudes));
    }
    else if (std::vector<std::string> uids; spec.get("uid", uids) || spec.has("uid")) {
        if (uids.empty()) {
            uids.emplace_back(spec.get_string("uid"));
        }

        for (const auto& uid : uids) {
            grids.emplace_back(GridFactory::build(spec::Custom{{"uid", uid}}));
        }
    }
    else {
        throw exception::AreaError("Polygon: 'latitudes' and 'longitudes', or 'uid' (one or more) expected", Here());
    }

    container_type rings;
    rings.reserve(grids.size());

    for (const auto& grid : grids) {
        if (grid->type() != "unstructured_ll" || grid->size() < 3) {
            throw exception::AreaError("Polygon: rings of 3 points or more expected", Here());
        }

        const auto [latitudes, longitudes] = grid->to_latlons();

        value_type ring;
        ring.reserve(latitudes.size());
        for (size_t i = 0; i < latitudes.size(); ++i) {
            ring.emplace_back(longitudes[i], latitudes[i]);
        }

        rings.emplace_back(std::move(ring));
    }

    return new Polygon(std::move(rings));
}


}  // namespace eckit::geo::area
