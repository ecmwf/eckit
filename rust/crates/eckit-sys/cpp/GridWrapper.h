// eckit geo Grid bridge — wraps `eckit::geo::Grid`.
#pragma once

#include "eckit/geo/Grid.h"

#include "rust/cxx.h"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

namespace eckit_bridge {

struct Bbox;
struct LonLat;

//----------------------------------------------------------------------------------------------------------------------

/// Wraps `eckit::geo::Grid` for Rust FFI.
///
/// Grids are immutable and built through `eckit::geo::GridFactory`, which hands
/// back an owning pointer; the wrapper adopts it for its whole lifetime.
class GridWrapper {
    std::unique_ptr<const eckit::geo::Grid> grid_;

public:

    explicit GridWrapper(std::unique_ptr<const eckit::geo::Grid> grid) : grid_(std::move(grid)) {}

    // Identity
    rust::String spec_str() const;
    rust::String catalog_str() const;
    rust::String uid() const;
    rust::String grid_type() const;

    // Shape
    size_t size() const;
    bool is_empty() const;
    rust::Vec<size_t> shape() const;

    /// Points per latitude row ("pl"), empty for grids with no row structure.
    rust::Vec<std::int64_t> pl() const;

    // Geometry
    Bbox bounding_box() const;
    LonLat first_point() const;
    LonLat last_point() const;
    rust::Vec<double> distinct_latitudes() const;
    rust::Vec<double> distinct_longitudes() const;
    void fill_latlons(rust::Slice<double> lat, rust::Slice<double> lon) const;

    // Access underlying for other C++ bridge code
    const eckit::geo::Grid& inner() const { return *grid_; }

    // ============== Factories ==============

    /// Build a grid from a gridSpec — a YAML/JSON mapping, e.g. `{"grid":"O1280"}`.
    static std::unique_ptr<GridWrapper> from_spec(rust::Str spec);
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit_bridge
