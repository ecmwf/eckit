// eckit geo C++ bridge for Rust FFI — umbrella header pulled in by the
// cxx-generated geo bridge (`include!("EckitGeoBridge.h")` in geo.rs). Real
// declarations live in the per-topic headers below.
#pragma once

// Kept out of `EckitBridge.h`: that header is exported to downstream `-sys`
// crates via `cargo:cpp_dir`, and must not pull in geo — which is optional and
// only built when the `eckit-geo` feature is on.

#include "GridWrapper.h"
