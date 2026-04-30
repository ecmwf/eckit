# eckit-sys

Low-level Rust bindings to ECMWF's [eckit](https://github.com/ecmwf/eckit) C++
library.

This crate provides raw FFI bindings using [cxx](https://cxx.rs/). For a safe,
ergonomic API, use the higher-level `eckit` crate (forthcoming).

## Features

### Build strategy (mutually exclusive)

- `vendored` (default) - Clone and build eckit (and ecbuild) from source.
- `system` - Link against a system-installed eckit, located via CMake
  `find_package(eckit)`. Honours `ECKIT_DIR` and `CMAKE_PREFIX_PATH`.

### Core eckit libraries (enabled by default)

- `eckit-codec` - `eckit::codec` encoding/decoding library (implies `eckit-spec`).
- `eckit-spec` - `eckit::spec` metadata library.
- `eckit-geo` - `eckit::geo` geometry library.
- `unicode` - Unicode support.
- `aio` - Async I/O support.

### Optional eckit libraries

- `eckit-sql` - `eckit::sql` SQL engine (required by `odc`).

### MPI

- `mpi` - Use system MPI libraries.

### Compression codecs (off by default; require external libraries)

- `bzip2`, `snappy`, `lz4`, `aec`, `zip`

### Hashing (off by default)

- `xxhash`

### Linear algebra (off by default; require external libraries)

- `eigen`, `lapack`, `mkl`, `omp`

### Networking (off by default; require external libraries)

- `curl` - libcurl support for `eckit::URLHandle`.
- `ssl` - OpenSSL support.

### GPU (off by default)

- `cuda`, `hip`

### Geo sub-features

- `geo-codec-grids` - ORCA/FESOM/ICON grid types (implies `eckit-codec`,
  `eckit-geo`, `lz4`).
- `geo-caching` - Default eckit::geo caching behaviour.
- `geo-bitreproducible` - Bit-reproducibility tests.
- `geo-projection-proj-default` - Default to PROJ-based projections.
- `geo-area-shapefile` - Shapefile support.

### Other (off by default)

- `proj` - PROJ-based projection support.
- `rados` - Ceph/RADOS storage support.
- `jemalloc` - Link against jemalloc.
- `rsync` - librsync.
- `convex-hull` - `eckit::maths` convex hull / Delaunay triangulation.
- `experimental` - Experimental upstream features.
- `sandbox` - Sandbox builds.

## Environment variables

- `ECKIT_DIR` - Install prefix of an eckit build, used by `system` mode.
- `CMAKE_PREFIX_PATH` - Additional CMake search paths.
- `DOCS_RS` - When set, the build script becomes a no-op (for docs.rs).

## License

Apache-2.0
