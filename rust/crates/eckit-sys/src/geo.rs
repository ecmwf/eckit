// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

//! FFI bindings to `eckit::geo`: grids, identified by gridSpec, and their
//! areas, figures, projections and point searches.

#[cxx::bridge(namespace = "eckit_bridge")]
mod ffi {
    /// Bounding box extent, in degrees.
    #[derive(Debug, Clone, Copy, PartialEq)]
    struct Bbox {
        north: f64,
        west: f64,
        south: f64,
        east: f64,
    }

    /// Which alternative of the `eckit::geo::Point` variant a [`RawPoint`] holds.
    #[derive(Debug, Clone, Copy, PartialEq, Eq)]
    enum PointKind {
        /// `PointXY`: `coords` is `[x, y, _]`.
        Xy,
        /// `PointXYZ`: `coords` is `[x, y, z]`.
        Xyz,
        /// `PointLonLat`: `coords` is `[lon, lat, _]`, in degrees.
        LonLat,
        /// `PointLonLatR`: `coords` is `[lon, lat, _]`, in radians.
        LonLatR,
    }

    /// An `eckit::geo::Point`, flattened for FFI. Unused trailing coordinates are `0`.
    #[derive(Debug, Clone, Copy, PartialEq)]
    struct RawPoint {
        kind: PointKind,
        coords: [f64; 3],
    }

    /// A grid point found by a search: its index in the grid, the chord
    /// distance to the query (metres, on the Earth) and its geocentric
    /// position.
    #[derive(Debug, Clone, Copy, PartialEq)]
    struct Neighbour {
        index: usize,
        distance: f64,
        point: RawPoint,
    }

    /// Memory used, in bytes: private to the process, and shared between
    /// processes (shared memory, page cache).
    #[derive(Debug, Clone, Copy, PartialEq, Eq, Default)]
    struct MemoryUsage {
        memory: usize,
        shared: usize,
    }

    /// Statistics of the process-wide cache of searches.
    #[derive(Debug, Clone, Copy, PartialEq, Eq, Default)]
    struct SearchCacheStatistics {
        hits: usize,
        misses: usize,
        evictions: usize,
        /// Searches cached.
        size: usize,
        usage: MemoryUsage,
    }

