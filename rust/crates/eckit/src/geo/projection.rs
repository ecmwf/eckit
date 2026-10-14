// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

//! Projections between geographic and projected coordinates.

use std::fmt;

use crate::error::Result;
use crate::geo::{Figure, Point, Spec};

/// A projection, from a source to a target point type.
///
/// Borrowed from the [`Grid`](crate::Grid) holding it. `fwd` projects from the
/// source type (geographic, for most) to the target (planar, or geographic
/// again for a rotation); `inv` the reverse. Point types are named by
/// [`Projection::source_point_coordinates`] and
/// [`Projection::target_point_coordinates`].
///
/// ```
/// # fn main() -> eckit::Result<()> {
/// # eckit::init();
/// use eckit::geo::Point;
///
/// let grid = eckit::Grid::from_spec(r#"{"grid":[5,5],"rotation":[-40,10]}"#)?;
/// let rotation = grid.projection();
/// assert_eq!(rotation.projection_type()?, "rotation");
///
/// // the rotated south pole is the given south pole
/// let Point::LonLat { lat, .. } = rotation.fwd(Point::LonLat { lon: 0.0, lat: -90.0 })? else {
///     unreachable!("rotations map lon/lat to lon/lat");
/// };
/// assert!((lat + 40.0).abs() < 1e-9);
/// # Ok(())
/// # }
/// ```
#[derive(Clone, Copy)]
pub struct Projection<'g> {
    inner: &'g eckit_sys::geo::ProjectionWrapper,
}

impl<'g> Projection<'g> {
    pub(crate) const fn new(inner: &'g eckit_sys::geo::ProjectionWrapper) -> Self {
        Self { inner }
    }

    /// Projection type, e.g. `rotation`, `lcc`, `eqc`, `space_view`; `none`
    /// for the default.
    pub fn projection_type(&self) -> Result<String> {
        self.inner.projection_type().map_err(eckit_sys::Error::from)
    }

    /// The canonical spec, with the figure unless the default; empty for the
    /// default projection.
    #[must_use]
    pub fn spec(&self) -> Spec<'g> {
        Spec {
            inner: self.inner.spec(),
        }
    }

    /// PROJ string of the projection and its figure, for the types with a
    /// PROJ equivalent.
    pub fn proj_str(&self) -> Result<String> {
        self.inner.proj_str().map_err(eckit_sys::Error::from)
    }

    /// Whether this is the default projection, the identity on geographic
    /// coordinates.
    pub fn is_default(&self) -> Result<bool> {
        self.inner.is_default().map_err(eckit_sys::Error::from)
    }

    /// Whether `fwd` and `inv` are swapped (spec `"reverse": true`), as for
    /// grids whose points are given in projected coordinates.
    pub fn is_reverse(&self) -> Result<bool> {
        self.inner.reverse().map_err(eckit_sys::Error::from)
    }

    /// Coordinate names of the source points, e.g. `["longitude", "latitude"]`.
    pub fn source_point_coordinates(&self) -> Result<Vec<String>> {
        self.inner
            .source_point_coordinates()
            .map_err(eckit_sys::Error::from)
    }

    /// Coordinate names of the target points, e.g. `["x", "y"]`.
    pub fn target_point_coordinates(&self) -> Result<Vec<String>> {
        self.inner
            .target_point_coordinates()
            .map_err(eckit_sys::Error::from)
    }

    /// The figure the projection is on.
    #[must_use]
    pub fn figure(&self) -> Figure<'g> {
        Figure::new(self.inner.figure())
    }

    /// Project a point from the source to the target point type.
    pub fn fwd(&self, point: Point) -> Result<Point> {
        self.inner
            .fwd(point.into())
            .map(Point::from)
            .map_err(eckit_sys::Error::from)
    }

    /// Project a point from the target to the source point type.
    pub fn inv(&self, point: Point) -> Result<Point> {
        self.inner
            .inv(point.into())
            .map(Point::from)
            .map_err(eckit_sys::Error::from)
    }

    /// Project points given one slice per source coordinate (2 or 3, all of
    /// the same length), returning one vector per target coordinate. Points
    /// that fail to project are NaN.
    ///
    /// ```
    /// # fn main() -> eckit::Result<()> {
    /// # eckit::init();
    /// let grid = eckit::Grid::from_spec(r#"{"grid":[5,5],"rotation":[-40,10]}"#)?;
    /// let rotated = grid.projection().fwd_vectors(&[&[0.0, 10.0], &[0.0, 20.0]])?;
    /// assert_eq!(rotated.len(), 2);
    /// assert_eq!(rotated[0].len(), 2);
    /// # Ok(())
    /// # }
    /// ```
    pub fn fwd_vectors(&self, coords: &[&[f64]]) -> Result<Vec<Vec<f64>>> {
        let [v1, v2, v3] = split(coords)?;
        let mut out = vec![vec![0.0; v1.len()]; self.inner.target_dims()?];
        let [o1, o2, o3] = split_mut(&mut out)?;
        self.inner.fwd_vectors(v1, v2, v3, o1, o2, o3)?;
        Ok(out)
    }

    /// Project points from the target to the source point type; see
    /// [`Projection::fwd_vectors`].
    pub fn inv_vectors(&self, coords: &[&[f64]]) -> Result<Vec<Vec<f64>>> {
        let [v1, v2, v3] = split(coords)?;
        let mut out = vec![vec![0.0; v1.len()]; self.inner.source_dims()?];
        let [o1, o2, o3] = split_mut(&mut out)?;
        self.inner.inv_vectors(v1, v2, v3, o1, o2, o3)?;
        Ok(out)
    }
}

/// One slice per coordinate, the third empty for points of 2 coordinates.
fn split<'a>(coords: &[&'a [f64]]) -> Result<[&'a [f64]; 3]> {
    match coords {
        [v1, v2] => Ok([v1, v2, &[]]),
        [v1, v2, v3] => Ok([v1, v2, v3]),
        _ => Err(eckit_sys::Error::Other(format!(
            "projection: expected 2 or 3 coordinate slices, got {}",
            coords.len()
        ))),
    }
}

fn split_mut(coords: &mut [Vec<f64>]) -> Result<[&mut [f64]; 3]> {
    match coords {
        [v1, v2] => Ok([v1, v2, &mut []]),
        [v1, v2, v3] => Ok([v1, v2, v3]),
        _ => Err(eckit_sys::Error::Other(format!(
            "projection: expected 2 or 3 coordinates, got {}",
            coords.len()
        ))),
    }
}

/// Equality by spec, however the projections were described. Projections
/// whose spec cannot be read compare unequal.
impl PartialEq<Projection<'_>> for Projection<'_> {
    fn eq(&self, other: &Projection<'_>) -> bool {
        self.inner.equals(other.inner).unwrap_or(false)
    }
}

impl fmt::Debug for Projection<'_> {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        f.debug_tuple("Projection").field(&self.spec()).finish()
    }
}
