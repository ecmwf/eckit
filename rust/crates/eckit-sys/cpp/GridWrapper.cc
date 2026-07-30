// eckit geo Grid bridge — implementation.

#include "eckit_exceptions.h"

#include "GridWrapper.h"
#include "eckit-sys/src/geo.rs.h"

#include "eckit/geo/Point.h"
#include "eckit/geo/PointLonLat.h"
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

}  // namespace

//----------------------------------------------------------------------------------------------------------------------

// ============== Identity ==============

rust::String GridWrapper::spec_str() const {
    return rust::String(grid_->spec_str());
}

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
    rust::Vec<size_t> out;
    for (auto n : grid_->shape()) {
        out.push_back(n);
    }
    return out;
}

rust::Vec<std::int64_t> GridWrapper::pl() const {
    using namespace eckit::geo::grid;

    rust::Vec<std::int64_t> out;

    // `grid::Reduced` keeps `nx`/`ny` protected and has no `pl()`, and named
    // grids like `{"grid":"O1280"}` do not carry "pl" in their spec, so the
    // only route is a cast to the concrete class.
    auto rows = [&out](size_t ny, auto&& nx) {
        out.reserve(ny);
        for (size_t j = 0; j < ny; ++j) {
            out.push_back(static_cast<std::int64_t>(nx(j)));
        }
    };

    if (const auto* g = dynamic_cast<const reduced::ReducedGaussian*>(grid_.get())) {
        rows(g->pl().size(), [g](size_t j) { return g->pl()[j]; });
    }
    else if (const auto* g = dynamic_cast<const reduced::ReducedLonLat*>(grid_.get())) {
        rows(g->pl().size(), [g](size_t j) { return g->pl()[j]; });
    }
    else if (const auto* g = dynamic_cast<const reduced::HEALPix*>(grid_.get())) {
        rows(g->ny(), [g](size_t j) { return g->nx(j); });
    }
    else if (const auto* g = dynamic_cast<const Regular*>(grid_.get())) {
        rows(g->ny(), [g](size_t /*j*/) { return g->nx(); });
    }

    return out;
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

rust::Vec<double> GridWrapper::distinct_latitudes() const {
    rust::Vec<double> out;
    for (auto v : grid_->distinct_latitudes()) {
        out.push_back(v);
    }
    return out;
}

rust::Vec<double> GridWrapper::distinct_longitudes() const {
    rust::Vec<double> out;
    for (auto v : grid_->distinct_longitudes()) {
        out.push_back(v);
    }
    return out;
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
