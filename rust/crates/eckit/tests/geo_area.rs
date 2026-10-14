// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

//! Areas and bounding boxes, and grids cropped by them. Mirrors
//! `python/eckit/tests/geo/test_area.py`, `test_bbox.py` and
//! `test_grid_crop.py`.

mod common;

use std::sync::OnceLock;

use eckit::geo::{Area, BoundingBox};
use eckit::{Grid, Result};

fn grid(spec: &str) -> Grid {
    common::init();
    Grid::from_spec(spec).unwrap_or_else(|err| panic!("grid {spec}: {err}"))
}

fn area(spec: &str) -> Area {
    common::init();
    Area::from_spec(spec).unwrap_or_else(|err| panic!("area {spec}: {err}"))
}

fn bbox(north: f64, west: f64, south: f64, east: f64) -> BoundingBox {
    common::init();
    BoundingBox::new(north, west, south, east).expect("bounding box")
}

/// Longitude difference, modulo 360.
fn lon_diff(a: f64, b: f64) -> f64 {
    (a - b + 180.0).rem_euclid(360.0) - 180.0
}

/// Whether the cropped grid points (lon, lat) are those of the grid, by index.
fn same_points(cropped: &Grid, grid: &Grid, indices: &[usize]) -> Result<bool> {
    let (lat, lon) = grid.to_latlons()?;
    let (cropped_lat, cropped_lon) = cropped.to_latlons()?;
    Ok(cropped_lat.len() == indices.len()
        && indices
            .iter()
            .zip(&cropped_lat)
            .all(|(&i, &c)| (c - lat[i]).abs() < 1e-9)
        && indices
            .iter()
            .zip(&cropped_lon)
            .all(|(&i, &c)| lon_diff(c, lon[i]).abs() < 1e-9))
}

/// Two disjoint rings, off the 1-degree grid points: 10 x 10 and 10 x 9
/// points inside. Rings are unstructured grids, the polygon refers to them by
/// uid.
///
/// The rings are built once: eckit registers unstructured grids by uid in a
/// process-wide registry, which tests building the same points concurrently
/// would race on.
fn polygons() -> Area {
    static SPEC: OnceLock<String> = OnceLock::new();

    let spec = SPEC.get_or_init(|| {
        common::init();
        let first = Grid::from_latlons(&[0.5, 0.5, 10.5, 10.5], &[0.5, 10.5, 10.5, 0.5], None)
            .expect("ring");
        let second = Grid::from_latlons(
            &[-19.5, -19.5, -10.5, -10.5],
            &[100.5, 110.5, 110.5, 100.5],
            None,
        )
        .expect("ring");
        format!(
            r#"{{"type":"polygon","uid":["{}","{}"]}}"#,
            first.uid().expect("uid"),
            second.uid().expect("uid")
        )
    });
    area(spec)
}

const LCC: &str = r#"{"x":[0,120000,2500],"y":[0,170000,2500],"false":[60000.021,85000.055],"projection":{"type":"lcc","lat_0":51.967,"lat_1":51.967,"lon_0":4.9,"reverse":true}}"#;

// ============== bounding boxes ==============

