//! Geospatial grids from `eckit::geo`.
//!
//! A grid is identified by its **gridSpec** — a YAML/JSON mapping such as
//! `{"grid":"O1280"}`. This is a more precise identity than a grid *name*: the
//! spec round-trips through [`Grid::spec`] in canonical form and can carry an
//! area, projection or explicit `pl` that a name cannot express.

use std::str::FromStr;

use crate::error::Result;

pub use eckit_sys::geo::{Bbox, LonLat};

/// A geospatial grid.
///
/// # Example
///
/// ```ignore
/// let grid = Grid::from_name("O1280")?;
/// assert_eq!(grid.spec()?, r#"{"grid":"O1280"}"#);
///
/// let pl = grid.pl()?;                       // points per latitude row
/// assert_eq!(pl.iter().sum::<i64>() as usize, grid.len()?);
/// ```
pub struct Grid {
    inner: eckit_sys::UniquePtr<eckit_sys::geo::GridWrapper>,
}

// SAFETY: `GridWrapper` owns its `eckit::geo::Grid` outright, so moving one
// between threads transfers sole ownership.
//
// `Sync` is deliberately NOT implemented: `eckit::geo::Grid` fills `mutable`
// caches (its bounding box, projection, catalog, spec and uid) from `const`
// methods without synchronisation, so sharing a `&Grid` across threads races.
// Share via `Arc<Mutex<Grid>>`, or rebuild per thread from the gridSpec.
#[allow(clippy::non_send_fields_in_send_ty)]
unsafe impl Send for Grid {}

impl Grid {
    /// Build a grid from a gridSpec.
    ///
    /// The spec is a YAML/JSON *mapping*, e.g. `{"grid":"O1280"}` or
    /// `{"area":[73,-27,33,45],"grid":[4,4]}`. A bare name such as `"O1280"`
    /// is **not** a gridSpec — it parses as a scalar and is rejected with
    /// [`Error::SpecError`](crate::Error). Use [`Grid::from_name`] for names.
    pub fn from_spec(spec: &str) -> Result<Self> {
        let inner = eckit_sys::geo::GridWrapper::from_spec(spec).map_err(eckit_sys::Error::from)?;
        Ok(Self { inner })
    }

