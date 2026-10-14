// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

//! Print everything `eckit::geo` knows about a grid.
//!
//! ```text
//! cargo run --example grid_info                                          # O16
//! cargo run --example grid_info -- N320
//! cargo run --example grid_info -- H4
//! cargo run --example grid_info -- '{"area":[73,-27,33,45],"grid":[4,4]}'
//! cargo run --example grid_info -- '{"grid":[5,5],"rotation":[-40,10]}'
//! ```
//!
//! An argument starting with `{` is treated as a gridSpec, anything else as a
//! grid name. Fields a grid cannot answer are reported inline rather than
//! aborting, so the same command works across grid types.

use std::process::ExitCode;

use eckit::Grid;
use eckit::geo::{BoundingBox, Point, cache_footprint, cache_purge, search_cache_statistics};

/// Grid described when the caller passes no argument.
const DEFAULT_GRID: &str = "O16";

/// Leading and trailing entries shown for arrays too long to print whole.
const EDGE: usize = 4;

/// Points listed in the sample section.
const SAMPLE: usize = 4;

/// Column width for field labels.
const LABEL: usize = 18;

/// Location searched for in the grid: London.
const QUERY_LON: f64 = -0.1;
const QUERY_LAT: f64 = 51.5;

fn main() -> ExitCode {
    // Reporting the error through `Display` rather than returning it from
    // `main`, which would print the `Debug` form.
    if let Err(err) = run() {
        eprintln!("grid_info: {err}");
        return ExitCode::FAILURE;
    }
    ExitCode::SUCCESS
}

fn run() -> Result<(), eckit::Error> {
    eckit::init();

    let arg = std::env::args().nth(1);
    let input = arg.as_deref().unwrap_or(DEFAULT_GRID);

    let grid = if input.trim_start().starts_with('{') {
        Grid::from_spec(input)?
    } else {
        Grid::from_name(input)?
    };

    println!("grid_info: {input}");

    identity(&grid);
    let n = shape(&grid)?;
    geometry(&grid);
    projection_and_figure(&grid);
    let (lat, lon) = points(&grid, n)?;
    search(&grid, &lat, &lon);
    crop(&grid)?;
    caches(grid);

    Ok(())
}

fn identity(grid: &Grid) {
    section("identity");
    let spec = grid.spec();
    show("spec", spec.to_json_str());
    // Typed access: a named grid carries its name under "grid"; a lon/lat grid
    // built from increments carries them under "grid", with its "area".
    if spec.has("area") {
        show(
            "spec.area",
            spec.get::<Vec<f64>>("area").map(|v| format!("{v:?}")),
        );
        show(
            "spec.grid",
            spec.get::<Vec<f64>>("grid").map(|v| format!("{v:?}")),
        );
    } else if spec.has("grid") {
        show("spec.grid", spec.get::<String>("grid"));
    }
    show("uid", grid.uid());
    show("type", grid.grid_type());
    // The catalog entry is the grid's own spec, except for grids shipped as
    // data files (ORCA, FESOM, ICON), whose entry carries the download URL.
    match grid.catalog() {
        Ok(catalog) => {
            show("catalog", catalog.to_json_str());
            if catalog.has("url") {
                show("catalog.url", catalog.get::<String>("url"));
            }
        }
        Err(err) => show("catalog", Err::<String, _>(err)),
    }
}

/// Returns the number of points.
fn shape(grid: &Grid) -> Result<usize, eckit::Error> {
    let n = grid.len()?;

    section("shape");
    show("points", Ok(n));
    show("empty", grid.is_empty().map(yes_no));
    show("shape", grid.shape().map(|s| format!("{s:?}")));
    show("regular", grid.is_regular().map(yes_no));
    show("latitude rows", grid.ny());
    show("structured", grid.is_structured().map(yes_no));
    show("pl", grid.pl().map(|pl| pl_summary(&pl)));
    show("order", grid.order());

    Ok(n)
}

fn geometry(grid: &Grid) {
    section("geometry");
    show(
        "coordinates",
        grid.point_coordinates().map(|c| c.join(", ")),
    );
    show("bounding box", grid.bounding_box().map(|b| bbox(&b)));
    show("first point", grid.first_point().map(point));
    show("last point", grid.last_point().map(point));
    // Empty when the rows (columns) share no latitudes (longitudes), as for
    // reduced grids' longitudes: only regular grids have a single set.
    show(
        "distinct lats",
        grid.distinct_latitudes().map(|v| degrees(&v)),
    );
    show(
        "distinct lons",
        grid.distinct_longitudes().map(|v| degrees(&v)),
    );
}

fn projection_and_figure(grid: &Grid) {
    section("projection and figure");
    let projection = grid.projection();
    show("type", projection.projection_type());
    show("spec", projection.spec().to_json_str());
    show("proj", projection.proj_str());
    show(
        "coordinates",
        projection.source_point_coordinates().and_then(|s| {
            Ok(format!(
                "{} -> {}",
                s.join(", "),
                projection.target_point_coordinates()?.join(", ")
            ))
        }),
    );
    let figure = grid.figure();
    show("figure", figure.spec().to_json_str());
    show("spherical", figure.is_spherical().map(yes_no));
    show(
        "semi-axes",
        figure
            .a()
            .and_then(|a| Ok(format!("a {a}  b {}", figure.b()?))),
    );
}

