// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

use eckit::Grid;
use eckit::geo::Point;

fn main() -> eckit::Result<()> {
    eckit::init();

    let grid = Grid::from_name("N320")?;

    // [search]
    let search = grid.search()?;
    let london = Point::LonLat {
        lon: -0.1,
        lat: 51.5,
    };

    let nearest = search.nearest(london)?;
    let four = search.nearest_k(london, 4)?;
    let within = search.within_radius(london, 50_000.0)?;
    // [end search]

    println!(
        "nearest point {} at {:.0} m",
        nearest.index, nearest.distance
    );
    println!(
        "{} within 50 km, four nearest: {:?}",
        within.len(),
        four.iter().map(|n| n.index).collect::<Vec<_>>()
    );
    Ok(())
}
