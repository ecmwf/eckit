// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

//! Projections, as borrowed from their grids. Mirrors the parts of
//! `python/eckit/tests/geo/test_projection.py` reachable through a grid.

mod common;

use common::{grid, lonlat};
use eckit::geo::Point;

const EPS_LL: f64 = 1e-9; // [degree]
const EPS_XY: f64 = 1e-6; // [m]

fn lonlat_of(point: Point) -> (f64, f64) {
    match point {
        Point::LonLat { lon, lat } => (lon, lat),
        other => panic!("expected a (lon, lat) point, got {other:?}"),
    }
}

fn xy_of(point: Point) -> (f64, f64) {
    match point {
        Point::Xy { x, y } => (x, y),
        other => panic!("expected an (x, y) point, got {other:?}"),
    }
}

fn close(a: (f64, f64), b: (f64, f64), eps: f64) -> bool {
    (a.0 - b.0).abs() <= eps && (a.1 - b.1).abs() <= eps
}

const ROTATION: &str = r#"{"grid":[5,5],"rotation":[-40,10]}"#;

const LCC: &str = r#"{"x":[0,120000,2500],"y":[0,170000,2500],"false":[60000.021,85000.055],"projection":{"type":"lcc","lat_0":51.967,"lat_1":51.967,"lon_0":4.9,"reverse":true}}"#;

