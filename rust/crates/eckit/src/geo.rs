// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

//! Geospatial grids from `eckit::geo`.
//!
//! A grid is identified by its **gridSpec** — a YAML/JSON mapping such as
//! `{"grid":"O1280"}`. This is a more precise identity than a grid *name*: the
//! spec round-trips through [`Grid::spec`] in canonical form and can carry an
//! area, projection or explicit `pl` that a name cannot express.
//!
//! Around the grid:
//! - [`Area`] and [`BoundingBox`] select points ([`Grid::crop`]);
//! - [`Projection`] and [`Figure`] describe the coordinates the points are in
//!   ([`Grid::projection`], [`Grid::figure`]);
//! - [`Search`] finds the points nearest to a location ([`Grid::search`]).
//!
//! # Caching
//!
//! Unstructured grids ([`Grid::from_latlons`], [`Grid::to_unstructured_ll`],
//! [`Grid::cropped_unstructured_ll`], and so the cropping of projected grids)
//! and polygon areas keep their coordinates in eckit geo's cache, which has to
//! be enabled: resource `eckit-geo-caching` or environment variable
//! `ECKIT_GEO_CACHING`, defaulting to the `geo-caching` feature of `eckit-sys`
//! (off). The cache directory is resource `eckit-geo-cache-path` or
//! `ECKIT_GEO_CACHE_PATH`, by default `~/.cache/eckit/geo`. Without it, those
//! grids fail to build with a `CacheManager cannot create key` error.
//!
//! eckit registers every unstructured grid by uid in a process-wide registry
//! as it is built, re-registering on each build. Building grids of the same
//! points from several threads at once races on it (`Generator has already a
//! builder`), so build a shared unstructured grid once and share the `Grid`.
//!
//! The cached records are LZ4-compressed by default (resource
//! `eckit-codec-compression` or `ECKIT_CODEC_COMPRESSION`), which needs eckit
//! built with the `lz4` feature of `eckit-sys`; otherwise set the resource
//! empty to store them uncompressed, or building those grids fails with `No
//! CompressorBuilder called lz4`.

pub mod area;
pub mod figure;
pub mod projection;
pub mod search;

use std::fmt;
use std::str::FromStr;

use crate::error::Result;

pub use area::{Area, BoundingBox};
pub use figure::Figure;
pub use projection::Projection;
pub use search::{
    MemoryUsage, Neighbour, Search, SearchCacheStatistics, search_cache_clear,
    search_cache_statistics,
};

/// A point, in whichever coordinate system a grid or projection works in.
///
/// Mirrors the `eckit::geo::Point` variant: geographic grids yield
/// [`Point::LonLat`], but projected and Cartesian grids yield planar or 3-D
/// points.
#[derive(Debug, Clone, Copy, PartialEq)]
pub enum Point {
    /// Planar coordinates.
    Xy { x: f64, y: f64 },
    /// Cartesian coordinates.
    Xyz { x: f64, y: f64, z: f64 },
    /// Geographic coordinates, in degrees.
    LonLat { lon: f64, lat: f64 },
    /// Geographic coordinates, in radians.
    LonLatR { lon: f64, lat: f64 },
}

impl From<eckit_sys::geo::RawPoint> for Point {
    fn from(raw: eckit_sys::geo::RawPoint) -> Self {
        use eckit_sys::geo::PointKind;

        let [a, b, c] = raw.coords;
        match raw.kind {
            PointKind::Xy => Self::Xy { x: a, y: b },
            PointKind::Xyz => Self::Xyz { x: a, y: b, z: c },
            PointKind::LonLat => Self::LonLat { lon: a, lat: b },
            PointKind::LonLatR => Self::LonLatR { lon: a, lat: b },
            kind => unreachable!("unknown eckit::geo::Point kind {}", kind.repr),
        }
    }
}

impl From<Point> for eckit_sys::geo::RawPoint {
    fn from(point: Point) -> Self {
        use eckit_sys::geo::PointKind;

        let (kind, coords) = match point {
            Point::Xy { x, y } => (PointKind::Xy, [x, y, 0.0]),
            Point::Xyz { x, y, z } => (PointKind::Xyz, [x, y, z]),
            Point::LonLat { lon, lat } => (PointKind::LonLat, [lon, lat, 0.0]),
            Point::LonLatR { lon, lat } => (PointKind::LonLatR, [lon, lat, 0.0]),
        };
        Self { kind, coords }
    }
}

