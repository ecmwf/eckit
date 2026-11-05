// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// [main]
use std::io::{Read, Write};

use eckit::DataHandle;

fn main() -> Result<(), Box<dyn std::error::Error>> {
    eckit::init();

    let path = std::env::temp_dir().join("quickstart.dat");

    let mut output = DataHandle::from_path(&path)?.open_for_write(0)?;
    output.write_all(b"hello from eckit")?;
    output.close()?;

    let (mut input, length) = DataHandle::from_path(&path)?.open_for_read()?;
    println!("handle holds {length} bytes");

    let mut text = String::new();
    input.read_to_string(&mut text)?;
    println!("{text}");
    Ok(())
}
// [end main]
