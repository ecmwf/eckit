// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

use eckit::Grid;
use eckit::geo::BoundingBox;

fn main() -> eckit::Result<()> {
    eckit::init();

    let grid = Grid::from_name("N320")?;

    // [crop]
    let bbox = BoundingBox::new(73.0, -27.0, 33.0, 45.0)?;

    let indices: Vec<usize> = grid.crop(&bbox)?;
    let europe = grid.cropped(&bbox)?;
    // [end crop]

    println!("{} points in the box", indices.len());
    println!("{} with {} points", europe.grid_type()?, europe.len()?);
    println!("contains London: {}", bbox.contains_point(-0.1, 51.5)?);
    println!("{:?}", europe.bounding_box()?);
    Ok(())
}
