// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

//! Shared setup of the geo tests.

use std::sync::OnceLock;

use tempfile::TempDir;

static CACHE: OnceLock<TempDir> = OnceLock::new();

/// Initialise eckit with geo caching into a temporary directory, as the
/// Python tests do (see `python/eckit/tests/conftest.py`).
///
/// Unstructured grids and polygon areas keep their coordinates in eckit geo's
/// cache, so they need it enabled; pointing it at a temporary directory keeps
/// the tests out of the runtime cache. The cached records are compressed with
/// LZ4 by default, which the default build of eckit leaves out, so they are
/// stored uncompressed. The resources are resolved on first use, so this runs
/// before any eckit call.
pub fn init() {
    CACHE.get_or_init(|| {
        let dir = tempfile::Builder::new()
            .prefix("eckit_geo_cache.")
            .tempdir()
            .expect("temporary cache directory");
        // SAFETY: no other thread reads the environment during the call: every
        // test thread reaches eckit only through this `OnceLock`, which blocks
        // the others until the variables are set and eckit is initialised.
        unsafe {
            std::env::set_var("ECKIT_GEO_CACHING", "true");
            std::env::set_var("ECKIT_GEO_CACHE_PATH", dir.path());
            std::env::set_var("ECKIT_CODEC_COMPRESSION", "");
        }
        eckit::init();
        dir
    });
}