#[test]
fn default_projection_of_a_lonlat_grid() {
    let grid = grid(r#"{"grid":[1,1]}"#);
    let projection = grid.projection();

    // equidistant cylindrical: (lon, lat) to (x, y) in degree, the identity in value
    assert!(projection.is_default().expect("default"));
    assert!(!projection.is_reverse().expect("reverse"));
    assert_eq!(projection.projection_type().expect("type"), "eqc");
    assert_eq!(projection.spec().to_json_str().expect("json"), "{}");
    assert_eq!(
        projection.source_point_coordinates().expect("source"),
        ["longitude", "latitude"]
    );
    assert_eq!(
        projection.target_point_coordinates().expect("target"),
        ["x", "y"]
    );
    assert_eq!(
        xy_of(projection.fwd(lonlat(10.0, 20.0)).expect("fwd")),
        (10.0, 20.0)
    );
    assert_eq!(
        lonlat_of(projection.inv(Point::Xy { x: 10.0, y: 20.0 }).expect("inv")),
        (10.0, 20.0)
    );
    assert_eq!(projection.figure(), grid.figure());
}

#[test]
fn rotation() {
    let grid = grid(ROTATION);
    let rotation = grid.projection();

    assert_eq!(rotation.projection_type().expect("type"), "rotation");
    assert!(!rotation.is_default().expect("default"));
    assert_eq!(
        rotation.spec().get::<String>("type").expect("type"),
        "rotation"
    );
    assert_eq!(
        rotation.spec().get::<Vec<f64>>("south_pole").expect("pole"),
        [10.0, -40.0]
    );
    assert_eq!(
        rotation.source_point_coordinates().expect("source"),
        ["longitude", "latitude"]
    );
    assert_eq!(
        rotation.target_point_coordinates().expect("target"),
        ["longitude", "latitude"]
    );

    // fwd: geographic (lon, lat) to the rotated sphere (as PROJ ob_tran), inv the reverse; the
    // rotated south pole is the given south pole, and the rotated equator is 90 degree away from it
    assert!((lonlat_of(rotation.inv(lonlat(0.0, -90.0)).expect("inv")).1 + 40.0).abs() < EPS_LL);
    assert!((lonlat_of(rotation.fwd(lonlat(10.0, -40.0)).expect("fwd")).1 + 90.0).abs() < EPS_LL);
    assert!(close(
        lonlat_of(rotation.inv(lonlat(0.0, 0.0)).expect("inv")),
        (10.0, 50.0),
        EPS_LL
    ));

    for p in [(0.0, 0.0), (10.0, 20.0), (-30.0, 45.0), (170.0, -60.0)] {
        let q = rotation.fwd(lonlat(p.0, p.1)).expect("fwd");
        assert!(
            close(lonlat_of(rotation.inv(q).expect("inv")), p, EPS_LL),
            "{p:?}"
        );
    }
}

#[test]
fn rotation_vectors_match_points() {
    let grid = grid(ROTATION);
    let rotation = grid.projection();

    let lon = [0.0, 10.0, -30.0, 170.0, 45.0];
    let lat = [0.0, 20.0, 45.0, -60.0, 89.0];
    let rotated = rotation.fwd_vectors(&[&lon, &lat]).expect("fwd");
    assert_eq!(rotated.len(), 2);
    for i in 0..lon.len() {
        let point = lonlat_of(rotation.fwd(lonlat(lon[i], lat[i])).expect("fwd"));
        assert!(
            close((rotated[0][i], rotated[1][i]), point, EPS_LL),
            "[{i}]"
        );
    }

    let back = rotation
        .inv_vectors(&[&rotated[0], &rotated[1]])
        .expect("inv");
    for i in 0..lon.len() {
        assert!(
            close((back[0][i], back[1][i]), (lon[i], lat[i]), EPS_LL),
            "[{i}]"
        );
    }

    // no points, and the wrong number of coordinates
    assert_eq!(
        rotation.fwd_vectors(&[&[], &[]]).expect("empty"),
        [Vec::<f64>::new(), Vec::new()]
    );
    assert!(rotation.fwd_vectors(&[&lon]).is_err());
    assert!(rotation.fwd_vectors(&[&lon, &lat, &lat]).is_err());
    assert!(rotation.fwd_vectors(&[&lon, &lat[..2]]).is_err());
}

#[test]
fn equality_is_by_spec() {
    let a = grid(ROTATION);
    let b = grid(r#"{"grid":[5,5],"projection":{"type":"rotation","south_pole":[10,-40]}}"#);
    let c = grid(r#"{"grid":[5,5],"rotation":[-40,20]}"#);
    let plain = grid(r#"{"grid":[5,5]}"#);

    assert_eq!(a.projection(), b.projection());
    assert_ne!(a.projection(), c.projection());
    assert_ne!(a.projection(), plain.projection());
    assert_eq!(a.figure(), plain.figure());
}

#[test]
fn reversed_projection_of_a_projected_grid() {
    let grid = grid(LCC);
    let lcc = grid.projection();

    assert_eq!(lcc.projection_type().expect("type"), "lcc");
    assert!(lcc.is_reverse().expect("reverse"));
    assert!(lcc.spec().get::<bool>("reverse").expect("reverse"));

    // reversed: the grid's (x, y) are the source, geographic (lon, lat) the target
    assert_eq!(lcc.source_point_coordinates().expect("source"), ["x", "y"]);
    assert_eq!(
        lcc.target_point_coordinates().expect("target"),
        ["longitude", "latitude"]
    );

    // the projection origin (lon_0, lat_0) is (x, y) = (0, 0); the grid's false
    // easting and northing are the grid's business, not the projection's
    let origin = Point::Xy { x: 0.0, y: 0.0 };
    let (lon, lat) = lonlat_of(lcc.fwd(origin).expect("fwd"));
    assert!(close((lon, lat), (4.9, 51.967), 1e-9), "({lon}, {lat})");
    assert!(close(
        xy_of(lcc.inv(lonlat(lon, lat)).expect("inv")),
        (0.0, 0.0),
        EPS_XY
    ));

    // the grid points are geographic, around the origin
    assert_eq!(
        grid.point_coordinates().expect("coordinates"),
        ["longitude", "latitude"]
    );
    let (grid_lat, grid_lon) = grid.to_latlons().expect("latlons");
    assert_eq!(grid_lat.len(), grid.len().expect("len"));
    assert!(grid_lon.iter().all(|lon| (3.0..7.0).contains(lon)));
    assert!(grid_lat.iter().all(|lat| (50.0..54.0).contains(lat)));

    assert!(lcc.proj_str().expect("proj").contains("+proj=lcc"));
    assert!(lcc.figure().is_default().expect("default"));
}
