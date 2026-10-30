// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

//! Searching grid points: (lon, lat) in degrees, distances in metres. Mirrors
//! `python/eckit/tests/geo/test_search.py` and the search tests of
//! `test_grid.py`.

mod common;

use common::{grid, lonlat};
use eckit::geo::{Neighbour, Point, search_cache_clear, search_cache_statistics};

fn indices(neighbours: &[Neighbour]) -> Vec<usize> {
    let mut indices: Vec<usize> = neighbours.iter().map(|n| n.index).collect();
    indices.sort_unstable();
    indices
}

#[test]
fn nearest_grid_point() {
    let grid = grid(r#"{"grid":[1,1]}"#);
    let (lat, lon) = grid.to_latlons().expect("latlons");

    let near = grid
        .search()
        .expect("search")
        .nearest(lonlat(-0.2, 51.6))
        .expect("nearest");
    assert_eq!((lon[near.index], lat[near.index]), (0.0, 52.0));
    assert!(near.distance < 50e3);
    assert!(matches!(near.point, Point::Xyz { .. }));
}

#[test]
fn inverse_distance_weighting() {
    let grid = grid(r#"{"grid":[1,1]}"#);
    let (field, _) = grid.to_latlons().expect("latlons"); // a field of latitudes

    let neighbours = grid
        .search()
        .expect("search")
        .nearest_k(lonlat(10.3, 45.4), 4)
        .expect("knn");
    let weights: Vec<f64> = neighbours.iter().map(|n| 1.0 / n.distance).collect();
    let value = neighbours
        .iter()
        .zip(&weights)
        .map(|(n, w)| w * field[n.index])
        .sum::<f64>()
        / weights.iter().sum::<f64>();

    assert!(45.0 < value && value < 46.0, "{value}");
}

#[test]
fn points_within_radius() {
    let grid = grid(r#"{"grid":[1,1]}"#);

    // (0, 0) and its 4 neighbours, 1 degree (~111 km) away
    let within = grid
        .search()
        .expect("search")
        .within_radius(lonlat(0.0, 0.0), 120e3)
        .expect("radius");
    assert_eq!(within.len(), 5);
    assert!(within.windows(2).all(|w| w[0].distance <= w[1].distance));
}

#[test]
fn configured_search() {
    let grid = grid(r#"{"grid":[1,1]}"#);

    // 4 nearest, but only within 120 km
    let search = grid
        .search_with_spec(r#"{"search":"knn_and_radius","search-k":4,"search-radius":120e3}"#)
        .expect("search");
    assert_eq!(search.search(lonlat(0.5, 0.0)).expect("search").len(), 2);

    // k is required
    assert!(grid.search_with_spec(r#"{"search":"knn"}"#).is_err());
}

#[test]
fn search_modes() {
    let grid = grid(r#"{"grid":[30,30]}"#);
    let search = grid.search().expect("search");
    assert_eq!(search.len().expect("len"), grid.len().expect("len"));
    assert!(!search.is_empty().expect("empty"));

    // rows of 12 points from the North Pole: (lon, lat) = (0, 0) is point 36
    let near = search.nearest(lonlat(0.0, 0.0)).expect("nn");
    assert_eq!(near.index, 36);
    assert!(near.distance.abs() < 1e-6);
    assert_eq!(
        search.search(lonlat(0.0, 0.0)).expect("configured"),
        vec![near]
    );

    // equidistant to (0, 0) and (30, 0), in metres (chord on the Earth)
    let knn = search.nearest_k(lonlat(15.0, 0.0), 2).expect("knn");
    assert_eq!(indices(&knn), [36, 37]);
    assert!((knn[0].distance - knn[1].distance).abs() < 1e-6);
    assert!((knn[0].distance - 1.663e6).abs() < 1e-3 * 1.663e6);

    let radius = knn[0].distance * 1.001;
    assert_eq!(
        indices(
            &search
                .within_radius(lonlat(15.0, 0.0), radius)
                .expect("radius")
        ),
        [36, 37]
    );
    assert_eq!(
        indices(
            &search
                .nearest_k_and_within_radius(lonlat(15.0, 0.0), 4, radius)
                .expect("knn and radius")
        ),
        [36, 37]
    );
    assert_eq!(
        search
            .nearest_k_or_within_radius(lonlat(15.0, 0.0), 4, 1.0)
            .expect("knn or radius")
            .len(),
        4
    );
}

#[test]
fn search_spec_sets_the_behaviour() {
    let grid = grid(r#"{"grid":[30,30]}"#);

    let search = grid
        .search_with_spec(r#"{"search":"knn","search-k":3}"#)
        .expect("search");
    assert_eq!(
        search.search(lonlat(15.0, 0.0)).expect("configured"),
        search.nearest_k(lonlat(15.0, 0.0), 3).expect("knn")
    );
}

#[test]
fn geocentric_queries() {
    let grid = grid(r#"{"grid":[30,30]}"#);
    let (x, y, z) = grid.to_xyz().expect("xyz");
    let search = grid.search().expect("search");

    let i = 17;
    let point = Point::Xyz {
        x: x[i],
        y: y[i],
        z: z[i],
    };
    let near = search.nearest(point).expect("nn");
    assert_eq!(near.index, i);
    assert!(near.distance.abs() < 1e-6);
    assert_eq!(near.point, point);

    // only (lon, lat) and (x, y, z) queries are supported
    assert!(search.nearest(Point::Xy { x: 0.0, y: 0.0 }).is_err());
}

#[test]
fn search_cache_accounts_for_loaded_searches() {
    let grid = grid(r#"{"grid":[7,7]}"#);

    search_cache_clear().expect("clear");
    let before = search_cache_statistics().expect("statistics");

    let search = grid
        .search()
        .expect("search");
    let footprint = search.footprint().expect("footprint");
    assert!(footprint.memory + footprint.shared > 0);

    // the cache accounts for the loaded search (other tests may add to it concurrently)
    let after = search_cache_statistics().expect("statistics");
    assert!(after.misses > before.misses);
    assert!(after.size >= 1);
    assert!(after.usage.memory >= footprint.memory && after.usage.shared >= footprint.shared);

    // a second search of the same configuration is a hit
    let again = grid
        .search()
        .expect("search");
    assert!(search_cache_statistics().expect("statistics").hits > after.hits);
    assert_eq!(again.footprint().expect("footprint"), footprint);
}

#[test]
fn search_debug_names_the_kdtree() {
    let search = grid(r#"{"grid":[30,30]}"#).search().expect("search");
    assert!(format!("{search:?}").starts_with("Search["));
}
