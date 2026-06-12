// eckit C++ bridge for Rust FFI — umbrella header.
//
// Pulled in by the cxx-generated bridge (`include!("eckit_bridge.h")` in
// lib.rs). Real declarations live in the per-topic headers below.
#pragma once

// Note: the auto-generated `rust::behavior::trycatch` lives in
// `eckit_exceptions.h`, which is included by `eckit_bridge.cpp` directly
// (not from this header). Downstream `-sys` crates have their own generated
// `<ns>_exceptions.h` and must not see eckit's transitively through here,
// or they would have two `trycatch` specializations in one translation unit.

#include "config.h"
#include "datahandle.h"
#include "log.h"
#include "message.h"
#include "stream.h"
