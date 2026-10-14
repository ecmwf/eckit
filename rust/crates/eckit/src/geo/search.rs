// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

//! Nearest-neighbour searches on grid points.

use std::fmt;

use crate::error::Result;
use crate::geo::Point;

pub use eckit_sys::geo::{MemoryUsage, SearchCacheStatistics};

/// A grid point found by a [`Search`].
#[derive(Debug, Clone, Copy, PartialEq)]
pub struct Neighbour {
    /// Index of the point in the grid.
    pub index: usize,
    /// Chord distance to the query, in metres on the Earth.
    pub distance: f64,
    /// Geocentric position of the point, a [`Point::Xyz`].
    pub point: Point,
}

impl From<eckit_sys::geo::Neighbour> for Neighbour {
    fn from(raw: eckit_sys::geo::Neighbour) -> Self {
        Self {
            index: raw.index,
            distance: raw.distance,
            point: raw.point.into(),
        }
    }
}

/// A search on the points of a grid, following k-d tree algorithms.
///
/// Built by [`Grid::search`](crate::Grid::search) or
/// [`Grid::search_with_spec`](crate::Grid::search_with_spec). Queries are
/// [`Point::LonLat`] in degrees, or [`Point::Xyz`] geocentric on the grid's
/// figure; distances are chord lengths in metres on the Earth, and results
/// come by increasing distance, then index.
///
/// Searches are shared in a process-wide cache by grid and k-d tree
/// configuration, within a capacity (resource
/// `eckit-geo-cache-search-capacity`); a `Search` keeps its entry from being
/// evicted while it lives.
pub struct Search {
    inner: eckit_sys::UniquePtr<eckit_sys::geo::SearchWrapper>,
}

// SAFETY: `SearchWrapper` holds a shared handle on an immutable
// `eckit::geo::Search`, which can be used from any thread. Not `Sync`: the
// handle itself is a non-atomic view and the k-d tree statistics are
// updated per query.
#[allow(clippy::non_send_fields_in_send_ty)]
unsafe impl Send for Search {}

impl Search {
    pub(crate) const fn wrap(inner: eckit_sys::UniquePtr<eckit_sys::geo::SearchWrapper>) -> Self {
        Self { inner }
    }

    /// Search with the configured behaviour: nearest neighbour unless built
    /// from a spec saying otherwise.
    pub fn search(&self, point: Point) -> Result<Vec<Neighbour>> {
        self.inner
            .search(point.into())
            .map(neighbours)
            .map_err(eckit_sys::Error::from)
    }

    /// The nearest point.
    pub fn nearest(&self, point: Point) -> Result<Neighbour> {
        self.inner
            .search_nn(point.into())
            .map(Neighbour::from)
            .map_err(eckit_sys::Error::from)
    }

    /// The `k` nearest points.
    pub fn nearest_k(&self, point: Point, k: usize) -> Result<Vec<Neighbour>> {
        self.inner
            .search_knn(point.into(), k)
            .map(neighbours)
            .map_err(eckit_sys::Error::from)
    }

    /// The points within `radius` metres.
    pub fn within_radius(&self, point: Point, radius: f64) -> Result<Vec<Neighbour>> {
        self.inner
            .search_radius(point.into(), radius)
            .map(neighbours)
            .map_err(eckit_sys::Error::from)
    }

    /// The union of the `k` nearest points and those within `radius` metres.
    pub fn nearest_k_or_within_radius(
        &self,
        point: Point,
        k: usize,
        radius: f64,
    ) -> Result<Vec<Neighbour>> {
        self.inner
            .search_knn_or_radius(point.into(), k, radius)
            .map(neighbours)
            .map_err(eckit_sys::Error::from)
    }

    /// The `k` nearest points, only those within `radius` metres.
    pub fn nearest_k_and_within_radius(
        &self,
        point: Point,
        k: usize,
        radius: f64,
    ) -> Result<Vec<Neighbour>> {
        self.inner
            .search_knn_and_radius(point.into(), k, radius)
            .map(neighbours)
            .map_err(eckit_sys::Error::from)
    }

    /// Number of points searched, the grid's.
    pub fn len(&self) -> Result<usize> {
        self.inner.size().map_err(eckit_sys::Error::from)
    }

    /// Whether there are no points to search.
    pub fn is_empty(&self) -> Result<bool> {
        Ok(self.len()? == 0)
    }

    /// Memory used by the k-d tree, in the process and shared.
    pub fn footprint(&self) -> Result<MemoryUsage> {
        self.inner.footprint().map_err(eckit_sys::Error::from)
    }
}

fn neighbours(raw: Vec<eckit_sys::geo::Neighbour>) -> Vec<Neighbour> {
    raw.into_iter().map(Neighbour::from).collect()
}

impl fmt::Debug for Search {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self.inner.to_string() {
            Ok(s) => f.write_str(&s),
            Err(err) => write!(f, "<search unavailable: {err}>"),
        }
    }
}

/// Statistics of the process-wide cache of searches: hits, misses, evictions,
/// the number of searches cached and their memory usage.
pub fn search_cache_statistics() -> Result<SearchCacheStatistics> {
    eckit_sys::geo::SearchWrapper::cache_statistics().map_err(eckit_sys::Error::from)
}

/// Evict every cached search. Live [`Search`]es stay valid.
pub fn search_cache_clear() -> Result<()> {
    eckit_sys::geo::SearchWrapper::cache_clear().map_err(eckit_sys::Error::from)
}
