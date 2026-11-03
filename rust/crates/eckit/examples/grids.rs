// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

use eckit::Grid;

fn main() -> eckit::Result<()> {
    eckit::init();

    // [grids]
    let grid = Grid::from_name("O1280")?;
    let regular = Grid::from_spec(r#"{"area":[73,-27,33,45],"grid":[4,4]}"#)?;
    let rotated = Grid::from_spec(r#"{"grid":[5,5],"rotation":[-40,10]}"#)?;

    println!("{} with {} points", grid.grid_type()?, grid.len()?);
    println!("{:?}", grid.bounding_box()?);
    println!("{:?} {:?}", regular.shape()?, rotated.shape()?);

    let spec = grid.spec();
    assert_eq!(spec.to_json_str()?, r#"{"grid":"O1280"}"#);
    assert_eq!(spec.get::<String>("grid")?, "O1280");
    // [end grids]

    // [canonical]
    assert_eq!(
        Grid::from_spec("o16")?.spec().to_json_str()?,
        r#"{"grid":"O16"}"#
    );
    assert_eq!(
        Grid::from_name("o16")?,
        Grid::from_spec(r#"{"grid":"O16"}"#)?
    );
    // [end canonical]

    // [latlons]
    let n = grid.len()?;
    let mut lat = vec![0.0; n];
    let mut lon = vec![0.0; n];
    grid.fill_latlons(&mut lat, &mut lon)?;
    // [end latlons]
    println!("first point {} {}", lon[0], lat[0]);
    Ok(())
}
