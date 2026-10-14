// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

//! Areas selecting grid points: bounding boxes, polygons and shapefile
//! features.

use std::fmt;

use crate::error::Result;
use crate::geo::Spec;

/// An area on the Earth, described by a spec.
///
/// Types, by their spec:
/// - a bounding box, `{"area":[north,west,south,east]}` (see [`BoundingBox`]);
/// - a polygon of one ring, `{"type":"polygon","latitudes":[...],"longitudes":[...]}`,
///   or of several, `{"type":"polygon","uid":["<uid>",...]}` with each ring
///   an unstructured grid ([`Grid::from_latlons`](crate::Grid::from_latlons));
///   rings keep their coordinates in eckit geo's cache (see the [module
///   docs](crate::geo#caching));
/// - a shapefile feature, `{"type":"shapefile","file":...,"name":...}`, with
///   eckit built with shapefile support.
///
/// Areas crop grids ([`Grid::crop`](crate::Grid::crop)) and compare equal by
/// uid, however they are described.
pub struct Area {
    inner: eckit_sys::UniquePtr<eckit_sys::geo::AreaWrapper>,
}

// SAFETY: `AreaWrapper` owns its `eckit::geo::Area` outright, so moving one
// between threads transfers sole ownership. Not `Sync`: the area caches its
// spec from `const` methods without synchronisation.
#[allow(clippy::non_send_fields_in_send_ty)]
unsafe impl Send for Area {}

impl Area {
    pub(crate) const fn wrap(inner: eckit_sys::UniquePtr<eckit_sys::geo::AreaWrapper>) -> Self {
        Self { inner }
    }

    /// Build an area from a spec, a YAML/JSON mapping.
    ///
    /// ```
    /// # fn main() -> eckit::Result<()> {
    /// # eckit::init();
    /// let area = eckit::geo::Area::from_spec(r#"{"north":90,"west":0,"south":-90,"east":360}"#)?;
    /// assert_eq!(area.area_type()?, "bounding_box");
    /// assert_eq!(area.spec().to_json_str()?, r#"{"area":[90,0,-90,360]}"#);
    /// # Ok(())
    /// # }
    /// ```
    pub fn from_spec(spec: &str) -> Result<Self> {
        eckit_sys::geo::AreaWrapper::from_spec(spec)
            .map(Self::wrap)
            .map_err(eckit_sys::Error::from)
    }

    /// Area type: `bounding_box`, `polygon`, `shapefile`, ...
    pub fn area_type(&self) -> Result<String> {
        self.inner.area_type().map_err(eckit_sys::Error::from)
    }

    /// Identifies the area, independently of how it is specified.
    pub fn uid(&self) -> Result<String> {
        self.inner.uid().map_err(eckit_sys::Error::from)
    }

    /// The canonical spec, borrowed from the area.
    #[must_use]
    pub fn spec(&self) -> Spec<'_> {
        Spec {
            inner: self.inner.spec(),
        }
    }

    /// Whether this is the default area, the whole globe.
    pub fn is_default(&self) -> Result<bool> {
        self.inner.is_default().map_err(eckit_sys::Error::from)
    }

    /// Whether the point, in degrees, is in the area.
    pub fn contains(&self, lon: f64, lat: f64) -> Result<bool> {
        self.inner
            .contains(lon, lat)
            .map_err(eckit_sys::Error::from)
    }

    /// Whether the area and the bounding box overlap.
    pub fn intersects(&self, bbox: &BoundingBox) -> Result<bool> {
        self.inner
            .intersects(bbox.as_sys())
            .map_err(eckit_sys::Error::from)
    }

    /// Surface area, on the Earth, in square metres.
    pub fn surface_area(&self) -> Result<f64> {
        self.inner.surface_area().map_err(eckit_sys::Error::from)
    }

    /// Whether the area is a bounding box, so [`Area::into_bounding_box`]
    /// succeeds.
    #[must_use]
    pub fn is_bounding_box(&self) -> bool {
        self.inner.is_bounding_box()
    }

    /// This area as a [`BoundingBox`]; an [`Error::AreaError`](crate::Error)
    /// for areas of other types.
    pub fn into_bounding_box(self) -> Result<BoundingBox> {
        let extent = self.inner.bbox().map_err(eckit_sys::Error::from)?;
        Ok(BoundingBox { area: self, extent })
    }

    pub(crate) fn as_sys(&self) -> &eckit_sys::geo::AreaWrapper {
        &self.inner
    }
}

impl AsRef<Self> for Area {
    fn as_ref(&self) -> &Self {
        self
    }
}

/// Equality by uid, however the areas were described. Areas whose uid cannot
/// be computed compare unequal.
impl PartialEq for Area {
    fn eq(&self, other: &Self) -> bool {
        self.inner.equals(&other.inner).unwrap_or(false)
    }
}