    /// Build a grid from a name, e.g. `O1280`, `N320`, `F48` or `H4`.
    ///
    /// Convenience over [`Grid::from_spec`] that wraps the name into
    /// `{"grid":"<name>"}`.
    pub fn from_name(name: &str) -> Result<Self> {
        if name.is_empty()
            || !name
                .chars()
                .all(|c| c.is_ascii_alphanumeric() || matches!(c, '-' | '_' | '.'))
        {
            return Err(eckit_sys::Error::Other(format!(
                "not a valid grid name: {name:?}"
            )));
        }
        Self::from_spec(&format!(r#"{{"grid":"{name}"}}"#))
    }

    /// The canonical gridSpec identifying this grid.
    ///
    /// Normalised, so it may differ textually from the spec passed in —
    /// `{"grid":"o16"}` comes back as `{"grid":"O16"}`.
    pub fn spec(&self) -> Result<String> {
        self.inner.spec_str().map_err(eckit_sys::Error::from)
    }

    /// The catalog entry backing this grid, as a spec string.
    pub fn catalog(&self) -> Result<String> {
        self.inner.catalog_str().map_err(eckit_sys::Error::from)
    }

    /// Unique identifier for this grid.
    pub fn uid(&self) -> Result<String> {
        self.inner.uid().map_err(eckit_sys::Error::from)
    }

    /// Grid type, e.g. `reduced_gg`, `regular_ll`, `healpix`.
    pub fn grid_type(&self) -> Result<String> {
        self.inner.grid_type().map_err(eckit_sys::Error::from)
    }

    /// Total number of points.
    pub fn len(&self) -> Result<usize> {
        self.inner.size().map_err(eckit_sys::Error::from)
    }

    /// Whether the grid has no points.
    pub fn is_empty(&self) -> Result<bool> {
        self.inner.is_empty().map_err(eckit_sys::Error::from)
    }

    /// Dimensions: `[ny, nx]` for regular grids, `[size]` for reduced ones.
    pub fn shape(&self) -> Result<Vec<usize>> {
        self.inner.shape().map_err(eckit_sys::Error::from)
    }

    /// Points per latitude row (the "pl" array), north to south.
    ///
    /// Empty for grids with no row structure (unstructured, spherical
    /// harmonics, ORCA). For structured grids the entries sum to [`Grid::len`].
    ///
    /// For reduced Gaussian and reduced lon/lat grids this is the grid's
    /// defining `pl`; for `HEALPix` and regular grids it is derived from the
    /// per-row point counts.
    pub fn pl(&self) -> Result<Vec<i64>> {
        self.inner.pl().map_err(eckit_sys::Error::from)
    }

    /// Number of latitude rows, or `0` for grids with no row structure.
    pub fn ny(&self) -> Result<usize> {
        Ok(self.pl()?.len())
    }

    /// Whether this grid has a row structure, i.e. a non-empty [`Grid::pl`].
    pub fn is_structured(&self) -> Result<bool> {
        Ok(!self.pl()?.is_empty())
    }

    /// Bounding box in degrees.
    pub fn bounding_box(&self) -> Result<Bbox> {
        self.inner.bounding_box().map_err(eckit_sys::Error::from)
    }

    /// First point in iteration order.
    pub fn first_point(&self) -> Result<LonLat> {
        self.inner.first_point().map_err(eckit_sys::Error::from)
    }

    /// Last point in iteration order.
    pub fn last_point(&self) -> Result<LonLat> {
        self.inner.last_point().map_err(eckit_sys::Error::from)
    }

    /// Distinct latitudes, north to south.
    pub fn distinct_latitudes(&self) -> Result<Vec<f64>> {
        self.inner
            .distinct_latitudes()
            .map_err(eckit_sys::Error::from)
    }

    /// Distinct longitudes, west to east.
    pub fn distinct_longitudes(&self) -> Result<Vec<f64>> {
        self.inner
            .distinct_longitudes()
            .map_err(eckit_sys::Error::from)
    }

    /// Write every point's latitude and longitude into caller-owned buffers.
    ///
    /// Both slices must hold at least [`Grid::len`] elements. Preferred over
    /// [`Grid::to_latlons`] for large grids, since it lets the caller reuse
    /// buffers — O1280 alone is 6.6 million points per array.
    pub fn fill_latlons(&self, lat: &mut [f64], lon: &mut [f64]) -> Result<()> {
        let n = self.len()?;
        if lat.len() < n || lon.len() < n {
            return Err(eckit_sys::Error::Other(format!(
                "fill_latlons: buffers hold {} and {} points, need {n}",
                lat.len(),
                lon.len()
            )));
        }
        self.inner
            .fill_latlons(lat, lon)
            .map_err(eckit_sys::Error::from)
    }

    /// All latitudes and longitudes, as freshly allocated vectors.
    ///
    /// Allocates two `Vec<f64>` of [`Grid::len`] elements; prefer
    /// [`Grid::fill_latlons`] when the buffers can be reused.
    pub fn to_latlons(&self) -> Result<(Vec<f64>, Vec<f64>)> {
        let n = self.len()?;
        let mut lat = vec![0.0; n];
        let mut lon = vec![0.0; n];
        self.fill_latlons(&mut lat, &mut lon)?;
        Ok((lat, lon))
    }

    /// Access the underlying eckit-sys wrapper (for FFI interop with other -sys crates).
    #[must_use]
    pub fn as_sys(&self) -> &eckit_sys::geo::GridWrapper {
        &self.inner
    }
}

impl FromStr for Grid {
    type Err = eckit_sys::Error;

    /// Parse a gridSpec. Equivalent to [`Grid::from_spec`].
    fn from_str(spec: &str) -> std::result::Result<Self, Self::Err> {
        Self::from_spec(spec)
    }
}

/// Total size of eckit geo's process-global memory caches, in bytes.
///
/// These caches grow as distinct grids are built (Gaussian latitudes, `HEALPix`
/// tables, downloaded grid data) and are never evicted, so a long-running
/// process should watch this and call [`cache_purge`].
#[must_use]
pub fn cache_footprint() -> usize {
    eckit_sys::geo::MemoryCache::total_footprint()
}

/// Drop every entry from eckit geo's process-global memory caches.
pub fn cache_purge() {
    eckit_sys::geo::MemoryCache::total_purge();
}