    unsafe extern "C++" {
        include!("EckitGeoBridge.h");
        include!("eckit_exceptions.h");

        // ==================== Grid ====================

        type GridWrapper;

        /// Build a grid from a gridSpec.
        ///
        /// The spec is a YAML/JSON *mapping*, e.g. `{"grid":"O1280"}` or
        /// `{"area":[73,-27,33,45],"grid":[4,4]}`; a bare name such as
        /// `O1280` is read as `{"grid":"O1280"}`.
        #[Self = "GridWrapper"]
        fn from_spec(spec: &str) -> Result<UniquePtr<GridWrapper>>;

        /// Build an unstructured grid from its points, optionally named
        /// (`name` empty for none). The slices must be of the same length.
        #[Self = "GridWrapper"]
        fn from_latlons(lat: &[f64], lon: &[f64], name: &str) -> Result<UniquePtr<GridWrapper>>;

        // Identity

        /// The canonical gridSpec, cached by the grid and valid for its lifetime.
        fn spec(self: &GridWrapper) -> &SpecWrapper;
        /// The catalog entry backing the grid, resolved on first call and valid for its lifetime.
        fn catalog(self: &GridWrapper) -> Result<&SpecWrapper>;
        fn uid(self: &GridWrapper) -> Result<String>;
        fn grid_type(self: &GridWrapper) -> Result<String>;
        /// Name of a named grid (`O1280`, `ORCA2_T`, ...) and the staggering
        /// arrangement of the grids with one (ORCA, FESOM, ICON); empty otherwise.
        fn name(self: &GridWrapper) -> Result<String>;
        fn arrangement(self: &GridWrapper) -> Result<String>;
        /// Equality by gridSpec.
        fn equals(self: &GridWrapper, other: &GridWrapper) -> Result<bool>;

        // Shape
        fn size(self: &GridWrapper) -> Result<usize>;
        fn is_empty(self: &GridWrapper) -> Result<bool>;
        fn shape(self: &GridWrapper) -> Result<Vec<usize>>;

        /// Points per latitude row. Empty for grids with no row structure.
        fn pl(self: &GridWrapper) -> Result<Vec<i64>>;

        /// Number of latitude rows, `0` for grids with no row structure.
        fn ny(self: &GridWrapper) -> Result<usize>;

        /// Points per row of a grid of shape `(ny, nx)`, `0` otherwise.
        fn nx(self: &GridWrapper) -> Result<usize>;

        fn is_regular(self: &GridWrapper) -> Result<bool>;

        /// Spectral truncation, `0` for grids other than spherical harmonics.
        fn truncation(self: &GridWrapper) -> Result<usize>;

        // Order

        /// Scanning order, e.g. `i+j-`; an `OrderError` for grids without one.
        fn order(self: &GridWrapper) -> Result<String>;

        /// Grid point `k` is point `reorder(to)[k]` in order `to`; an
        /// `OrderError` for grids without an order.
        fn reorder(self: &GridWrapper, to: &str) -> Result<Vec<usize>>;

        // Geometry
        fn bounding_box(self: &GridWrapper) -> Result<UniquePtr<AreaWrapper>>;
        fn first_point(self: &GridWrapper) -> Result<RawPoint>;
        fn last_point(self: &GridWrapper) -> Result<RawPoint>;
        /// Fill a caller-owned buffer of at least `size()` elements with every
        /// point, in iteration order, whatever the grid's point type.
        fn fill_points(self: &GridWrapper, points: &mut [RawPoint]) -> Result<()>;
        /// Coordinate names of the grid points, e.g. `["longitude", "latitude"]` or `["x", "y"]`.
        fn point_coordinates(self: &GridWrapper) -> Result<Vec<String>>;

        /// Distinct latitudes (longitudes), empty for grids whose rows
        /// (columns) share none.
        fn distinct_latitudes(self: &GridWrapper) -> Result<Vec<f64>>;
        fn distinct_longitudes(self: &GridWrapper) -> Result<Vec<f64>>;

        /// Distinct x (y) values of a regular grid, empty otherwise.
        fn x_values(self: &GridWrapper) -> Result<Vec<f64>>;
        fn y_values(self: &GridWrapper) -> Result<Vec<f64>>;

        /// Fill caller-owned buffers with the grid's latitudes and longitudes,
        /// in degrees. Both must hold at least `size()` elements. A
        /// `GridError` if the grid's points are not lon/lat.
        fn fill_latlons(self: &GridWrapper, lat: &mut [f64], lon: &mut [f64]) -> Result<()>;

        /// Fill caller-owned buffers with the grid's geocentric coordinates,
        /// on its figure. All must hold at least `size()` elements.
        fn fill_xyz(self: &GridWrapper, x: &mut [f64], y: &mut [f64], z: &mut [f64]) -> Result<()>;

        /// The grid's projection, valid for its lifetime.
        fn projection(self: &GridWrapper) -> &ProjectionWrapper;

        // Derived grids

        /// The grid as an unstructured grid of the same (lon, lat) points,
        /// optionally named (`name` empty for none).
        fn to_unstructured_ll(self: &GridWrapper, name: &str) -> Result<UniquePtr<GridWrapper>>;

        /// Indices of the grid points in the area, as the cropped grid.
        fn crop(self: &GridWrapper, area: &AreaWrapper) -> Result<Vec<usize>>;

        /// The grid cropped to the area, of the same type when supported,
        /// otherwise unstructured.
        fn make_grid_cropped(
            self: &GridWrapper,
            area: &AreaWrapper,
        ) -> Result<UniquePtr<GridWrapper>>;

        /// The grid points in the area, as an unstructured grid.
        fn make_grid_cropped_unstructured_ll(
            self: &GridWrapper,
            area: &AreaWrapper,
        ) -> Result<UniquePtr<GridWrapper>>;

        // Search

        /// A nearest-neighbour search on the grid points, shared by the
        /// process-wide cache.
        fn search(self: &GridWrapper) -> Result<UniquePtr<SearchWrapper>>;

        /// A search configured by a spec: keys `search` (`nn`, `knn`,
        /// `radius`, `knn_or_radius`, `knn_and_radius`), `search-k`,
        /// `search-radius`, `search-loader`, `caching`, `search-fast-build`.
        fn search_with_spec(self: &GridWrapper, spec: &str) -> Result<UniquePtr<SearchWrapper>>;

        // ==================== Spec ====================

        /// Read-only view of a spec, borrowed from the object that owns it.
        type SpecWrapper;

        fn has(self: &SpecWrapper, key: &str) -> bool;

        // Typed access; a missing key, or one holding another type, is a `SpecError`.
        fn get_string(self: &SpecWrapper, key: &str) -> Result<String>;
        fn get_bool(self: &SpecWrapper, key: &str) -> Result<bool>;
        fn get_long(self: &SpecWrapper, key: &str) -> Result<i64>;
        fn get_unsigned(self: &SpecWrapper, key: &str) -> Result<usize>;
        fn get_double(self: &SpecWrapper, key: &str) -> Result<f64>;
        fn get_long_vector(self: &SpecWrapper, key: &str) -> Result<Vec<i64>>;
        fn get_unsigned_vector(self: &SpecWrapper, key: &str) -> Result<Vec<usize>>;
        fn get_double_vector(self: &SpecWrapper, key: &str) -> Result<Vec<f64>>;
        fn get_string_vector(self: &SpecWrapper, key: &str) -> Result<Vec<String>>;

        /// Canonical JSON form.
        fn json(self: &SpecWrapper) -> Result<String>;

        // ==================== Area ====================

        /// An `eckit::geo::Area`: a bounding box, polygon or shapefile feature.
        type AreaWrapper;

        /// Build an area from a spec, a YAML/JSON mapping, e.g.
        /// `{"area":[90,0,-90,360]}` or `{"type":"polygon",...}`.
        #[Self = "AreaWrapper"]
        fn from_spec(spec: &str) -> Result<UniquePtr<AreaWrapper>>;

        /// Build a bounding box from its extent.
        #[Self = "AreaWrapper"]
        fn from_bbox(bbox: Bbox) -> Result<UniquePtr<AreaWrapper>>;

        fn area_type(self: &AreaWrapper) -> Result<String>;
        fn uid(self: &AreaWrapper) -> Result<String>;
        /// The canonical spec, cached by the area and valid for its lifetime.
        fn spec(self: &AreaWrapper) -> &SpecWrapper;
        fn is_default(self: &AreaWrapper) -> Result<bool>;
        /// Whether the (lon, lat) point is in the area.
        fn contains(self: &AreaWrapper, lon: f64, lat: f64) -> Result<bool>;
        /// Whether the area intersects a bounding box; an `AreaError` if `bbox` isn't one.
        fn intersects(self: &AreaWrapper, bbox: &AreaWrapper) -> Result<bool>;
        /// Surface area, assuming Earth [m^2].
        fn surface_area(self: &AreaWrapper) -> Result<f64>;
        /// Equality by uid.
        fn equals(self: &AreaWrapper, other: &AreaWrapper) -> Result<bool>;

        // Bounding boxes: an `AreaError` when the area is not one.
        fn is_bounding_box(self: &AreaWrapper) -> bool;
        fn bbox(self: &AreaWrapper) -> Result<Bbox>;
        fn bbox_global(self: &AreaWrapper) -> Result<bool>;
        fn bbox_periodic(self: &AreaWrapper) -> Result<bool>;
        fn bbox_empty(self: &AreaWrapper) -> Result<bool>;
        fn bbox_contains(self: &AreaWrapper, other: &AreaWrapper) -> Result<bool>;

        // ==================== Figure ====================

        /// View of an `eckit::geo::Figure` (sphere or spheroid), borrowed
        /// from the projection holding it.
        type FigureWrapper;

        /// Radius; a `FigureError` for a non-spherical figure.
        #[cxx_name = "R"]
        fn radius(self: &FigureWrapper) -> Result<f64>;
        /// Semi-major and semi-minor axes.
        fn a(self: &FigureWrapper) -> Result<f64>;
        fn b(self: &FigureWrapper) -> Result<f64>;
        /// Surface area [L^2].
        fn area(self: &FigureWrapper) -> Result<f64>;
        /// Surface area between the parallels and meridians of a bounding box [L^2].
        fn area_within(self: &FigureWrapper, bbox: &AreaWrapper) -> Result<f64>;
        /// The figure's spec, built with the view and valid for its lifetime.
        fn spec(self: &FigureWrapper) -> &SpecWrapper;
        fn proj_str(self: &FigureWrapper) -> Result<String>;
        fn spherical(self: &FigureWrapper) -> Result<bool>;
        fn eccentricity(self: &FigureWrapper) -> Result<f64>;
        fn flattening(self: &FigureWrapper) -> Result<f64>;
        fn is_default(self: &FigureWrapper) -> Result<bool>;
        /// Equality by size.
        fn equals(self: &FigureWrapper, other: &FigureWrapper) -> Result<bool>;

        // ==================== Projection ====================

        /// View of an `eckit::geo::Projection`, borrowed from the grid holding it.
        type ProjectionWrapper;

        fn projection_type(self: &ProjectionWrapper) -> Result<String>;
        /// The canonical spec, cached by the projection and valid for its lifetime.
        fn spec(self: &ProjectionWrapper) -> &SpecWrapper;
        fn proj_str(self: &ProjectionWrapper) -> Result<String>;
        fn is_default(self: &ProjectionWrapper) -> Result<bool>;
        /// Whether `fwd` and `inv` are swapped (see `projection::Reverse`).
        fn reverse(self: &ProjectionWrapper) -> Result<bool>;
        /// Source and target point coordinate names, e.g. `["longitude", "latitude"]` to `["x", "y"]`.
        fn source_point_coordinates(self: &ProjectionWrapper) -> Result<Vec<String>>;
        fn target_point_coordinates(self: &ProjectionWrapper) -> Result<Vec<String>>;
        /// Number of coordinates of the source (target) points, 2 or 3.
        fn source_dims(self: &ProjectionWrapper) -> Result<usize>;
        fn target_dims(self: &ProjectionWrapper) -> Result<usize>;
        /// The projection's figure, valid for its lifetime.
        fn figure(self: &ProjectionWrapper) -> &FigureWrapper;
        /// Project a point from the source to the target point type (`fwd`) or the reverse (`inv`).
        fn fwd(self: &ProjectionWrapper, point: RawPoint) -> Result<RawPoint>;
        fn inv(self: &ProjectionWrapper, point: RawPoint) -> Result<RawPoint>;
        /// Project points given one slice per source coordinate (`v3` empty
        /// for points of 2 coordinates), writing one slice per target
        /// coordinate (`out3` unused for targets of 2 coordinates). Points
        /// that fail to project are NaN.
        fn fwd_vectors(
            self: &ProjectionWrapper,
            v1: &[f64],
            v2: &[f64],
            v3: &[f64],
            out1: &mut [f64],
            out2: &mut [f64],
            out3: &mut [f64],
        ) -> Result<()>;
        fn inv_vectors(
            self: &ProjectionWrapper,
            v1: &[f64],
            v2: &[f64],
            v3: &[f64],
            out1: &mut [f64],
            out2: &mut [f64],
            out3: &mut [f64],
        ) -> Result<()>;
        /// Equality by spec.
        fn equals(self: &ProjectionWrapper, other: &ProjectionWrapper) -> Result<bool>;

        // ==================== Search ====================

        /// A search on the points of a grid, following k-d tree algorithms.
        /// Query points are (lon, lat) in degrees or geocentric (x, y, z);
        /// results are by increasing distance, then index.
        type SearchWrapper;

        /// As configured by the spec (nearest neighbour by default).
        fn search(self: &SearchWrapper, point: RawPoint) -> Result<Vec<Neighbour>>;
        fn search_nn(self: &SearchWrapper, point: RawPoint) -> Result<Neighbour>;
        fn search_knn(self: &SearchWrapper, point: RawPoint, k: usize) -> Result<Vec<Neighbour>>;
        fn search_radius(
            self: &SearchWrapper,
            point: RawPoint,
            radius: f64,
        ) -> Result<Vec<Neighbour>>;
        /// Union of the k nearest and those within the radius.
        fn search_knn_or_radius(
            self: &SearchWrapper,
            point: RawPoint,
            k: usize,
            radius: f64,
        ) -> Result<Vec<Neighbour>>;
        /// Intersection of the k nearest and those within the radius.
        fn search_knn_and_radius(
            self: &SearchWrapper,
            point: RawPoint,
            k: usize,
            radius: f64,
        ) -> Result<Vec<Neighbour>>;
        /// Number of points searched.
        fn size(self: &SearchWrapper) -> Result<usize>;
        fn footprint(self: &SearchWrapper) -> Result<MemoryUsage>;
        fn to_string(self: &SearchWrapper) -> Result<String>;

        /// Statistics of the process-wide cache of searches.
        #[Self = "SearchWrapper"]
        fn cache_statistics() -> Result<SearchCacheStatistics>;

        /// Evict every cached search; those in use stay valid.
        #[Self = "SearchWrapper"]
        fn cache_clear() -> Result<()>;

        // ==================== Caches ====================

        include!("eckit/geo/cache/MemoryCache.h");

        /// eckit geo's process-global memory caches. They grow as distinct
        /// grids are built and are never evicted, so a long-running process
        /// has to reclaim them explicitly.
        #[namespace = "eckit::geo::cache"]
        type MemoryCache;

        /// Total size of the caches, in bytes.
        #[namespace = "eckit::geo::cache"]
        #[Self = "MemoryCache"]
        #[must_use]
        fn total_footprint() -> usize;

        /// Drop every cached entry.
        #[namespace = "eckit::geo::cache"]
        #[Self = "MemoryCache"]
        fn total_purge();
    }
}

pub use ffi::*;