impl fmt::Debug for Area {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        f.debug_tuple("Area").field(&self.spec()).finish()
    }
}

/// A bounding box: the [`Area`] between two parallels and two meridians, in
/// degrees.
///
/// Longitudes are normalised on construction: `west` into `[-180, 180)`, and
/// `east` into `[west, west + 360]`, so `east - west == 360` is a periodic
/// box.
pub struct BoundingBox {
    area: Area,
    extent: eckit_sys::geo::Bbox,
}

impl BoundingBox {
    /// Build a bounding box from its extent, in degrees.
    ///
    /// ```
    /// # fn main() -> eckit::Result<()> {
    /// # eckit::init();
    /// let globe = eckit::geo::BoundingBox::new(90.0, 0.0, -90.0, 360.0)?;
    /// assert!(globe.is_global()?);
    /// assert!(globe.is_periodic()?);
    /// assert_eq!(globe.spec().to_json_str()?, r#"{"area":[90,0,-90,360]}"#);
    /// # Ok(())
    /// # }
    /// ```
    pub fn new(north: f64, west: f64, south: f64, east: f64) -> Result<Self> {
        eckit_sys::geo::AreaWrapper::from_bbox(eckit_sys::geo::Bbox {
            north,
            west,
            south,
            east,
        })
        .map(Area::wrap)
        .map_err(eckit_sys::Error::from)?
        .into_bounding_box()
    }

    /// Build a bounding box from a spec, e.g. `{"area":[90,0,-90,360]}`; an
    /// [`Error::AreaError`](crate::Error) if the spec describes another area
    /// type.
    pub fn from_spec(spec: &str) -> Result<Self> {
        Area::from_spec(spec)?.into_bounding_box()
    }

    /// Northernmost latitude.
    #[must_use]
    pub const fn north(&self) -> f64 {
        self.extent.north
    }

    /// Westernmost longitude.
    #[must_use]
    pub const fn west(&self) -> f64 {
        self.extent.west
    }

    /// Southernmost latitude.
    #[must_use]
    pub const fn south(&self) -> f64 {
        self.extent.south
    }

    /// Easternmost longitude.
    #[must_use]
    pub const fn east(&self) -> f64 {
        self.extent.east
    }

    /// Whether the box covers the globe.
    pub fn is_global(&self) -> Result<bool> {
        self.area
            .inner
            .bbox_global()
            .map_err(eckit_sys::Error::from)
    }

    /// Whether the box spans all longitudes.
    pub fn is_periodic(&self) -> Result<bool> {
        self.area
            .inner
            .bbox_periodic()
            .map_err(eckit_sys::Error::from)
    }

    /// Whether the box has no extent.
    pub fn is_empty(&self) -> Result<bool> {
        self.area.inner.bbox_empty().map_err(eckit_sys::Error::from)
    }

    /// Whether `other` is within this box.
    pub fn contains(&self, other: &Self) -> Result<bool> {
        self.area
            .inner
            .bbox_contains(other.as_sys())
            .map_err(eckit_sys::Error::from)
    }

    /// Whether the point, in degrees, is in the box.
    pub fn contains_point(&self, lon: f64, lat: f64) -> Result<bool> {
        self.area.contains(lon, lat)
    }

    /// Whether the boxes overlap.
    pub fn intersects(&self, other: &Self) -> Result<bool> {
        self.area.intersects(other)
    }

    /// Surface area, on the Earth, in square metres.
    pub fn surface_area(&self) -> Result<f64> {
        self.area.surface_area()
    }

    /// Identifies the box, independently of how it is specified.
    pub fn uid(&self) -> Result<String> {
        self.area.uid()
    }

    /// The canonical spec, `{"area":[north,west,south,east]}`, borrowed from
    /// the box.
    #[must_use]
    pub fn spec(&self) -> Spec<'_> {
        self.area.spec()
    }

    pub(crate) fn as_sys(&self) -> &eckit_sys::geo::AreaWrapper {
        self.area.as_sys()
    }
}

/// The box as an [`Area`].
impl AsRef<Area> for BoundingBox {
    fn as_ref(&self) -> &Area {
        &self.area
    }
}

/// The box as an [`Area`], giving up the extent accessors.
impl From<BoundingBox> for Area {
    fn from(bbox: BoundingBox) -> Self {
        bbox.area
    }
}

/// Equality by uid, as [`Area`].
impl PartialEq for BoundingBox {
    fn eq(&self, other: &Self) -> bool {
        self.area == other.area
    }
}

impl fmt::Debug for BoundingBox {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        f.debug_struct("BoundingBox")
            .field("north", &self.north())
            .field("west", &self.west())
            .field("south", &self.south())
            .field("east", &self.east())
            .finish()
    }
}
