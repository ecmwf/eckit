<!--
SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
SPDX-License-Identifier: Apache-2.0
-->

# eckit

Safe Rust wrapper for ECMWF's [eckit](https://github.com/ecmwf/eckit) C++ library.

## Overview

This crate provides a safe API on top of the low-level
[`eckit-sys`](https://crates.io/crates/eckit-sys) bindings:

- `Config` - YAML configuration loading and typed key access
- `DataHandle` - abstract I/O over files, buffers, multi-part and tee handles,
  with typestate open-for-read/open-for-write (`impl Read + Seek` / `impl Write`)
- `Message` / `MessageReader` - GRIB message abstraction
- `Stream` - eckit stream protocol (memory, TCP)
- `geo::Grid` - geospatial grids by gridSpec (`geo` feature): points, shape,
  scanning order, with `geo::Area` / `geo::BoundingBox` to crop them,
  `geo::Projection` / `geo::Figure` for their coordinates and
  `geo::Search` for nearest-neighbour queries
- `Error` - eckit C++ exceptions surfaced as typed Rust errors
- `init` - runtime initialization routing `eckit::Log` through the Rust
  [`log`](https://crates.io/crates/log) crate

## Installation

Add the crate to your `Cargo.toml`:

```toml
[dependencies]
eckit = "2.2"
```

The default `vendored` feature builds the eckit C++ library from source, which
requires CMake and a C++17 compiler.

## Usage

```rust,no_run
use std::io::Read;

fn main() -> Result<(), Box<dyn std::error::Error>> {
    eckit::init();

    let handle = eckit::DataHandle::from_path("data.grib")?;
    let (mut reading, length) = handle.open_for_read()?;
    println!("handle holds {length} bytes");

    let mut buf = Vec::new();
    reading.read_to_end(&mut buf)?;
    Ok(())
}
```

### Grids

```rust,no_run
use eckit::geo::{BoundingBox, Point};

fn main() -> eckit::Result<()> {
    eckit::init();

    let grid = eckit::Grid::from_name("O1280")?;
    println!("{} points, spec {:?}", grid.len()?, grid.spec());

    // nearest grid point to London, through the grid's k-d tree
    let near = grid.search()?.nearest(Point::LonLat { lon: -0.1, lat: 51.5 })?;
    let (lat, lon) = grid.to_latlons()?;
    println!("point {} at ({}, {}), {:.0} m away", near.index, lon[near.index], lat[near.index], near.distance);

    // the grid cropped to Europe, as a reduced Gaussian grid of its points there
    let europe = BoundingBox::new(73.0, -27.0, 33.0, 45.0)?;
    let cropped = grid.cropped(&europe)?;
    println!("{} points in {:?}", cropped.len()?, europe);
    Ok(())
}
```

Unstructured grids (from coordinates, or cropping projected grids) keep their
points in eckit geo's cache, enabled with `ECKIT_GEO_CACHING=true` and located
by `ECKIT_GEO_CACHE_PATH` (default `~/.cache/eckit/geo`); see the `geo` module
documentation.

## Cargo build features

- `vendored` (default) - Build the eckit C++ library from source
  (forwards `eckit-sys/vendored`).
- `system` - Link against a system-installed eckit
  (forwards `eckit-sys/system`).
- `geo` (default) - Geospatial grids, `eckit::geo`
  (forwards `eckit-sys/eckit-geo`).

See the [`eckit-sys` README](https://crates.io/crates/eckit-sys) for the full
set of underlying C++ build features and environment variables.

## Copyright and License

Copyright 1996- European Centre for Medium-Range Weather Forecasts (ECMWF).

This software is licensed under the terms of the [Apache License, Version 2.0](LICENSE) which can also be obtained at http://www.apache.org/licenses/LICENSE-2.0.

In applying this licence, ECMWF does not waive the privileges and immunities granted to it by virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
