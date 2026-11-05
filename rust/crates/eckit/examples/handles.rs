// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

use std::io::{Read, Write};

use eckit::DataHandle;

fn main() -> Result<(), Box<dyn std::error::Error>> {
    eckit::init();

    let dir = std::env::temp_dir();
    let input_path = dir.join("handles-in.dat");
    let output_path = dir.join("handles-out.dat");
    std::fs::write(&input_path, b"some bytes to copy")?;

    // [copy]
    let (mut input, length) = DataHandle::from_path(&input_path)?.open_for_read()?;
    let mut output = DataHandle::from_path(&output_path)?.open_for_write(length)?;

    input.save_into(&mut output)?;
    output.close()?;
    // [end copy]

    // [read]
    let (mut handle, _) = DataHandle::from_buffer(b"held in memory")?.open_for_read()?;
    let mut text = String::new();
    handle.read_to_string(&mut text)?;
    // [end read]
    println!("{text}");

    // [writer]
    let mut handle = DataHandle::from_writer(std::io::stdout())?.open_for_write(0)?;
    handle.write_all(b"written by eckit into a Rust sink\n")?;
    handle.close()?;
    // [end writer]
    Ok(())
}