/// Every point's latitude and longitude, printing a sample.
///
/// `fill_latlons` over caller-owned buffers is the form to reach for on real
/// grids: O1280 is 6.6M points per array, and these buffers can be reused.
fn points(grid: &Grid, n: usize) -> Result<(Vec<f64>, Vec<f64>), eckit::Error> {
    let buffers = human_bytes(2 * n * size_of::<f64>());
    section(&format!("points: {n} total, {buffers} of buffers"));
    let mut lat = vec![0.0; n];
    let mut lon = vec![0.0; n];
    grid.fill_latlons(&mut lat, &mut lon)?;
    for i in 0..SAMPLE.min(n) {
        println!("  [{i}] lat {:.4}  lon {:.4}", lat[i], lon[i]);
    }

    Ok((lat, lon))
}

/// Nearest grid points to a location, through the grid's k-d tree.
fn search(grid: &Grid, lat: &[f64], lon: &[f64]) {
    section("search");
    let query = Point::LonLat {
        lon: QUERY_LON,
        lat: QUERY_LAT,
    };
    field("query", &format!("lat {QUERY_LAT:.4}  lon {QUERY_LON:.4}"));
    match grid.search() {
        Ok(search) => {
            show(
                "nearest",
                search.nearest(query).map(|near| {
                    format!(
                        "[{}] lat {:.4}  lon {:.4}  {:.1} km away",
                        near.index,
                        lat[near.index],
                        lon[near.index],
                        near.distance / 1000.0
                    )
                }),
            );
            show(
                "4 nearest",
                search.nearest_k(query, 4).map(|near| {
                    near.iter()
                        .map(|n| format!("[{}] {:.1} km", n.index, n.distance / 1000.0))
                        .collect::<Vec<_>>()
                        .join(", ")
                }),
            );
            show(
                "k-d tree",
                search.footprint().map(|f| human_bytes(f.memory)),
            );
        }
        Err(err) => show("nearest", Err::<String, _>(err)),
    }
}

/// Points in an area, and the grid cropped to it: of the same type when
/// supported (a lon/lat grid by a bounding box), otherwise unstructured.
fn crop(grid: &Grid) -> Result<(), eckit::Error> {
    section("crop");
    let box_ = BoundingBox::new(60.0, -10.0, 35.0, 30.0)?;
    field("area", &bbox(&box_));
    show("points in area", grid.crop(&box_).map(|i| i.len()));
    show(
        "cropped grid",
        grid.cropped(&box_)
            .and_then(|c| Ok(format!("{} ({} points)", c.grid_type()?, c.len()?))),
    );

    Ok(())
}

/// Takes the grid, to show the caches it leaves behind once dropped.
fn caches(grid: Grid) {
    section("caches");
    show(
        "searches",
        search_cache_statistics().map(|s| {
            format!(
                "{} cached, {} hits, {} misses, {}",
                s.size,
                s.hits,
                s.misses,
                human_bytes(s.usage.memory)
            )
        }),
    );
    field("footprint", &human_bytes(cache_footprint()));
    drop(grid);
    cache_purge();
    field("after purge", &human_bytes(cache_footprint()));
}

fn section(title: &str) {
    println!("\n{title}");
}

fn field(label: &str, value: &str) {
    println!("  {label:<LABEL$} {value}");
}

/// Print a field, reporting the error inline if the grid cannot answer.
fn show(label: &str, value: eckit::Result<impl std::fmt::Display>) {
    match value {
        Ok(value) => field(label, &value.to_string()),
        Err(err) => field(label, &format!("<unavailable: {err}>")),
    }
}

const fn yes_no(value: bool) -> &'static str {
    if value { "yes" } else { "no" }
}

fn bbox(b: &BoundingBox) -> String {
    format!(
        "N {:.4}  W {:.4}  S {:.4}  E {:.4}",
        b.north(),
        b.west(),
        b.south(),
        b.east()
    )
}

fn point(p: Point) -> String {
    match p {
        Point::Xy { x, y } => format!("x {x:.4}  y {y:.4}"),
        Point::Xyz { x, y, z } => format!("x {x:.4}  y {y:.4}  z {z:.4}"),
        Point::LonLat { lon, lat } => format!("lat {lat:.4}  lon {lon:.4}"),
        Point::LonLatR { lon, lat } => format!("lat {lat:.4} rad  lon {lon:.4} rad"),
    }
}

fn pl_summary(pl: &[i64]) -> String {
    if pl.is_empty() {
        return "(none: no row structure)".to_string();
    }
    let sum: i64 = pl.iter().sum();
    format!("{}  (sum {sum})", summarize(pl, i64::to_string))
}

fn degrees(values: &[f64]) -> String {
    if values.is_empty() {
        return "(none shared)".to_string();
    }
    format!(
        "{} values: {}",
        values.len(),
        summarize(values, |v| format!("{v:.3}"))
    )
}

/// Render a slice, eliding the middle when it is longer than `2 * EDGE`.
fn summarize<T>(values: &[T], render: impl Fn(&T) -> String) -> String {
    let join = |slice: &[T]| slice.iter().map(&render).collect::<Vec<_>>().join(", ");
    if values.len() <= EDGE * 2 {
        return join(values);
    }
    format!(
        "{}, …, {}",
        join(&values[..EDGE]),
        join(&values[values.len() - EDGE..])
    )
}

fn human_bytes(bytes: usize) -> String {
    const KIB: usize = 1024;
    const MIB: usize = KIB * KIB;
    const GIB: usize = MIB * KIB;
    match bytes {
        b if b < KIB => format!("{b} B"),
        b if b < MIB => format!("{}.{} KiB", b / KIB, b % KIB * 10 / KIB),
        b if b < GIB => format!("{}.{} MiB", b / MIB, b % MIB * 10 / MIB),
        b => format!("{}.{} GiB", b / GIB, b % GIB * 10 / GIB),
    }
}
