//! FFI bindings to `eckit::geo` — grids, identified by gridSpec.

#[cxx::bridge(namespace = "eckit_bridge")]
mod ffi {
    /// Grid bounding box, in degrees.
    #[derive(Debug, Clone, Copy, PartialEq)]
    struct Bbox {
        north: f64,
        west: f64,
        south: f64,
        east: f64,
    }

    /// A geographic point, in degrees.
    #[derive(Debug, Clone, Copy, PartialEq)]
    struct LonLat {
        lon: f64,
        lat: f64,
    }

    unsafe extern "C++" {
        include!("EckitGeoBridge.h");

        // ==================== Grid ====================

        type GridWrapper;

        /// Build a grid from a gridSpec.
        ///
        /// The spec is a YAML/JSON *mapping*, e.g. `{"grid":"O1280"}` or
        /// `{"area":[73,-27,33,45],"grid":[4,4]}`. A bare name such as
        /// `O1280` is not a gridSpec and is rejected.
        #[Self = "GridWrapper"]
        fn from_spec(spec: &str) -> Result<UniquePtr<GridWrapper>>;

        // Identity
        fn spec_str(self: &GridWrapper) -> Result<String>;
        fn catalog_str(self: &GridWrapper) -> Result<String>;
        fn uid(self: &GridWrapper) -> Result<String>;
        fn grid_type(self: &GridWrapper) -> Result<String>;

        // Shape
        fn size(self: &GridWrapper) -> Result<usize>;
        fn is_empty(self: &GridWrapper) -> Result<bool>;
        fn shape(self: &GridWrapper) -> Result<Vec<usize>>;

        /// Points per latitude row. Empty for grids with no row structure.
        fn pl(self: &GridWrapper) -> Result<Vec<i64>>;

        // Geometry
        fn bounding_box(self: &GridWrapper) -> Result<Bbox>;
        fn first_point(self: &GridWrapper) -> Result<LonLat>;
        fn last_point(self: &GridWrapper) -> Result<LonLat>;
        fn distinct_latitudes(self: &GridWrapper) -> Result<Vec<f64>>;
        fn distinct_longitudes(self: &GridWrapper) -> Result<Vec<f64>>;

        /// Fill caller-owned buffers with the grid's latitudes and longitudes.
        /// Both must hold at least `size()` elements.
        fn fill_latlons(self: &GridWrapper, lat: &mut [f64], lon: &mut [f64]) -> Result<()>;

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
        fn total_footprint() -> usize;

        /// Drop every cached entry.
        #[namespace = "eckit::geo::cache"]
        #[Self = "MemoryCache"]
        fn total_purge();
    }
}

pub use ffi::*;
