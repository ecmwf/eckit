// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

//! Figures: the sphere or spheroid grid points are on.

use std::fmt;

use crate::error::Result;
use crate::geo::{BoundingBox, Spec};

/// A figure: a "shape" (sphere, spheroid) and "size" (radius, or semi-axes).
///
/// Borrowed from the [`Projection`](crate::geo::Projection) holding it, and so
/// from the [`Grid`](crate::Grid). The default figure is the spherical Earth of
/// radius 6371229 m; named figures include `wgs84`, `grs80` and `grib1`.
///
/// ```
/// # fn main() -> eckit::Result<()> {
/// # eckit::init();
/// let grid = eckit::Grid::from_spec(r#"{"grid":[30,30],"figure":"wgs84"}"#)?;
/// let figure = grid.figure();
/// assert!(!figure.is_spherical()?);
/// assert_eq!(figure.a()?, 6378137.0);
/// assert_eq!(figure.spec().to_json_str()?, r#"{"figure":"wgs84"}"#);
/// # Ok(())
/// # }
/// ```
#[derive(Clone, Copy)]
pub struct Figure<'g> {
    inner: &'g eckit_sys::geo::FigureWrapper,
}

impl<'g> Figure<'g> {
    pub(crate) const fn new(inner: &'g eckit_sys::geo::FigureWrapper) -> Self {
        Self { inner }
    }

    /// Radius; an [`Error::FigureError`](crate::Error) for a non-spherical
    /// figure.
    pub fn radius(&self) -> Result<f64> {
        self.inner.radius().map_err(eckit_sys::Error::from)
    }

    /// Semi-major axis (the radius, for a sphere).
    pub fn a(&self) -> Result<f64> {
        self.inner.a().map_err(eckit_sys::Error::from)
    }

    /// Semi-minor axis (the radius, for a sphere).
    pub fn b(&self) -> Result<f64> {
        self.inner.b().map_err(eckit_sys::Error::from)
    }

    /// Surface area, in the figure's units squared.
    pub fn area(&self) -> Result<f64> {
        self.inner.area().map_err(eckit_sys::Error::from)
    }

    /// Surface area between the parallels and meridians of a bounding box.
    pub fn area_within(&self, bbox: &BoundingBox) -> Result<f64> {
        self.inner
            .area_within(bbox.as_sys())
            .map_err(eckit_sys::Error::from)
    }

    /// The spec, e.g. `{"figure":"earth"}` or `{"figure":{"a":1,"b":0.5}}`.
    #[must_use]
    pub fn spec(&self) -> Spec<'g> {
        Spec {
            inner: self.inner.spec(),
        }
    }

    /// PROJ string of the figure, e.g. `+R=6371229` or `+ellps=WGS84`.
    pub fn proj_str(&self) -> Result<String> {
        self.inner.proj_str().map_err(eckit_sys::Error::from)
    }

    /// Whether the semi-axes are equal.
    pub fn is_spherical(&self) -> Result<bool> {
        self.inner.spherical().map_err(eckit_sys::Error::from)
    }

    /// First eccentricity, `0` for a sphere.
    pub fn eccentricity(&self) -> Result<f64> {
        self.inner.eccentricity().map_err(eckit_sys::Error::from)
    }

    /// Flattening `1 - b / a`, `0` for a sphere.
    pub fn flattening(&self) -> Result<f64> {
        self.inner.flattening().map_err(eckit_sys::Error::from)
    }

    /// Whether this is the default figure, the spherical Earth.
    pub fn is_default(&self) -> Result<bool> {
        self.inner.is_default().map_err(eckit_sys::Error::from)
    }
}

/// Equality by size, however the figures were described. Figures whose size
/// cannot be read compare unequal.
impl PartialEq<Figure<'_>> for Figure<'_> {
    fn eq(&self, other: &Figure<'_>) -> bool {
        self.inner.equals(other.inner).unwrap_or(false)
    }
}

impl fmt::Debug for Figure<'_> {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        f.debug_tuple("Figure").field(&self.spec()).finish()
    }
}
