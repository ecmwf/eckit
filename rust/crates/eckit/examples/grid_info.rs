//! Print everything `eckit::geo` knows about a grid.
//!
//! ```text
//! cargo run --example grid_info                                          # O16
//! cargo run --example grid_info -- N320
//! cargo run --example grid_info -- H4
//! cargo run --example grid_info -- '{"area":[73,-27,33,45],"grid":[4,4]}'
//! ```
//!
//! An argument starting with `{` is treated as a gridSpec, anything else as a
//! grid name. Fields a grid cannot answer are reported inline rather than
//! aborting, so the same command works across grid types.

use std::process::ExitCode;

use eckit::Grid;
use eckit::geo::{LonLat, cache_footprint, cache_purge};

/// Grid described when the caller passes no argument.
const DEFAULT_GRID: &str = "O16";

/// Leading and trailing entries shown for arrays too long to print whole.
const EDGE: usize = 4;

/// Points listed in the sample section.
const SAMPLE: usize = 4;

/// Column width for field labels.
const LABEL: usize = 18;

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

    section("identity");
    show("spec", grid.spec());
    show("uid", grid.uid());
    show("type", grid.grid_type());
    show("catalog", grid.catalog());

    let n = grid.len()?;

    section("shape");
    show("points", Ok(n));
    show("empty", grid.is_empty().map(yes_no));
    show("shape", grid.shape().map(|s| format!("{s:?}")));
    show("latitude rows", grid.ny());
    show("structured", grid.is_structured().map(yes_no));
    show("pl", grid.pl().map(|pl| pl_summary(&pl)));

    section("geometry");
    show(
        "bounding box",
        grid.bounding_box().map(|b| {
            format!(
                "N {:.4}  W {:.4}  S {:.4}  E {:.4}",
                b.north, b.west, b.south, b.east
            )
        }),
    );
    show("first point", grid.first_point().map(point));
    show("last point", grid.last_point().map(point));
    show(
        "distinct lats",
        grid.distinct_latitudes().map(|v| degrees(&v)),
    );
    // Only regular grids have a single set of longitudes shared by every row.
    // Asking a reduced grid throws `NotImplemented`, and eckit prints a
    // backtrace to stdout as it does so.
    if is_regular(&grid) {
        show(
            "distinct lons",
            grid.distinct_longitudes().map(|v| degrees(&v)),
        );
    } else {
        field("distinct lons", "(regular grids only)");
    }

    // `fill_latlons` over caller-owned buffers is the form to reach for on real
    // grids — O1280 is 6.6M points per array, and these buffers can be reused.
    let buffers = human_bytes(2 * n * size_of::<f64>());
    section(&format!("points — {n} total, {buffers} of buffers"));
    let mut lat = vec![0.0; n];
    let mut lon = vec![0.0; n];
    grid.fill_latlons(&mut lat, &mut lon)?;
    for i in 0..SAMPLE.min(n) {
        println!("  [{i}] lat {:.4}  lon {:.4}", lat[i], lon[i]);
    }

    section("caches");
    field("footprint", &human_bytes(cache_footprint()));
    drop(grid);
    cache_purge();
    field("after purge", &human_bytes(cache_footprint()));

    Ok(())
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

/// Regular grids report `[ny, nx]`; reduced ones report `[size]`.
fn is_regular(grid: &Grid) -> bool {
    grid.shape().is_ok_and(|shape| shape.len() == 2)
}

const fn yes_no(value: bool) -> &'static str {
    if value { "yes" } else { "no" }
}

fn point(p: LonLat) -> String {
    format!("lat {:.4}  lon {:.4}", p.lat, p.lon)
}

fn pl_summary(pl: &[i64]) -> String {
    if pl.is_empty() {
        return "(none — no row structure)".to_string();
    }
    let sum: i64 = pl.iter().sum();
    format!("{}  (sum {sum})", summarize(pl, i64::to_string))
}

fn degrees(values: &[f64]) -> String {
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
