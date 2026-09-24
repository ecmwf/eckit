// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// eckit geo Grid bridge — implementation.

#include "eckit_exceptions.h"

#include "GridWrapper.h"
#include "RustVec.h"
#include "eckit-sys/src/geo.rs.h"

#include "eckit/geo/Point.h"
#include "eckit/geo/PointLonLat.h"
#include "eckit/geo/Range.h"
#include "eckit/geo/area/BoundingBox.h"
#include "eckit/geo/grid/Regular.h"
#include "eckit/geo/grid/reduced/HEALPix.h"
#include "eckit/geo/grid/reduced/ReducedGaussian.h"
#include "eckit/geo/grid/reduced/ReducedLonLat.h"

#include <algorithm>
#include <string>
#include <utility>

namespace eckit_bridge {

namespace {

LonLat to_lonlat(const eckit::geo::Point& point) {
    const auto& p = std::get<eckit::geo::PointLonLat>(point);
    return {p.lon(), p.lat()};
}

/// Calls `f(ny, nx)` with the grid's row structure, where `nx(j)` is the number
/// of points in row `j`. Does nothing for grids with no row structure.
///
/// `grid::Reduced` keeps `nxj` protected and has no `pl()`, and named grids
/// like `{"grid":"O1280"}` do not carry "pl" in their spec, so the only route
/// is a cast to the concrete class.
template <typename F>
void with_rows(const eckit::geo::Grid& grid, F&& f) {
    using namespace eckit::geo::grid;

    if (const auto* g = dynamic_cast<const reduced::ReducedGaussian*>(&grid)) {
        f(g->pl().size(), [g](size_t j) { return g->pl()[j]; });
    }
    else if (const auto* g = dynamic_cast<const reduced::ReducedLonLat*>(&grid)) {
        f(g->pl().size(), [g](size_t j) { return g->pl()[j]; });
    }
    else if (const auto* g = dynamic_cast<const reduced::HEALPix*>(&grid)) {
        f(g->ny(), [g](size_t j) { return g->nxj(j); });
    }
    else if (const auto* g = dynamic_cast<const Regular*>(&grid)) {
        f(g->ny(), [g](size_t /*j*/) { return g->nx(); });
    }
}

}  // namespace

//----------------------------------------------------------------------------------------------------------------------

// ============== Identity ==============

rust::String GridWrapper::catalog_str() const {
    return rust::String(grid_->catalog_str());
}

rust::String GridWrapper::uid() const {
    return rust::String(grid_->uid());
}

rust::String GridWrapper::grid_type() const {
    return rust::String(grid_->type());
}

// ============== Shape ==============

size_t GridWrapper::size() const {
    return grid_->size();
}

bool GridWrapper::is_empty() const {
    return grid_->empty();
}

rust::Vec<size_t> GridWrapper::shape() const {
    return RustVec::copy<size_t>(grid_->shape());
}

rust::Vec<std::int64_t> GridWrapper::pl() const {
    rust::Vec<std::int64_t> out;
    with_rows(*grid_, [&out](size_t ny, auto&& nx) {
        out.reserve(ny);
        for (size_t j = 0; j < ny; ++j) {
            out.push_back(static_cast<std::int64_t>(nx(j)));
        }
    });
    return out;
}

size_t GridWrapper::ny() const {
    size_t ny = 0;
    with_rows(*grid_, [&ny](size_t n, auto&& /*nx*/) { ny = n; });
    return ny;
}

// ============== Geometry ==============

Bbox GridWrapper::bounding_box() const {
    const auto& bbox = grid_->boundingBox();
    return {bbox.north(), bbox.west(), bbox.south(), bbox.east()};
}

LonLat GridWrapper::first_point() const {
    return to_lonlat(grid_->first_point());
}

LonLat GridWrapper::last_point() const {
    return to_lonlat(grid_->last_point());
}

// Copied, not borrowed: `Range::values()` may return a reference into the
// process-global memory cache, which `MemoryCache::total_purge()` frees.

rust::Vec<double> GridWrapper::distinct_latitudes() const {
    return RustVec::copy<double>(grid_->lat().values());
}

rust::Vec<double> GridWrapper::distinct_longitudes() const {
    return RustVec::copy<double>(grid_->lon().values());
}

void GridWrapper::fill_latlons(rust::Slice<double> lat, rust::Slice<double> lon) const {
    const auto n = grid_->size();
    if (lat.size() < n || lon.size() < n) {
        throw eckit::UserError("GridWrapper::fill_latlons: buffers too small for " + std::to_string(n) + " points",
                               Here());
    }

    size_t i = 0;
    std::for_each(grid_->cbegin(), grid_->cend(), [&](const auto& point) {
        const auto p = to_lonlat(point);
        lat[i]       = p.lat;
        lon[i]       = p.lon;
        ++i;
    });
}

// ============== Factories ==============

std::unique_ptr<GridWrapper> GridWrapper::from_spec(rust::Str spec) {
    // Adopt the owning pointer before anything else can throw.
    std::unique_ptr<const eckit::geo::Grid> grid(eckit::geo::GridFactory::make_from_string(std::string(spec)));
    return std::make_unique<GridWrapper>(std::move(grid));
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit_bridge