#[test]
fn bbox_spec_is_normalised() {
    for ((n, w, s, e), expected) in [
        ((90.0, 0.0, -90.0, 360.0), r#"{"area":[90,0,-90,360]}"#),
        // 99 and 42 turns around
        ((10.0, 35_641.0, 0.0, 15_130.0), r#"{"area":[10,1,0,10]}"#),
    ] {
        let bbox = bbox(n, w, s, e);
        assert_eq!(bbox.spec().to_json_str().expect("json"), expected);
        assert_eq!(bbox.as_area().area_type().expect("type"), "bounding_box");
        assert_eq!(BoundingBox::from_spec(expected).expect("from spec"), bbox);
    }
}

#[test]
fn bbox_global() {
    let bbox = bbox(90.0, 0.0, -90.0, 360.0);
    assert_eq!(
        (bbox.north(), bbox.west(), bbox.south(), bbox.east()),
        (90.0, 0.0, -90.0, 360.0)
    );
    assert!(bbox.is_global().expect("global"));
    assert!(!bbox.is_empty().expect("empty"));
    assert!(bbox.is_periodic().expect("periodic"));
    assert!(bbox.as_area().is_default().expect("default"));
}

#[test]
fn bbox_not_global() {
    let bbox = bbox(10.0, 0.0, 0.0, 10.0);
    assert!(!bbox.is_global().expect("global"));
    assert!(!bbox.is_empty().expect("empty"));
    assert!(!bbox.is_periodic().expect("periodic"));
    assert!(!bbox.as_area().is_default().expect("default"));
}

#[test]
fn bbox_intersects_and_contains() {
    let outer = bbox(10.0, 0.0, -10.0, 10.0);
    let inner = bbox(5.0, 2.0, -5.0, 8.0);
    let disjoint = bbox(20.0, 20.0, 15.0, 30.0);

    assert!(outer.intersects(&inner).expect("intersects"));
    assert!(outer.contains(&inner).expect("contains"));
    assert!(!inner.contains(&outer).expect("contains"));
    assert!(!outer.intersects(&disjoint).expect("intersects"));
    assert!(!outer.contains(&disjoint).expect("contains"));

    assert!(outer.contains_point(5.0, 5.0).expect("point"));
    assert!(!outer.contains_point(50.0, 50.0).expect("point"));
    assert!(outer.as_area().contains(5.0, 5.0).expect("point"));
}

#[test]
fn bbox_surface_area() {
    let globe = bbox(90.0, 0.0, -90.0, 360.0);
    let north = bbox(90.0, 0.0, 0.0, 360.0);
    let total = globe.surface_area().expect("area");
    assert!(total > 0.0);
    assert!((north.surface_area().expect("area") - total / 2.0).abs() < 1e-3 * total);
}

// ============== areas ==============

#[test]
fn area_specs() {
    for (spec, area_type, expected) in [
        (
            r#"{"north":90,"west":0,"south":-90,"east":360}"#,
            "bounding_box",
            r#"{"area":[90,0,-90,360]}"#,
        ),
        (
            r#"{"area":[10,1,0,10]}"#,
            "bounding_box",
            r#"{"area":[10,1,0,10]}"#,
        ),
        (
            r#"{"type":"bounding_box_xy","bounding_box_xy":[0,0,1,1]}"#,
            "bounding_box_xy",
            r#"{"bounding_box_xy":[0,0,1,1],"type":"bounding_box_xy"}"#,
        ),
    ] {
        let area = area(spec);
        assert_eq!(area.area_type().expect("type"), area_type, "{spec}");
        assert_eq!(area.spec().to_json_str().expect("json"), expected, "{spec}");
        assert_eq!(
            Area::from_spec(expected).expect("from own spec"),
            area,
            "{spec}"
        );
        assert_eq!(
            area.is_bounding_box(),
            area_type == "bounding_box",
            "{spec}"
        );
    }
}

#[test]
fn area_equality_is_by_uid() {
    assert_eq!(
        area(r#"{"north":90,"west":0,"south":-90,"east":360}"#),
        area(r#"{"area":[90,0,-90,360]}"#)
    );
    assert_ne!(
        area(r#"{"area":[10,1,0,10]}"#),
        area(r#"{"area":[10,0,0,10]}"#)
    );
    assert_eq!(
        area(r#"{"area":[10,1,0,10]}"#).uid().expect("uid"),
        bbox(10.0, 1.0, 0.0, 10.0).uid().expect("uid")
    );
}

#[test]
fn area_into_bounding_box() {
    let bbox = area(r#"{"area":[10,1,0,10]}"#)
        .into_bounding_box()
        .expect("bbox");
    assert_eq!(
        (bbox.north(), bbox.west(), bbox.south(), bbox.east()),
        (10.0, 1.0, 0.0, 10.0)
    );
    assert_eq!(bbox.into_area(), area(r#"{"area":[10,1,0,10]}"#));

    let polygon = polygons();
    assert!(!polygon.is_bounding_box());
    let json = polygon.spec().to_json_str().expect("json");
    assert!(polygon.into_bounding_box().is_err());
    assert!(BoundingBox::from_spec(&json).is_err());
}

#[test]
fn polygon() {
    common::init();
    // a ring of its own, not shared with the other tests (see `polygons`)
    let ring = Grid::from_latlons(&[20.5, 20.5, 30.5, 30.5], &[40.5, 50.5, 50.5, 40.5], None)
        .expect("ring");
    assert_eq!(ring.grid_type().expect("type"), "unstructured_ll");

    let polygon = area(
        r#"{"type":"polygon","latitudes":[20.5,20.5,30.5,30.5],"longitudes":[40.5,50.5,50.5,40.5]}"#,
    );
    assert_eq!(polygon.area_type().expect("type"), "polygon");
    assert_eq!(
        polygon.spec().get::<String>("type").expect("type"),
        "polygon"
    );
    assert_eq!(
        polygon.spec().get::<String>("uid").expect("uid"),
        ring.uid().expect("uid")
    );

    let json = polygon.spec().to_json_str().expect("json");
    assert_eq!(Area::from_spec(&json).expect("from own spec"), polygon);
    assert_eq!(
        area(&format!(
            r#"{{"type":"polygon","uid":"{}"}}"#,
            ring.uid().expect("uid")
        )),
        polygon
    );

    assert!(polygon.contains(45.0, 25.0).expect("contains"));
    assert!(!polygon.contains(5.0, 25.0).expect("contains"));

    assert!(
        Area::from_spec(r#"{"type":"polygon","latitudes":[0,0,10],"longitudes":[0,10]}"#).is_err()
    );
}

#[test]
fn polygon_rings_crop() {
    let grid = grid(r#"{"grid":[1,1]}"#);
    let polygons = polygons();
    assert_eq!(polygons.area_type().expect("type"), "polygon");
    assert_eq!(
        polygons
            .spec()
            .get::<Vec<String>>("uid")
            .expect("uids")
            .len(),
        2
    );
    assert_eq!(grid.crop(&polygons).expect("crop").len(), 10 * 10 + 10 * 9);
}

// ============== cropping grids ==============

#[test]
fn unprojected_grid_crop() {
    for spec in [r#"{"grid":[1,1]}"#, r#"{"grid":"O32"}"#] {
        let grid = grid(spec);
        for area in [
            bbox(10.0, -10.0, -10.0, 10.0).into_area(),
            area(r#"{"type":"bounding_box","north":60.5,"west":349.5,"south":40.5,"east":30.5}"#),
        ] {
            let indices = grid.crop(&area).expect("crop");
            assert!(!indices.is_empty(), "{spec} {area:?}");

            // the cropped grid has the same points, in the same order
            let cropped = grid.cropped(&area).expect("cropped");
            assert_eq!(
                cropped.len().expect("len"),
                indices.len(),
                "{spec} {area:?}"
            );
            assert!(
                same_points(&cropped, &grid, &indices).expect("points"),
                "{spec} {area:?}"
            );
        }
    }
}

#[test]
fn projected_grid_crop() {
    // areas of this test's own, as the cropped unstructured grids register by uid (see `polygons`)
    for (spec, area) in [
        (
            r#"{"grid":[1,1],"rotation":[-40,10]}"#,
            bbox(12.0, -12.0, -12.0, 12.0),
        ),
        (LCC, bbox(52.2, 4.4, 51.4, 5.6)),
    ] {
        let grid = grid(spec);

        // points in the area (geographic), as the grid
        let indices = grid.crop(&area).expect("crop");
        assert!(!indices.is_empty(), "{spec}");
        assert!(indices.is_sorted(), "{spec}");

        let (lat, lon) = grid.to_latlons().expect("latlons");
        for &i in &indices {
            assert!(
                area.contains_point(lon[i], lat[i]).expect("contains"),
                "{spec} [{i}]"
            );
        }

        // projected grids crop as unstructured grids, of the same points
        let cropped = grid.cropped(&area).expect("cropped");
        assert_eq!(
            cropped.grid_type().expect("type"),
            "unstructured_ll",
            "{spec}"
        );
        assert!(
            same_points(&cropped, &grid, &indices).expect("points"),
            "{spec}"
        );
    }
}

#[test]
fn cropped_grid_types() {
    let bbox10 = || bbox(10.0, -10.0, -10.0, 10.0).into_area();
    for (spec, area, grid_type) in [
        (r#"{"grid":[1,1]}"#, bbox10(), "regular_ll"),
        (r#"{"grid":"F32"}"#, bbox10(), "regular_gg"),
        (r#"{"grid":"O32"}"#, bbox10(), "reduced_gg"),
        (r#"{"grid":[1,1]}"#, polygons(), "unstructured_ll"),
        (
            r#"{"grid":[1,1],"rotation":[-40,10]}"#,
            bbox10(),
            "unstructured_ll",
        ),
        (
            LCC,
            bbox(52.0, 4.5, 51.5, 5.5).into_area(),
            "unstructured_ll",
        ),
    ] {
        let grid = grid(spec);
        let indices = grid.crop(&area).expect("crop");

        // of the same type if supported, otherwise unstructured
        let cropped = grid.cropped(&area).expect("cropped");
        assert_eq!(cropped.grid_type().expect("type"), grid_type, "{spec}");
        assert!(
            same_points(&cropped, &grid, &indices).expect("points"),
            "{spec}"
        );

        // as unstructured, always: its coordinates cached, exchangeable by uid
        let unstructured = grid.cropped_unstructured_ll(&area).expect("unstructured");
        assert_eq!(
            unstructured.grid_type().expect("type"),
            "unstructured_ll",
            "{spec}"
        );
        let by_uid = Grid::from_spec(&format!(
            r#"{{"uid":"{}"}}"#,
            unstructured.uid().expect("uid")
        ))
        .expect("by uid");
        assert!(
            same_points(&by_uid, &grid, &indices).expect("points"),
            "{spec}"
        );
    }
}

#[test]
fn global_grid_by_global_bounding_box_from_another_longitude() {
    // all points, rows from longitude -180 (180), the grids from longitude 0
    for (spec, grid_type) in [
        (r#"{"grid":[9,9]}"#, "regular_ll"),
        (r#"{"grid":"O2"}"#, "reduced_gg"),
    ] {
        let grid = grid(spec);
        let area = bbox(90.0, -180.0, -90.0, 360.0);

        let indices = grid.crop(&area).expect("crop");
        let mut sorted = indices.clone();
        sorted.sort_unstable();
        assert_eq!(
            sorted,
            (0..grid.len().expect("len")).collect::<Vec<_>>(),
            "{spec}"
        );
        assert_ne!(indices, sorted, "{spec}");

        let (lat, lon) = grid.to_latlons().expect("latlons");
        let first_of_rows = indices
            .iter()
            .enumerate()
            .filter(|&(n, &i)| n == 0 || (lat[i] - lat[indices[n - 1]]).abs() > 1e-12)
            .map(|(_, &i)| i);
        for i in first_of_rows {
            assert!(
                lon_diff(lon[i], -180.0).abs() < 1e-9,
                "{spec}: row from lon {}",
                lon[i]
            );
        }

        // the cropped grid, of the same type, has the same points in the same order
        let cropped = grid.cropped(&area).expect("cropped");
        assert_eq!(cropped.grid_type().expect("type"), grid_type, "{spec}");
        assert!(
            same_points(&cropped, &grid, &indices).expect("points"),
            "{spec}"
        );
    }
}

#[test]
fn empty_intersection_cannot_be_cropped() {
    let grid = grid(r#"{"area":[73,-27,33,45],"grid":[4,4]}"#);
    let far = bbox(-60.0, 100.0, -70.0, 110.0);
    assert_eq!(grid.crop(&far).expect("crop"), [] as [usize; 0]);
    assert!(grid.cropped_unstructured_ll(&far).is_err());
}
