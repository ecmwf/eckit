// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

use eckit::{DataHandle, MessageReader};

fn main() -> eckit::Result<()> {
    eckit::init();

    let path = std::env::args()
        .nth(1)
        .unwrap_or_else(|| "fields.grib".to_string());

    // [read]
    let (mut handle, _) = DataHandle::from_path(&path)?.open_for_read()?;

    for msg in MessageReader::new(&mut handle)? {
        let msg = msg?;
        let param: String = msg.get("shortName")?;
        println!("{param}: {} bytes at offset {}", msg.length(), msg.offset());
    }
    // [end read]
    Ok(())
}
