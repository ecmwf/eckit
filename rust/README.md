# eckit Rust bindings

Rust bindings for ECMWF's [eckit](https://github.com/ecmwf/eckit) C++ library.

## Overview

eckit is a cross-platform C++ toolkit that supports the development of tools
and applications at ECMWF. These crates make it usable from Rust: the C++
library is built from source (or located on the system) at build time, and its
configuration, data handle, GRIB message, stream, and logging facilities are
exposed through a safe API.

## Installation

Add the safe wrapper to your `Cargo.toml`:

```toml
[dependencies]
eckit = "2.2"
```

Building requires CMake and a C++17 compiler. To link against an existing
eckit installation instead of building from source, use the `system` feature:

```toml
[dependencies]
eckit = { version = "2.2", default-features = false, features = ["system"] }
```

## Crates

- **eckit**: the safe API. Configuration, data handles, GRIB messages,
  streams, and eckit exceptions surfaced as typed Rust errors.
- **eckit-sys**: the low-level FFI layer using [CXX](https://cxx.rs/). Builds
  the eckit C++ library from source (`vendored`, default) or links against a
  system installation (`system`), and exposes the raw bridge. Its Cargo
  features control which parts of the C++ library are compiled.

## Copyright and License

Copyright 1996- European Centre for Medium-Range Weather Forecasts (ECMWF).

This software is licensed under the terms of the [Apache License, Version 2.0](LICENSE) which can also be obtained at http://www.apache.org/licenses/LICENSE-2.0.

In applying this licence, ECMWF does not waive the privileges and immunities granted to it by virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
