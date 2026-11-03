// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

use std::io::Read;

use eckit::{DataHandle, Error};

fn main() -> eckit::Result<()> {
    eckit::init();

    // [match]
    match DataHandle::from_path("/no/such/field.grib")?.open_for_read() {
        Ok((_handle, length)) => println!("{length} bytes"),
        Err(Error::CantOpenFile(msg)) => eprintln!("missing input: {msg}"),
        Err(other) => return Err(other),
    }
    // [end match]

    let path = std::env::temp_dir().join("short.dat");
    std::fs::write(&path, b"short").map_err(|e| Error::Other(e.to_string()))?;

    // [io]
    let (mut handle, _) = DataHandle::from_part(&path, 0, 100)?.open_for_read()?;
    let mut buf = [0u8; 100];
    if let Err(err) = handle.read_exact(&mut buf) {
        println!("{:?}: {err}", err.kind());
    }
    // [end io]
    Ok(())
}