/// A geospatial grid.
///
/// # Example
///
/// ```ignore
/// let grid = Grid::from_name("O1280")?;
/// assert_eq!(grid.spec().to_json_str()?, r#"{"grid":"O1280"}"#);
/// assert_eq!(grid.spec().get::<String>("grid")?, "O1280");
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
    const fn wrap(inner: eckit_sys::UniquePtr<eckit_sys::geo::GridWrapper>) -> Self {
        Self { inner }
    }

    /// Build a grid from a gridSpec.
    ///
    /// The spec is a YAML/JSON *mapping*, e.g. `{"grid":"O1280"}` or
    /// `{"area":[73,-27,33,45],"grid":[4,4]}`. A bare name such as `O1280`
    /// is read as `{"grid":"O1280"}`; see [`Grid::from_name`] for names
    /// alone, which it checks first.
    pub fn from_spec(spec: &str) -> Result<Self> {
        eckit_sys::geo::GridWrapper::from_spec(spec)
            .map(Self::wrap)
            .map_err(eckit_sys::Error::from)
    }

    /// Build a grid from a name, e.g. `O1280`, `N320`, `F48` or `H4`.
    ///
    /// Convenience over [`Grid::from_spec`] that checks the name is one (no
    /// spaces, braces or quotes) and wraps it into `{"grid":"<name>"}`.
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

    /// Build an unstructured grid from its points, in degrees.
    ///
    /// The grid is identified by the points' uid, so the same points give the
    /// same grid, and `{"uid":"<uid>"}` rebuilds it while the process lives.
    /// Named, it is also rebuilt by [`Grid::from_name`]; the name does not
    /// change the uid.
    ///
    /// Needs eckit geo's cache (see the [module docs](self#caching)).
    ///
    /// ```no_run
    /// # fn main() -> eckit::Result<()> {
    /// # eckit::init();
    /// let grid = eckit::Grid::from_latlons(&[1.0, 2.0, 3.0], &[4.0, 5.0, 6.0], None)?;
    /// assert_eq!(grid.grid_type()?, "unstructured_ll");
    /// assert_eq!(grid.len()?, 3);
    /// assert_eq!(grid.spec().get::<String>("uid")?, grid.uid()?);
    /// # Ok(())
    /// # }
    /// ```
    pub fn from_latlons(lats: &[f64], lons: &[f64], name: Option<&str>) -> Result<Self> {
        eckit_sys::geo::GridWrapper::from_latlons(lats, lons, name.unwrap_or_default())
            .map(Self::wrap)
            .map_err(eckit_sys::Error::from)
    }

    /// The canonical gridSpec identifying this grid.
    ///
    /// Normalised, so it may differ from the spec passed in — `{"grid":"o16"}`
    /// comes back as `{"grid":"O16"}`. The view borrows the spec the grid
    /// caches; [`Spec::to_json_str`] gives the string form.
    #[must_use]
    pub fn spec(&self) -> Spec<'_> {
        Spec {
            inner: self.inner.spec(),
        }
    }

    /// The catalog entry backing this grid.
    ///
    /// Grids shipped as data files (ORCA, FESOM, ICON) are described by an
    /// entry in eckit's shared grid catalog, keyed by uid, which carries what
    /// the gridSpec does not: the data URL, shape, arrangement and so on. For
    /// every other grid the entry is the grid's own canonical spec, so the
    /// catalog is never empty and compares equal to [`Grid::spec`].
    ///
    /// The view borrows the entry the grid caches, like [`Grid::spec`]; the
    /// lookup runs once, on first call.
    ///
    /// ```
    /// # fn main() -> eckit::Result<()> {
    /// # eckit::init();
    /// let grid = eckit::Grid::from_name("O16")?;
    /// let catalog = grid.catalog()?;
    /// assert_eq!(catalog.get::<String>("grid")?, "O16");
    /// assert_eq!(catalog.to_json_str()?, grid.spec().to_json_str()?);
    /// # Ok(())
    /// # }
    /// ```
    pub fn catalog(&self) -> Result<Spec<'_>> {
        self.inner
            .catalog()
            .map(|inner| Spec { inner })
            .map_err(eckit_sys::Error::from)
    }

    /// Unique identifier for this grid.
    pub fn uid(&self) -> Result<String> {
        self.inner.uid().map_err(eckit_sys::Error::from)
    }

    /// Grid type, e.g. `reduced_gg`, `regular_ll`, `healpix`.
    pub fn grid_type(&self) -> Result<String> {
        self.inner.grid_type().map_err(eckit_sys::Error::from)
    }

    /// Name of a named grid, e.g. `O1280`, `H4` or `ORCA2_T`; empty for grids
    /// described by increments or coordinates.
    pub fn name(&self) -> Result<String> {
        self.inner.name().map_err(eckit_sys::Error::from)
    }

    /// Staggering arrangement of the grids identified by one (ORCA, FESOM,
    /// ICON), e.g. `T` or `F`, empty otherwise.
    pub fn arrangement(&self) -> Result<String> {
        self.inner.arrangement().map_err(eckit_sys::Error::from)
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
        self.inner.ny().map_err(eckit_sys::Error::from)
    }

    /// Points per row of a regular grid, or `0` for other grids.
    pub fn nx(&self) -> Result<usize> {
        self.inner.nx().map_err(eckit_sys::Error::from)
    }

    /// Whether this grid has a row structure, i.e. a non-empty [`Grid::pl`].
    pub fn is_structured(&self) -> Result<bool> {
        Ok(self.ny()? > 0)
    }

    /// Whether this grid is regular: `nx` points on each of `ny` rows, with
    /// the same `x` values on every row (`regular_ll`, `regular_gg`,
    /// `regular_xy`).
    pub fn is_regular(&self) -> Result<bool> {
        self.inner.is_regular().map_err(eckit_sys::Error::from)
    }

    /// Spectral truncation of a spherical harmonics grid, `None` for others.
    pub fn truncation(&self) -> Result<Option<usize>> {
        self.inner
            .truncation()
            .map(|t| (t > 0).then_some(t))
            .map_err(eckit_sys::Error::from)
    }

    /// Scanning order of the points, e.g. `i+j-` (west to east, north to
    /// south) or `i+-j+` for alternating rows, as GRIB
    /// `alternativeRowScanning`.
    ///
    /// An [`Error::OrderError`](crate::Error) for grids without an order
    /// (unstructured, spherical harmonics, ORCA).
    pub fn order(&self) -> Result<String> {
        self.inner.order().map_err(eckit_sys::Error::from)
    }

    /// Renumbering of the points into order `to`: point `k` of this grid is
    /// point `reorder(to)[k]` in that order. The empty order is the default,
    /// `i+j-`.
    ///
    /// ```
    /// # fn main() -> eckit::Result<()> {
    /// # eckit::init();
    /// // x and y increasing, so the grid order is i+j+
    /// let grid = eckit::Grid::from_spec(r#"{"x":[0,2,1],"y":[0,1,1]}"#)?;
    /// assert_eq!(grid.order()?, "i+j+");
    /// assert_eq!(grid.reorder("i-j-")?, [5, 4, 3, 2, 1, 0]);
    /// # Ok(())
    /// # }
    /// ```
    pub fn reorder(&self, to: &str) -> Result<Vec<usize>> {
        self.inner.reorder(to).map_err(eckit_sys::Error::from)
    }

    /// Bounding box of the points, in degrees.
    pub fn bounding_box(&self) -> Result<BoundingBox> {
        let area = self
            .inner
            .bounding_box()
            .map(Area::wrap)
            .map_err(eckit_sys::Error::from)?;
        area.into_bounding_box()
    }

    /// First point in iteration order.
    pub fn first_point(&self) -> Result<Point> {
        self.inner
            .first_point()
            .map(Point::from)
            .map_err(eckit_sys::Error::from)
    }

    /// Last point in iteration order.
    pub fn last_point(&self) -> Result<Point> {
        self.inner
            .last_point()
            .map(Point::from)
            .map_err(eckit_sys::Error::from)
    }

    /// Every point in iteration order, whatever the grid's point type.
    ///
    /// The general form of [`Grid::to_latlons`], for Cartesian grids whose
    /// points are [`Point::Xy`]. Each point is 32 bytes, so prefer
    /// [`Grid::fill_latlons`] or [`Grid::fill_xyz`] for large geographic
    /// grids.
    pub fn to_points(&self) -> Result<Vec<Point>> {
        self.inner
            .to_points()
            .map(|points| points.into_iter().map(Point::from).collect())
            .map_err(eckit_sys::Error::from)
    }

    /// Coordinate names of the points, one per coordinate: `["longitude",
    /// "latitude"]` for geographic grids, `["x", "y"]` for Cartesian ones.
    pub fn point_coordinates(&self) -> Result<Vec<String>> {
        self.inner
            .point_coordinates()
            .map_err(eckit_sys::Error::from)
    }

    /// Distinct latitudes, north to south.
    ///
    /// Empty for grids whose rows do not share latitudes (unstructured,
    /// Cartesian `regular_xy`, spherical harmonics).
    pub fn distinct_latitudes(&self) -> Result<Vec<f64>> {
        self.inner
            .distinct_latitudes()
            .map_err(eckit_sys::Error::from)
    }

    /// Distinct longitudes, west to east.
    ///
    /// Empty for grids whose columns do not share longitudes, i.e. all but
    /// `regular_ll` and `regular_gg`.
    pub fn distinct_longitudes(&self) -> Result<Vec<f64>> {
        self.inner
            .distinct_longitudes()
            .map_err(eckit_sys::Error::from)
    }

    /// Distinct `x` values of a regular grid (longitudes in degrees, or
    /// eastings in the projection's units), empty for other grids.
    pub fn x_values(&self) -> Result<Vec<f64>> {
        self.inner.x_values().map_err(eckit_sys::Error::from)
    }

    /// Distinct `y` values of a regular grid (latitudes in degrees, or
    /// northings in the projection's units), empty for other grids.
    pub fn y_values(&self) -> Result<Vec<f64>> {
        self.inner.y_values().map_err(eckit_sys::Error::from)
    }

    /// Write every point's latitude and longitude, in degrees, into
    /// caller-owned buffers.
    ///
    /// Both slices must hold at least [`Grid::len`] elements. Preferred over
    /// [`Grid::to_latlons`] for large grids, since it lets the caller reuse
    /// buffers — O1280 alone is 6.6 million points per array.
    ///
    /// Fails with [`Error::GridError`](crate::Error) if the grid's points are
    /// not geographic ([`Point::LonLat`] or [`Point::LonLatR`]); see
    /// [`Grid::fill_xyz`] for coordinates every grid can give.
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

    /// All latitudes and longitudes, in degrees, as freshly allocated vectors.
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

    /// Write every point's geocentric Cartesian coordinates, on the grid's
    /// [`Figure`], into caller-owned buffers.
    ///
    /// Works for every grid, projected or not. All slices must hold at least
    /// [`Grid::len`] elements. Unlike [`Grid::fill_latlons`], eckit computes
    /// the coordinates into temporaries before they are copied out.
    pub fn fill_xyz(&self, x: &mut [f64], y: &mut [f64], z: &mut [f64]) -> Result<()> {
        let n = self.len()?;
        if x.len() < n || y.len() < n || z.len() < n {
            return Err(eckit_sys::Error::Other(format!(
                "fill_xyz: buffers hold {}, {} and {} points, need {n}",
                x.len(),
                y.len(),
                z.len()
            )));
        }
        self.inner.fill_xyz(x, y, z).map_err(eckit_sys::Error::from)
    }

    /// All geocentric Cartesian coordinates, as freshly allocated vectors
    /// `(x, y, z)`; see [`Grid::fill_xyz`].
    pub fn to_xyz(&self) -> Result<(Vec<f64>, Vec<f64>, Vec<f64>)> {
        let n = self.len()?;
        let mut x = vec![0.0; n];
        let mut y = vec![0.0; n];
        let mut z = vec![0.0; n];
        self.fill_xyz(&mut x, &mut y, &mut z)?;
        Ok((x, y, z))
    }

    /// The projection of the grid points, the default (none) for geographic
    /// grids. Borrowed from the grid, so it lives as long as the grid does.
    #[must_use]
    pub fn projection(&self) -> Projection<'_> {
        Projection::new(self.inner.projection())
    }

    /// The figure (sphere or spheroid) the grid points are on, the
    /// projection's. Borrowed from the grid, so it lives as long as the grid
    /// does.
    #[must_use]
    pub fn figure(&self) -> Figure<'_> {
        Figure::new(self.inner.figure())
    }

    /// This grid as an unstructured grid of the same (lon, lat) points,
    /// optionally named; see [`Grid::from_latlons`], and the cache it needs.
    pub fn to_unstructured_ll(&self, name: Option<&str>) -> Result<Self> {
        self.inner
            .to_unstructured_ll(name.unwrap_or_default())
            .map(Self::wrap)
            .map_err(eckit_sys::Error::from)
    }

    /// Indices of the points in `area`, in the order of the cropped grid
    /// ([`Grid::cropped`]): for a bounding box on a geographic grid, rows
    /// start from the box's west, so the indices are not necessarily sorted.
    ///
    /// Results are shared in a process-wide cache.
    ///
    /// ```
    /// # fn main() -> eckit::Result<()> {
    /// # eckit::init();
    /// let grid = eckit::Grid::from_spec(r#"{"grid":[1,1]}"#)?;
    /// let bbox = eckit::geo::BoundingBox::new(10.0, -10.0, -10.0, 10.0)?;
    /// assert_eq!(grid.crop(&bbox)?.len(), 21 * 21);
    /// # Ok(())
    /// # }
    /// ```
    pub fn crop(&self, area: impl AsRef<Area>) -> Result<Vec<usize>> {
        self.inner
            .crop(area.as_ref().as_sys())
            .map_err(eckit_sys::Error::from)
    }

    /// This grid cropped to `area`, of the same type when supported (a
    /// geographic grid by a bounding box), otherwise unstructured; its points
    /// are those of [`Grid::crop`], in that order.
    ///
    /// An unstructured result needs eckit geo's cache (see the [module
    /// docs](self#caching)); an empty intersection is a
    /// [`Error::GridError`](crate::Error).
    pub fn cropped(&self, area: impl AsRef<Area>) -> Result<Self> {
        self.inner
            .make_grid_cropped(area.as_ref().as_sys())
            .map(Self::wrap)
            .map_err(eckit_sys::Error::from)
    }

    /// The points in `area` as an unstructured grid, whatever this grid's
    /// type; its coordinates are kept in eckit geo's cache (see the [module
    /// docs](self#caching)), so it is exchangeable by uid.
    pub fn cropped_unstructured_ll(&self, area: impl AsRef<Area>) -> Result<Self> {
        self.inner
            .make_grid_cropped_unstructured_ll(area.as_ref().as_sys())
            .map(Self::wrap)
            .map_err(eckit_sys::Error::from)
    }

    /// A nearest-neighbour search on the grid points.
    ///
    /// Searches are shared in a process-wide cache by grid and k-d tree
    /// configuration, so repeated calls are cheap; see
    /// [`search_cache_statistics`].
    ///
    /// ```
    /// # fn main() -> eckit::Result<()> {
    /// # eckit::init();
    /// use eckit::geo::Point;
    ///
    /// let grid = eckit::Grid::from_spec(r#"{"grid":[1,1]}"#)?;
    /// let nearest = grid.search()?.nearest(Point::LonLat { lon: -0.2, lat: 51.6 })?;
    /// let (lat, lon) = grid.to_latlons()?;
    /// assert_eq!((lon[nearest.index], lat[nearest.index]), (0.0, 52.0));
    /// # Ok(())
    /// # }
    /// ```
    pub fn search(&self) -> Result<Search> {
        self.inner
            .search()
            .map(Search::wrap)
            .map_err(eckit_sys::Error::from)
    }

    /// A search configured by a spec, so that [`Search::search`] runs it.
    ///
    /// Keys: `search` (`nn`, `knn`, `radius`, `knn_or_radius`,
    /// `knn_and_radius`), `search-k`, `search-radius` (metres), and for the
    /// k-d tree `search-loader` (`memory`, `mapped-memory`, `file-io`,
    /// `shmem`, ...), `caching`, `search-fast-build`.
    ///
    /// ```
    /// # fn main() -> eckit::Result<()> {
    /// # eckit::init();
    /// use eckit::geo::Point;
    ///
    /// let grid = eckit::Grid::from_spec(r#"{"grid":[1,1]}"#)?;
    /// let search = grid.search_with_spec(r#"{"search":"knn_and_radius","search-k":4,"search-radius":120e3}"#)?;
    /// // 4 nearest, but only within 120 km: (0, 0) and (1, 0)
    /// assert_eq!(search.search(Point::LonLat { lon: 0.5, lat: 0.0 })?.len(), 2);
    /// # Ok(())
    /// # }
    /// ```
    pub fn search_with_spec(&self, spec: &str) -> Result<Search> {
        self.inner
            .search_with_spec(spec)
            .map(Search::wrap)
            .map_err(eckit_sys::Error::from)
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

/// Equality by canonical gridSpec, however the grids were described. Grids
/// whose spec cannot be read compare unequal.
impl PartialEq for Grid {
    fn eq(&self, other: &Self) -> bool {
        self.inner.equals(&other.inner).unwrap_or(false)
    }
}

impl fmt::Debug for Grid {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        f.debug_tuple("Grid").field(&self.spec()).finish()
    }
}

