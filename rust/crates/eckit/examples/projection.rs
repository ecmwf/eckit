// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

use eckit::Grid;
use eckit::geo::Point;

fn main() -> eckit::Result<()> {
    eckit::init();

    let rotated = Grid::from_spec(r#"{"grid":[5,5],"rotation":[-40,10]}"#)?;

    // [projection]
    let rotation = rotated.projection();
    assert_eq!(rotation.projection_type()?, "rotation");

    let south_pole = rotation.inv(Point::LonLat {
        lon: 0.0,
        lat: -90.0,
    })?;
    let rotated_coords = rotation.fwd_vectors(&[&[0.0, 10.0], &[0.0, 20.0]])?;
    // [end projection]
    println!("{south_pole:?}");
    println!("{rotated_coords:?}");

    // [figure]
    let figure = rotated.figure();
    println!("radius {} m, {}", figure.radius()?, figure.proj_str()?);
    // [end figure]
    Ok(())
}
