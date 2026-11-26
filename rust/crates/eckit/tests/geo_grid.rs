// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

//! Grids: construction, identity, shape, ordering and coordinates. Mirrors
//! `python/eckit/tests/geo/test_grid.py` and `test_grid_reorder.py`.

mod common;

use common::{grid, lon_diff};
use eckit::Grid;
use eckit::geo::Point;

fn latlons(lat: &[f64], lon: &[f64], name: Option<&str>) -> Grid {
    common::init();
    Grid::from_latlons(lat, lon, name).expect("grid from latlons")
}

fn approx(a: f64, b: f64, eps: f64) -> bool {
    (a - b).abs() <= eps
}

// x and y increasing, so the grid order is i+j+
const XY: &str = r#"{"x":[0,2,1],"y":[0,1,1]}"#;

const fn xy(x: f64, y: f64) -> Point {
    Point::Xy { x, y }
}

#[test]
fn shapes() {
    for (spec, shape) in [
        (r#"{"grid":"1/1"}"#, vec![181, 360]),
        (r#"{"grid":"H2","order":"nested"}"#, vec![12 * 2 * 2]),
        (r#"{"grid":[2,2]}"#, vec![91, 180]),
        ("{grid: 3/3}", vec![61, 120]),
        (r#"{"grid":"o2"}"#, vec![20 + 24 + 24 + 20]),
    ] {
        assert_eq!(grid(spec).shape().expect("shape"), shape, "{spec}");
    }
}

#[test]
fn bare_names_and_specs() {
    common::init();
    // a bare name is read as {"grid": name}, so both forms give the same grid
    assert_eq!(
        Grid::from_spec("O1280").expect("bare name"),
        Grid::from_name("O1280").expect("name")
    );
    assert_eq!(
        grid(r#"{"grid":"o16"}"#)
            .spec()
            .to_json_str()
            .expect("json"),
        r#"{"grid":"O16"}"#
    );

    // `from_name` only takes names
    assert!(Grid::from_name("{grid: O1280}").is_err());
    assert!(Grid::from_name("").is_err());
    assert!(Grid::from_name("no-such-grid").is_err());
}

#[test]
fn unstructured_from_latlons() {
    let grid = latlons(&[1.0, 2.0, 3.0], &[4.0, 5.0, 6.0], None);
    let uid = grid.uid().expect("uid");

    assert_eq!(grid.grid_type().expect("type"), "unstructured_ll");
    assert_eq!(grid.shape().expect("shape"), [3]);
    assert_eq!(
        grid.spec().get::<String>("type").expect("type"),
        "unstructured_ll"
    );
    assert_eq!(grid.spec().get::<String>("uid").expect("uid"), uid);
    assert_eq!(
        grid,
        Grid::from_spec(&format!(r#"{{"uid":"{uid}"}}"#)).expect("by uid")
    );

    let (lat, lon) = grid.to_latlons().expect("latlons");
    assert_eq!(lat, [1.0, 2.0, 3.0]);
    assert_eq!(lon, [4.0, 5.0, 6.0]);

    // named: also by name, the name not changing the uid
    let name = "another-custom-grid";
    let named = latlons(&[1.0, 2.0, 3.0], &[4.0, 5.0, 6.0], Some(name));
    assert_eq!(named, Grid::from_name(name).expect("by name"));
    assert_eq!(named.spec().get::<String>("grid").expect("grid"), name);
    assert_eq!(named.uid().expect("uid"), uid);

    assert!(Grid::from_latlons(&[1.0, 2.0], &[4.0], None).is_err());
}

#[test]
fn unstructured_from_another_grid_points() {
    let h4 = grid(r#"{"grid":"H4","order":"nested"}"#);
    let (lat, lon) = h4.to_latlons().expect("latlons");

    let grid = latlons(&lat, &lon, None);
    assert_eq!(grid.shape().expect("shape"), [lat.len()]);
    assert_eq!(grid.to_latlons().expect("latlons"), (lat, lon));

    let uid = grid.uid().expect("uid");
    assert_eq!(
        grid,
        Grid::from_spec(&format!(r#"{{"uid":"{uid}"}}"#)).expect("by uid")
    );
}

#[test]
fn to_unstructured_ll() {
    // points of this test's own: eckit registers unstructured grids by uid, so
    // tests building the same points concurrently would race
    let grid = latlons(&[7.0, 8.0, 9.0], &[10.0, 11.0, 12.0], None);

    let a = grid.to_unstructured_ll(None).expect("unstructured");
    assert_eq!(a.grid_type().expect("type"), "unstructured_ll");
    assert_eq!(
        a.to_latlons().expect("latlons"),
        grid.to_latlons().expect("latlons")
    );
    assert_eq!(a.uid().expect("uid"), grid.uid().expect("uid"));

    let name = "custom-to-unstructured-ll";
    let b = grid.to_unstructured_ll(Some(name)).expect("unstructured");
    assert_eq!(b.uid().expect("uid"), grid.uid().expect("uid"));
    assert_eq!(b, Grid::from_name(name).expect("by name"));
}

#[test]
fn regular_from_uid() {
    let grid = grid(r#"{"grid":[0.5,0.5]}"#);
    let uid = grid.uid().expect("uid");
    assert_eq!(
        Grid::from_spec(&format!(r#"{{"uid":"{uid}"}}"#)).expect("by uid"),
        grid
    );
}

#[test]
fn regular_ll_spec_round_trips() {
    let spec = r#"{"area":[71.9834273,-25,25,49.9834833],"grid":[0.0166667,0.0166667],"reference":[5e-05,0.0166167]}"#;
    let grid = grid(spec);

    let area = grid.spec().get::<Vec<f64>>("area").expect("area");
    assert_eq!(area.len(), 4);
    assert!(approx(area[0], 71.983_427_3, 1e-9));
    assert_eq!(grid.spec().get::<Vec<f64>>("grid").expect("grid").len(), 2);
    assert!(grid.spec().has("reference"));

    let json = grid.spec().to_json_str().expect("json");
    assert_eq!(Grid::from_spec(&json).expect("from own spec"), grid);
}

#[test]
fn rotation_specs_are_canonical() {
    let canonical =
        grid(r#"{"grid":[5,5],"projection":{"south_pole":[-20,-40],"type":"rotation"}}"#);
    for spec in [
        r#"{"grid":[5,5],"rotation":[-40,-20]}"#,
        r#"{"grid":[5,5],"projection":{"south_pole_lon":-20,"south_pole_lat":-40,"type":"rotation"}}"#,
        r#"{"grid":[5,5],"projection":{"rotation":[-40,-20],"type":"rotation"}}"#,
    ] {
        let grid = grid(spec);
        assert_eq!(grid, canonical, "{spec}");
        assert!(grid.spec().has("projection"), "{spec}");
        assert_eq!(
            grid.projection().projection_type().expect("type"),
            "rotation"
        );
    }

    // the default south pole is not a rotation
    let plain = grid(r#"{"grid":[5,5]}"#);
    for spec in [
        r#"{"grid":[5,5],"rotation":[-90,0]}"#,
        r#"{"grid":[5,5],"projection":{"south_pole":[0,-90],"type":"rotation"}}"#,
    ] {
        let grid = grid(spec);
        assert_eq!(grid, plain, "{spec}");
        assert!(!grid.spec().has("projection"), "{spec}");
        assert!(grid.projection().is_default().expect("default"), "{spec}");
    }

    // south pole longitude and latitude are optional
    for (rotation, projection) in [
        ("[-90,-20]", r#"{"south_pole_lon":-20,"type":"rotation"}"#),
        ("[-40,0]", r#"{"south_pole_lat":-40,"type":"rotation"}"#),
    ] {
        assert_eq!(
            grid(&format!(r#"{{"grid":[5,5],"projection":{projection}}}"#)),
            grid(&format!(r#"{{"grid":[5,5],"rotation":{rotation}}}"#))
        );
    }
}

#[test]
fn rotated_gaussian_points() {
    let grid = grid(r#"{"grid":"F48","rotation":[30,30]}"#);
    assert_eq!(grid.grid_type().expect("type"), "regular_gg");
    assert!(grid.spec().has("projection"));

    let (lat, lon) = grid.to_latlons().expect("latlons");
    for (i, expected) in [
        -28.572_168_514_007_26,
        -28.572_922_306_258_01,
        -28.575_182_908_216_7,
    ]
    .into_iter()
    .enumerate()
    {
        assert!(approx(lat[i], expected, 1e-9), "lat[{i}] = {}", lat[i]);
    }
    for (i, expected) in [-150.0, -150.053_190_644_256_72, -150.106_326_661_154_95]
        .into_iter()
        .enumerate()
    {
        assert!(approx(lon[i], expected, 1e-9), "lon[{i}] = {}", lon[i]);
    }
}

#[test]
fn spec_carries_non_default_figure_only() {
    let plain = grid(r#"{"grid":[30,30]}"#);
    assert!(!plain.spec().has("figure"));
    assert_eq!(grid(r#"{"grid":[30,30],"figure":"earth"}"#), plain);

    let wgs84 = grid(r#"{"grid":[30,30],"figure":"wgs84"}"#);
    assert_eq!(
        wgs84.spec().get::<String>("figure").expect("figure"),
        "wgs84"
    );
    assert_ne!(wgs84, plain);
    assert_eq!(
        Grid::from_spec(&wgs84.spec().to_json_str().expect("json")).expect("from own spec"),
        wgs84
    );
}

#[test]
fn xyz_is_on_the_figure() {
    for (spec, a, b) in [
        (r#"{"grid":[30,30]}"#, 6_371_229.0, 6_371_229.0),
        (
            r#"{"grid":[30,30],"figure":"wgs84"}"#,
            6_378_137.0,
            6_356_752.314_245,
        ),
    ] {
        let grid = grid(spec);
        let (lat, _) = grid.to_latlons().expect("latlons");
        let (x, y, z) = grid.to_xyz().expect("xyz");
        let n = grid.len().expect("len");
        assert_eq!((x.len(), y.len(), z.len()), (n, n, n));

        // radius at the equator and the poles
        for i in 0..n {
            let radius = x[i].hypot(y[i]).hypot(z[i]);
            if lat[i].abs() < 1e-12 {
                assert!(approx(radius, a, 1e-3), "{spec}: equator radius {radius}");
            }
            if approx(lat[i].abs(), 90.0, 1e-12) {
                assert!(approx(radius, b, 1e-3), "{spec}: polar radius {radius}");
            }
        }

        let mut buffers = (vec![0.0; n], vec![0.0; n], vec![0.0; n]);
        grid.fill_xyz(&mut buffers.0, &mut buffers.1, &mut buffers.2)
            .expect("fill_xyz");
        assert_eq!(buffers, (x, y, z));
    }
}

#[test]
fn point_coordinates() {
    for (spec, coordinates) in [
        (r#"{"grid":[30,30]}"#, ["longitude", "latitude"]),
        (
            r#"{"grid":[30,30],"rotation":[-40,-20]}"#,
            ["longitude", "latitude"],
        ),
        (r#"{"grid":"o2"}"#, ["longitude", "latitude"]),
        (XY, ["x", "y"]),
    ] {
        assert_eq!(
            grid(spec).point_coordinates().expect("coordinates"),
            coordinates,
            "{spec}"
        );
    }
}

#[test]
fn regular_xy_points_and_order() {
    let grid = grid(XY);

    assert_eq!(grid.grid_type().expect("type"), "regular_xy");
    assert_eq!(grid.order().expect("order"), "i+j+");
    assert_eq!(grid.len().expect("len"), 6);
    assert!(grid.is_regular().expect("regular"));
    assert_eq!((grid.nx().expect("nx"), grid.ny().expect("ny")), (3, 2));
    assert_eq!(grid.x_values().expect("x"), [0.0, 1.0, 2.0]);
    assert_eq!(grid.y_values().expect("y"), [0.0, 1.0]);

    // the grid points are (x, y), which `to_latlons` cannot give
    assert!(grid.to_latlons().is_err());
    assert!(grid.to_xyz().is_err());
    assert_eq!(
        grid.to_points().expect("points"),
        [
            xy(0.0, 0.0),
            xy(1.0, 0.0),
            xy(2.0, 0.0),
            xy(0.0, 1.0),
            xy(1.0, 1.0),
            xy(2.0, 1.0)
        ]
    );
    assert_eq!(grid.first_point().expect("first"), xy(0.0, 0.0));
    assert_eq!(grid.last_point().expect("last"), xy(2.0, 1.0));
}

#[test]
fn regular_xy_reorder() {
    let grid = grid(XY);
    let points = grid.to_points().expect("points");

    // points in each order
    let orders: [(&str, [(f64, f64); 6]); 8] = [
        (
            "i+j-",
            [(0., 1.), (1., 1.), (2., 1.), (0., 0.), (1., 0.), (2., 0.)],
        ),
        (
            "i+j+",
            [(0., 0.), (1., 0.), (2., 0.), (0., 1.), (1., 1.), (2., 1.)],
        ),
        (
            "i-j-",
            [(2., 1.), (1., 1.), (0., 1.), (2., 0.), (1., 0.), (0., 0.)],
        ),
        (
            "i-j+",
            [(2., 0.), (1., 0.), (0., 0.), (2., 1.), (1., 1.), (0., 1.)],
        ),
        (
            "j+i-",
            [(2., 0.), (2., 1.), (1., 0.), (1., 1.), (0., 0.), (0., 1.)],
        ),
        (
            "j+i+",
            [(0., 0.), (0., 1.), (1., 0.), (1., 1.), (2., 0.), (2., 1.)],
        ),
        (
            "j-i-",
            [(2., 1.), (2., 0.), (1., 1.), (1., 0.), (0., 1.), (0., 0.)],
        ),
        (
            "j-i+",
            [(0., 1.), (0., 0.), (1., 1.), (1., 0.), (2., 1.), (2., 0.)],
        ),
    ];

    for (order, expected) in orders {
        // grid point k is point ren[k] in the order
        let ren = grid.reorder(order).expect("reorder");
        let mut sorted = ren.clone();
        sorted.sort_unstable();
        assert_eq!(sorted, [0, 1, 2, 3, 4, 5], "{order:?}");

        let renumbered: Vec<Point> = ren
            .iter()
            .map(|&r| xy(expected[r].0, expected[r].1))
            .collect();
        assert_eq!(points, renumbered, "{order:?}");
    }

    // the empty order is the default, i+j-
    assert_eq!(
        grid.reorder("").expect("default"),
        grid.reorder("i+j-").expect("i+j-")
    );
    assert!(grid.reorder("?").is_err());
}

#[test]
fn regular_xy_alternating_rows() {
    // rows (or columns) scanning in alternate directions, as GRIB alternativeRowScanning
    let ipos_jpos = [(0., 0.), (1., 0.), (2., 0.), (0., 1.), (1., 1.), (2., 1.)];
    for (order, expected) in [
        (
            "i+-j+",
            [(0., 0.), (1., 0.), (2., 0.), (2., 1.), (1., 1.), (0., 1.)],
        ),
        (
            "j+-i+",
            [(0., 0.), (0., 1.), (1., 1.), (1., 0.), (2., 0.), (2., 1.)],
        ),
    ] {
        let grid = grid(&format!(r#"{{"x":[0,2,1],"y":[0,1,1],"order":"{order}"}}"#));
        assert_eq!(grid.order().expect("order"), order);

        let points = grid.to_points().expect("points");
        let expected: Vec<Point> = expected.iter().map(|&(x, y)| xy(x, y)).collect();
        assert_eq!(points, expected, "{order}");

        // grid point k is point ren[k] in the order i+j+
        let ren = grid.reorder("i+j+").expect("reorder");
        let renumbered: Vec<Point> = ren
            .iter()
            .map(|&r| xy(ipos_jpos[r].0, ipos_jpos[r].1))
            .collect();
        assert_eq!(points, renumbered, "{order}");
    }
}

#[test]
fn order_of_structured_grids() {
    assert_eq!(grid(r#"{"grid":[30,30]}"#).order().expect("order"), "i+j-");
    assert_eq!(grid(r#"{"grid":"o2"}"#).order().expect("order"), "i+j-");
    assert_eq!(
        grid(r#"{"grid":"H2","order":"nested"}"#)
            .order()
            .expect("order"),
        "nested"
    );

    let unstructured = latlons(&[1.0, 2.0], &[3.0, 4.0], None);
    assert!(unstructured.order().is_err());
    assert!(unstructured.reorder("i+j-").is_err());
}

#[test]
fn rows_and_columns() {
    let regular = grid(r#"{"grid":[2,2]}"#);
    assert!(regular.is_regular().expect("regular"));
    assert!(regular.is_structured().expect("structured"));
    assert_eq!(
        (regular.nx().expect("nx"), regular.ny().expect("ny")),
        (180, 91)
    );
    assert_eq!(regular.pl().expect("pl"), vec![180; 91]);
    assert_eq!(regular.distinct_latitudes().expect("lats").len(), 91);
    assert_eq!(regular.distinct_longitudes().expect("lons").len(), 180);
    assert_eq!(
        regular.x_values().expect("x"),
        regular.distinct_longitudes().expect("lons")
    );
    assert_eq!(
        regular.y_values().expect("y"),
        regular.distinct_latitudes().expect("lats")
    );

    let reduced = grid(r#"{"grid":"o2"}"#);
    assert!(!reduced.is_regular().expect("regular"));
    assert!(reduced.is_structured().expect("structured"));
    assert_eq!(
        (reduced.nx().expect("nx"), reduced.ny().expect("ny")),
        (0, 4)
    );
    assert_eq!(reduced.pl().expect("pl"), [20, 24, 24, 20]);
    assert_eq!(reduced.distinct_latitudes().expect("lats").len(), 4);
    assert_eq!(reduced.distinct_longitudes().expect("lons"), [] as [f64; 0]);
    assert_eq!(reduced.x_values().expect("x"), [] as [f64; 0]);

    let cartesian = grid(XY);
    assert_eq!(
        cartesian.distinct_latitudes().expect("lats"),
        [] as [f64; 0]
    );
    assert_eq!(
        cartesian.distinct_longitudes().expect("lons"),
        [] as [f64; 0]
    );

    let unstructured = latlons(&[5.0, 6.0], &[7.0, 8.0], None);
    assert!(!unstructured.is_structured().expect("structured"));
    assert_eq!(unstructured.pl().expect("pl"), [] as [i64; 0]);
    assert_eq!(
        unstructured.distinct_latitudes().expect("lats"),
        [] as [f64; 0]
    );
}

#[test]
fn truncation() {
    let sh = grid(r#"{"type":"sh","truncation":21}"#);
    assert_eq!(sh.grid_type().expect("type"), "sh");
    assert_eq!(sh.truncation().expect("truncation"), Some(21));
    assert_eq!(
        grid(r#"{"grid":"o2"}"#).truncation().expect("truncation"),
        None
    );
}

#[test]
fn name_and_arrangement() {
    // named grids know their name; none carries an arrangement unless staggered (ORCA, FESOM, ICON)
    let named = grid(r#"{"grid":"o2"}"#);
    assert_eq!(named.name().expect("name"), "O2");
    assert_eq!(named.arrangement().expect("arrangement"), "");

    let unnamed = grid(r#"{"grid":[2,2]}"#);
    assert_eq!(unnamed.name().expect("name"), "");
    assert_eq!(unnamed.arrangement().expect("arrangement"), "");
}

#[test]
fn bounding_box() {
    let global = grid(r#"{"grid":[30,30]}"#).bounding_box().expect("bbox");
    assert!(global.is_global().expect("global"));

    let bbox = grid(r#"{"area":[73,-27,33,45],"grid":[4,4]}"#)
        .bounding_box()
        .expect("bbox");
    assert!(!bbox.is_global().expect("global"));
    assert_eq!(
        (bbox.north(), bbox.west(), bbox.south(), bbox.east()),
        (73.0, -27.0, 33.0, 45.0)
    );
}

#[test]
fn space_view_as_grib() {
    // as GRIB space_view (by eccodes), points as the eccodes iterator
    for (spec, first_last) in [
        (
            r#"{"x":[0,44961.0033558134,2997.40022372089],"y":[0,-44961.0033558134,-2997.40022372089],"false":[1067074.48,-467594.435],"projection":{"type":"space_view","h":35747054.5503,"reverse":true}}"#,
            [
                (4.223_693_723, 350.299_524_805),
                (3.815_601_110, 350.719_698_129),
            ],
        ),
        (
            r#"{"x":[0,45009.7522881123,3000.65015254082],"y":[0,-33005.8361442917,-3000.53055857197],"false":[-1632353.683,-1968348.046],"projection":{"type":"space_view","h":35785813.2659,"lon_0":-75,"figure":"wgs84","reverse":true}}"#,
            [
                (18.425_011_761, 300.905_565_350),
                (18.106_822_780, 301.327_135_202),
            ],
        ),
    ] {
        let grid = grid(spec);
        let shape = grid.shape().expect("shape");
        assert!(shape == [16, 16] || shape == [12, 16], "{shape:?}");
        assert_eq!(
            grid.projection().projection_type().expect("type"),
            "space_view"
        );
        assert!(grid.projection().is_reverse().expect("reverse"));

        let (lat, lon) = grid.to_latlons().expect("latlons");
        let n = lat.len();
        for ((lat0, lon0), i) in first_last.into_iter().zip([0, n - 1]) {
            assert!(approx(lat[i], lat0, 1e-6), "lat[{i}] = {}", lat[i]);
            assert!(
                approx(lon_diff(lon[i], lon0), 0.0, 1e-6),
                "lon[{i}] = {}",
                lon[i]
            );
        }
    }
}

#[test]
fn fill_latlons_checks_buffers() {
    let grid = grid(r#"{"grid":"o2"}"#);
    let n = grid.len().expect("len");
    let mut lat = vec![0.0; n - 1];
    let mut lon = vec![0.0; n];
    assert!(grid.fill_latlons(&mut lat, &mut lon).is_err());
    assert!(grid.fill_xyz(&mut lat, &mut lon.clone(), &mut lon).is_err());
}