/// A spec: the key/value description of a [`Grid`], [`Area`], [`Projection`]
/// or [`Figure`], borrowed from the object that owns it.
///
/// Values are read by type through [`Spec::get`], the canonical JSON through
/// [`Spec::to_json_str`]. A missing key, or one holding another type, is an
/// [`Error::SpecError`](crate::Error).
pub struct Spec<'g> {
    inner: &'g eckit_sys::geo::SpecWrapper,
}

impl Spec<'_> {
    /// Whether `key` is present.
    #[must_use]
    pub fn has(&self, key: &str) -> bool {
        self.inner.has(key)
    }

    /// Read `key` as `T`.
    pub fn get<T: SpecGet>(&self, key: &str) -> Result<T> {
        T::get_from(self.inner, key)
    }

    /// Canonical JSON form, e.g. `{"grid":"O1280"}`.
    pub fn to_json_str(&self) -> Result<String> {
        self.inner.json().map_err(eckit_sys::Error::from)
    }
}

impl fmt::Debug for Spec<'_> {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self.to_json_str() {
            Ok(json) => f.write_str(&json),
            Err(err) => write!(f, "<spec unavailable: {err}>"),
        }
    }
}

/// Types a [`Spec`] value can be read as.
pub trait SpecGet: Sized {
    /// Read `key` from `spec` as `Self`.
    fn get_from(spec: &eckit_sys::geo::SpecWrapper, key: &str) -> Result<Self>;
}

