// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

//! Figures, as borrowed from their grids. Mirrors the parts of
//! `python/eckit/tests/geo/test_figure.py` reachable through a grid.

mod common;

use std::f64::consts::PI;

use common::{bbox, grid};

const EARTH_RADIUS: f64 = 6_371_229.0; // [m] from eckit::geo::figure::DatumIFS::radius
const EARTH_AREA: f64 = 510_101_140_207_791.56; // [m^2]

fn rel(a: f64, b: f64, tolerance: f64) -> bool {
    (a - b).abs() <= tolerance * b.abs()
}

#[test]
fn default_figure_is_the_spherical_earth() {
    for spec in [
        r#"{"grid":"O32"}"#,
        r#"{"grid":"N32"}"#,
        r#"{"grid":"1/1"}"#,
    ] {
        let grid = grid(spec);
        let figure = grid.figure();

        assert!(figure.is_default().expect("default"), "{spec}");
        assert!(figure.is_spherical().expect("spherical"), "{spec}");
        assert!(
            rel(figure.radius().expect("R"), EARTH_RADIUS, 0.0),
            "{spec}"
        );
        assert!(rel(figure.a().expect("a"), EARTH_RADIUS, 0.0), "{spec}");
        assert!(rel(figure.b().expect("b"), EARTH_RADIUS, 0.0), "{spec}");
        assert!(figure.eccentricity().expect("e").abs() < 1e-12, "{spec}");
        assert!(figure.flattening().expect("f").abs() < 1e-12, "{spec}");
        assert_eq!(
            figure.spec().to_json_str().expect("json"),
            r#"{"figure":"earth"}"#,
            "{spec}"
        );
        assert_eq!(
            figure.spec().get::<String>("figure").expect("figure"),
            "earth",
            "{spec}"
        );

        // the projection's figure is the grid's
        assert_eq!(grid.projection().figure(), figure, "{spec}");
    }
}

#[test]
fn sphere_areas() {
    let grid = grid(r#"{"grid":"1/1"}"#);
    let figure = grid.figure();

    let globe = bbox(90.0, -180.0, -90.0, 180.0);
    let north = bbox(90.0, -180.0, 0.0, 180.0);
    let south = bbox(0.0, -180.0, -90.0, 180.0);

    let area = figure.area().expect("area");
    assert!(rel(area, EARTH_AREA, 1e-9), "{area}");
    assert!(rel(area, 4.0 * PI * EARTH_RADIUS * EARTH_RADIUS, 1e-9));
    assert!(rel(figure.area_within(&globe).expect("area"), area, 1e-9));
    assert!(rel(
        figure.area_within(&north).expect("area"),
        area / 2.0,
        1e-9
    ));
    assert!(rel(
        figure.area_within(&north).expect("area"),
        figure.area_within(&south).expect("area"),
        1e-9
    ));
}

#[test]
fn wgs84() {
    const A: f64 = 6_378_137.0; // [m] from eckit::geo::figure::DatumWgs84
    const B: f64 = 6_356_752.314_245;
    const AREA: f64 = 510_065_621_724_079.0;
    const INVERSE_FLATTENING: f64 = 298.257_223_563;

    let grid = grid(r#"{"grid":[30,30],"figure":"wgs84"}"#);
    let figure = grid.figure();

    assert!(!figure.is_default().expect("default"));
    assert!(!figure.is_spherical().expect("spherical"));
    assert!(rel(figure.a().expect("a"), A, 1e-12));
    assert!(rel(figure.b().expect("b"), B, 1e-12));
    let e = figure.eccentricity().expect("e");
    assert!(0.0 < e && e < 1.0, "{e}");
    let f = figure.flattening().expect("f");
    assert!(rel(f, 1.0 - B / A, 1e-9));
    assert!(rel(1.0 / f, INVERSE_FLATTENING, 1e-9));
    assert_eq!(
        figure.spec().to_json_str().expect("json"),
        r#"{"figure":"wgs84"}"#
    );
    assert!(figure.proj_str().expect("proj").contains("WGS84"));

    // the radius is not defined for a != b
    assert!(figure.radius().is_err());

    let globe = bbox(90.0, -180.0, -90.0, 180.0);
    let north = bbox(90.0, -180.0, 0.0, 180.0);
    let area = figure.area().expect("area");
    assert!(rel(area, AREA, 1e-9), "{area}");
    assert!(rel(figure.area_within(&globe).expect("area"), AREA, 1e-9));
    assert!(rel(
        figure.area_within(&north).expect("area"),
        AREA / 2.0,
        1e-9
    ));
}

#[test]
fn equality_is_by_size() {
    let earth = grid(r#"{"grid":[30,30]}"#);
    let by_radius = grid(r#"{"grid":[30,30],"figure":{"R":6371229}}"#);
    let by_axes = grid(r#"{"grid":[30,30],"figure":{"a":6371229,"b":6371229}}"#);
    let grib1 = grid(r#"{"grid":[30,30],"figure":"grib1"}"#);
    let wgs84 = grid(r#"{"grid":[30,30],"figure":"wgs84"}"#);
    let grs80 = grid(r#"{"grid":[30,30],"figure":"grs80"}"#);

    assert_eq!(earth.figure(), by_radius.figure());
    assert_eq!(earth.figure(), by_axes.figure());
    assert_ne!(earth.figure(), grib1.figure());
    assert!(rel(grib1.figure().radius().expect("R"), 6_367_470.0, 0.0));

    // same semi-major axis, different flattening
    assert_ne!(wgs84.figure(), grs80.figure());
    assert!(rel(
        wgs84.figure().a().expect("a"),
        grs80.figure().a().expect("a"),
        0.0
    ));
}
