// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// eckit geo Grid bridge: implementation.

#include "eckit_exceptions.h"

#include "AreaWrapper.h"
#include "GridWrapper.h"
#include "RustPoint.h"
#include "RustVec.h"
#include "SearchWrapper.h"
#include "eckit-sys/src/geo.rs.h"

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/Point.h"
#include "eckit/geo/Range.h"
#include "eckit/geo/area/BoundingBox.h"
#include "eckit/geo/cache/SearchCache.h"
#include "eckit/geo/grid/Regular.h"
#include "eckit/geo/grid/SphericalHarmonics.h"
#include "eckit/geo/grid/Unstructured.h"
#include "eckit/geo/grid/reduced/HEALPix.h"
#include "eckit/geo/grid/reduced/ReducedGaussian.h"
#include "eckit/geo/grid/reduced/ReducedLonLat.h"
#include "eckit/geo/grid/regular/RegularGaussian.h"
#include "eckit/geo/grid/regular/RegularLL.h"
#include "eckit/geo/projection/LonLatToXYZ.h"
#include "eckit/geo/util.h"
#include "eckit/parser/YAMLParser.h"
#include "eckit/spec/Custom.h"

#include <algorithm>
#include <string>
#include <utility>
#include <vector>

namespace eckit_bridge {

namespace {

using eckit::geo::Grid;
using eckit::geo::Range;

template <typename... Ts>
bool is_any_of(const Grid& grid) {
    return (... || (dynamic_cast<const Ts*>(&grid) != nullptr));
}

/// Calls `f(ny, nx)` with the grid's row structure, where `nx(j)` is the number
/// of points in row `j`. Does nothing for grids with no row structure.
///
/// `grid::Reduced` keeps `nxj` protected and has no `pl()`, and named grids
/// like `{"grid":"O1280"}` do not carry "pl" in their spec, so the only route
/// is a cast to the concrete class.
template <typename F>
void with_rows(const Grid& grid, F&& f) {
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

/// The grid types defining a scanning order; the base class throws `NotImplemented` (which prints a backtrace).
bool has_order(const Grid& grid) {
    using namespace eckit::geo::grid;
    return is_any_of<Regular, reduced::ReducedGaussian, reduced::ReducedLonLat, reduced::HEALPix>(grid);
}

/// The grid types whose rows share their latitudes (as `Grid::lat`).
bool has_latitudes(const Grid& grid) {
    using namespace eckit::geo::grid;
    return is_any_of<regular::RegularLL, regular::RegularGaussian, reduced::ReducedGaussian, reduced::ReducedLonLat,
                     reduced::HEALPix>(grid);
}

/// The grid types whose columns share their longitudes (as `Grid::lon`).
bool has_longitudes(const Grid& grid) {
    using namespace eckit::geo::grid;
    return is_any_of<regular::RegularLL, regular::RegularGaussian>(grid);
}

// Copied, not borrowed: `Range::values()` may return a reference into the
// process-global memory cache, which `MemoryCache::total_purge()` frees.
rust::Vec<double> values(const Range& range) {
    return RustVec::copy<double>(range.values());
}

void check_size(const char* method, size_t n, size_t have) {
    if (have < n) {
        throw eckit::UserError(
            std::string("GridWrapper::") + method + ": buffers too small for " + std::to_string(n) + " points", Here());
    }
}

std::unique_ptr<GridWrapper> adopt(const Grid* grid) {
    return std::make_unique<GridWrapper>(std::unique_ptr<const Grid>(grid));
}

}  // namespace

//----------------------------------------------------------------------------------------------------------------------

// ============== Identity ==============

const SpecWrapper& GridWrapper::catalog() const {
    if (!catalog_) {
        catalog_.emplace(grid_->catalog());
    }
    return *catalog_;
}

rust::String GridWrapper::uid() const {
    return rust::String(grid_->uid());
}

rust::String GridWrapper::grid_type() const {
    return rust::String(grid_->type());
}

rust::String GridWrapper::name() const {
    return rust::String(grid_->name());
}

rust::String GridWrapper::arrangement() const {
    return rust::String(grid_->arrangement());
}

bool GridWrapper::equals(const GridWrapper& other) const {
    return *grid_ == *other.grid_;
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

size_t GridWrapper::nx() const {
    const auto shape = grid_->shape();
    return shape.size() == 2 ? shape[1] : 0;
}

bool GridWrapper::is_regular() const {
    return is_any_of<eckit::geo::grid::Regular>(*grid_);
}

size_t GridWrapper::truncation() const {
    const auto* g = dynamic_cast<const eckit::geo::grid::SphericalHarmonics*>(grid_.get());
    return g == nullptr ? 0 : g->truncation();
}

// ============== Order ==============

rust::String GridWrapper::order() const {
    if (!has_order(*grid_)) {
        throw eckit::geo::exception::OrderError("Grid: order is not defined for type '" + grid_->type() + "'", Here());
    }
    return rust::String(grid_->order());
}

rust::Vec<size_t> GridWrapper::reorder(rust::Str to) const {
    if (!has_order(*grid_)) {
        throw eckit::geo::exception::OrderError("Grid: reorder is not defined for type '" + grid_->type() + "'",
                                                Here());
    }
    return RustVec::copy<size_t>(grid_->reorder(std::string(to)));
}

// ============== Geometry ==============

std::unique_ptr<AreaWrapper> GridWrapper::bounding_box() const {
    return std::make_unique<AreaWrapper>(std::make_unique<eckit::geo::area::BoundingBox>(grid_->boundingBox()));
}

RawPoint GridWrapper::first_point() const {
    return RustPoint::to_raw(grid_->first_point());
}

RawPoint GridWrapper::last_point() const {
    return RustPoint::to_raw(grid_->last_point());
}

rust::Vec<rust::String> GridWrapper::point_coordinates() const {
    return RustVec::strings(grid_->point_coordinates());
}

rust::Vec<double> GridWrapper::distinct_latitudes() const {
    return has_latitudes(*grid_) ? values(grid_->lat()) : rust::Vec<double>{};
}

rust::Vec<double> GridWrapper::distinct_longitudes() const {
    return has_longitudes(*grid_) ? values(grid_->lon()) : rust::Vec<double>{};
}

rust::Vec<double> GridWrapper::x_values() const {
    return is_regular() ? values(grid_->x()) : rust::Vec<double>{};
}

rust::Vec<double> GridWrapper::y_values() const {
    return is_regular() ? values(grid_->y()) : rust::Vec<double>{};
}

// Point by point, not through `Grid::to_latlons`/`to_xyz`, which allocate every coordinate array before copying.

void GridWrapper::fill_points(rust::Slice<RawPoint> points) const {
    check_size("fill_points", grid_->size(), points.size());

    size_t i = 0;
    std::for_each(grid_->cbegin(), grid_->cend(), [&](const auto& point) { points[i++] = RustPoint::to_raw(point); });
}

void GridWrapper::fill_latlons(rust::Slice<double> lat, rust::Slice<double> lon) const {
    using namespace eckit::geo;

    check_size("fill_latlons", grid_->size(), std::min(lat.size(), lon.size()));

    size_t i = 0;
    std::for_each(grid_->cbegin(), grid_->cend(), [&](const auto& point) {
        if (const auto* p = std::get_if<PointLonLat>(&point)) {
            lat[i] = p->lat();
            lon[i] = p->lon();
        }
        else if (const auto* r = std::get_if<PointLonLatR>(&point)) {
            lat[i] = util::RADIAN_TO_DEGREE * r->latr();
            lon[i] = util::RADIAN_TO_DEGREE * r->lonr();
        }
        else {
            throw exception::GridError("GridWrapper::fill_latlons: grid points are not (lon, lat)", Here());
        }
        ++i;
    });
}

void GridWrapper::fill_xyz(rust::Slice<double> x, rust::Slice<double> y, rust::Slice<double> z) const {
    using namespace eckit::geo;

    check_size("fill_xyz", grid_->size(), std::min({x.size(), y.size(), z.size()}));

    const auto& figure = grid_->figure();
    const projection::LonLatToXYZ to_xyz(figure.a(), figure.b());

    size_t i = 0;
    std::for_each(grid_->cbegin(), grid_->cend(), [&](const auto& point) {
        const auto q =
            std::holds_alternative<PointXYZ>(point)      ? std::get<PointXYZ>(point)
            : std::holds_alternative<PointLonLat>(point) ? to_xyz.fwd(std::get<PointLonLat>(point))
            : std::holds_alternative<PointLonLatR>(point)
                ? to_xyz.fwd(PointLonLat{util::RADIAN_TO_DEGREE * std::get<PointLonLatR>(point).lonr(),
                                         util::RADIAN_TO_DEGREE * std::get<PointLonLatR>(point).latr()})
                : throw exception::GridError("GridWrapper::fill_xyz: grid points are not geographic", Here());
        x[i] = q.X();
        y[i] = q.Y();
        z[i] = q.Z();
        ++i;
    });
}

// ============== Derived grids ==============

std::unique_ptr<GridWrapper> GridWrapper::to_unstructured_ll(rust::Str name) const {
    return adopt(grid_->to_unstructured_ll(std::string(name)));
}

rust::Vec<size_t> GridWrapper::crop(const AreaWrapper& area) const {
    return RustVec::copy<size_t>(grid_->crop(area.inner()));
}

std::unique_ptr<GridWrapper> GridWrapper::make_grid_cropped(const AreaWrapper& area) const {
    return adopt(grid_->make_grid_cropped(area.inner()));
}

std::unique_ptr<GridWrapper> GridWrapper::make_grid_cropped_unstructured_ll(const AreaWrapper& area) const {
    return adopt(grid_->make_grid_cropped_unstructured_ll(area.inner()));
}

// ============== Search ==============

std::unique_ptr<SearchWrapper> GridWrapper::search() const {
    return std::make_unique<SearchWrapper>(eckit::geo::cache::SearchCache::instance().get(*grid_));
}

std::unique_ptr<SearchWrapper> GridWrapper::search_with_spec(rust::Str spec) const {
    const std::unique_ptr<const eckit::spec::Spec> custom(
        eckit::spec::Custom::make_from_value(eckit::YAMLParser::decodeString(std::string(spec))));
    return std::make_unique<SearchWrapper>(eckit::geo::cache::SearchCache::instance().get(*grid_, *custom));
}

// ============== Factories ==============

std::unique_ptr<GridWrapper> GridWrapper::from_spec(rust::Str spec) {
    return adopt(eckit::geo::GridFactory::make_from_string(std::string(spec)));
}

std::unique_ptr<GridWrapper> GridWrapper::from_latlons(rust::Slice<const double> lat, rust::Slice<const double> lon,
                                                       rust::Str name) {
    if (lat.size() != lon.size()) {
        throw eckit::UserError("GridWrapper::from_latlons: " + std::to_string(lat.size()) + " latitudes, " +
                                   std::to_string(lon.size()) + " longitudes",
                               Here());
    }
    return adopt(new eckit::geo::grid::Unstructured(RustVec::to_std(lon), RustVec::to_std(lat), std::string(name)));
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit_bridge
