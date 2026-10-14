// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// eckit geo Grid bridge — wraps `eckit::geo::Grid`.
#pragma once

#include "ProjectionWrapper.h"
#include "SpecWrapper.h"

#include "eckit/geo/Grid.h"

#include "rust/cxx.h"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>

namespace eckit_bridge {

class AreaWrapper;
class SearchWrapper;
struct RawPoint;

//----------------------------------------------------------------------------------------------------------------------

/// Wraps `eckit::geo::Grid` for Rust FFI.
///
/// Grids are immutable and built through `eckit::geo::GridFactory`, which hands
/// back an owning pointer; the wrapper adopts it for its whole lifetime.
class GridWrapper {
    std::unique_ptr<const eckit::geo::Grid> grid_;
    SpecWrapper spec_;
    ProjectionWrapper projection_;
    mutable std::optional<SpecWrapper> catalog_;

public:

    explicit GridWrapper(std::unique_ptr<const eckit::geo::Grid> grid) :
        grid_(std::move(grid)), spec_(grid_->spec()), projection_(grid_->projection()) {}

    // Identity
    /// The canonical gridSpec, cached by the grid and valid for its lifetime.
    const SpecWrapper& spec() const { return spec_; }

    /// The catalog entry backing this grid, resolved on first use and valid for the grid's lifetime.
    const SpecWrapper& catalog() const;
    rust::String uid() const;
    rust::String grid_type() const;

    /// Name of a named grid, and staggering arrangement of the grids with one (ORCA, FESOM, ICON); empty otherwise.
    rust::String name() const;
    rust::String arrangement() const;

    /// Equality by gridSpec, however the grids are described.
    bool equals(const GridWrapper& other) const;

    // Shape
    size_t size() const;
    bool is_empty() const;
    rust::Vec<size_t> shape() const;

    /// Points per latitude row ("pl"), empty for grids with no row structure.
    rust::Vec<std::int64_t> pl() const;

    /// Number of latitude rows, 0 for grids with no row structure.
    size_t ny() const;

    /// Points per row of a regular grid, 0 otherwise.
    size_t nx() const;

    bool is_regular() const;

    /// Spectral truncation, 0 for grids other than spherical harmonics.
    size_t truncation() const;

    // Order
    /// Scanning order, e.g. "i+j-"; an `OrderError` for grids without one.
    rust::String order() const;

    /// Grid point k is point reorder(to)[k] in order `to`; an `OrderError` for grids without an order.
    rust::Vec<size_t> reorder(rust::Str to) const;

    // Geometry
    std::unique_ptr<AreaWrapper> bounding_box() const;
    RawPoint first_point() const;
    RawPoint last_point() const;
    rust::Vec<RawPoint> to_points() const;
    rust::Vec<rust::String> point_coordinates() const;

    /// Distinct latitudes (longitudes), empty for grids whose rows (columns) share none.
    rust::Vec<double> distinct_latitudes() const;
    rust::Vec<double> distinct_longitudes() const;

    /// Distinct x (y) values of a regular grid, empty otherwise.
    rust::Vec<double> x_values() const;
    rust::Vec<double> y_values() const;

    void fill_latlons(rust::Slice<double> lat, rust::Slice<double> lon) const;
    void fill_xyz(rust::Slice<double> x, rust::Slice<double> y, rust::Slice<double> z) const;

    const ProjectionWrapper& projection() const { return projection_; }
    const FigureWrapper& figure() const { return projection_.figure(); }

    // Derived grids
    std::unique_ptr<GridWrapper> to_unstructured_ll(rust::Str name) const;
    rust::Vec<size_t> crop(const AreaWrapper& area) const;
    std::unique_ptr<GridWrapper> make_grid_cropped(const AreaWrapper& area) const;
    std::unique_ptr<GridWrapper> make_grid_cropped_unstructured_ll(const AreaWrapper& area) const;

    // Search
    std::unique_ptr<SearchWrapper> search() const;
    std::unique_ptr<SearchWrapper> search_with_spec(rust::Str spec) const;

    // Access underlying for other C++ bridge code
    const eckit::geo::Grid& inner() const { return *grid_; }

    // ============== Factories ==============

    /// Build a grid from a gridSpec — a YAML/JSON mapping, e.g. `{"grid":"O1280"}`, or a bare name.
    static std::unique_ptr<GridWrapper> from_spec(rust::Str spec);

    /// Build an unstructured grid from its points, optionally named.
    static std::unique_ptr<GridWrapper> from_latlons(rust::Slice<const double> lat, rust::Slice<const double> lon,
                                                     rust::Str name);
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit_bridge