impl SpecGet for String {
    fn get_from(spec: &eckit_sys::geo::SpecWrapper, key: &str) -> Result<Self> {
        spec.get_string(key).map_err(eckit_sys::Error::from)
    }
}

impl SpecGet for bool {
    fn get_from(spec: &eckit_sys::geo::SpecWrapper, key: &str) -> Result<Self> {
        spec.get_bool(key).map_err(eckit_sys::Error::from)
    }
}

impl SpecGet for i64 {
    fn get_from(spec: &eckit_sys::geo::SpecWrapper, key: &str) -> Result<Self> {
        spec.get_long(key).map_err(eckit_sys::Error::from)
    }
}

impl SpecGet for usize {
    fn get_from(spec: &eckit_sys::geo::SpecWrapper, key: &str) -> Result<Self> {
        spec.get_unsigned(key).map_err(eckit_sys::Error::from)
    }
}

impl SpecGet for f64 {
    fn get_from(spec: &eckit_sys::geo::SpecWrapper, key: &str) -> Result<Self> {
        spec.get_double(key).map_err(eckit_sys::Error::from)
    }
}

impl SpecGet for Vec<i64> {
    fn get_from(spec: &eckit_sys::geo::SpecWrapper, key: &str) -> Result<Self> {
        spec.get_long_vector(key).map_err(eckit_sys::Error::from)
    }
}

impl SpecGet for Vec<usize> {
    fn get_from(spec: &eckit_sys::geo::SpecWrapper, key: &str) -> Result<Self> {
        spec.get_unsigned_vector(key)
            .map_err(eckit_sys::Error::from)
    }
}

impl SpecGet for Vec<f64> {
    fn get_from(spec: &eckit_sys::geo::SpecWrapper, key: &str) -> Result<Self> {
        spec.get_double_vector(key).map_err(eckit_sys::Error::from)
    }
}

impl SpecGet for Vec<String> {
    fn get_from(spec: &eckit_sys::geo::SpecWrapper, key: &str) -> Result<Self> {
        spec.get_string_vector(key).map_err(eckit_sys::Error::from)
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
///
/// Live grids keep working, they simply recompute on next use. However eckit's
/// own iterators hold references into these caches while they run, so do not
/// purge while another thread is inside [`Grid::fill_latlons`],
/// [`Grid::to_latlons`] or any other grid call.
pub fn cache_purge() {
    eckit_sys::geo::MemoryCache::total_purge();
}
