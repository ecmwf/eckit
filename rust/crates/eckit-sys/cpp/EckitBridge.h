// eckit C++ bridge for Rust FFI — umbrella header pulled in by the
// cxx-generated bridge (`include!("EckitBridge.h")` in lib.rs) and by
// downstream `-sys` crates. Real declarations live in the per-topic headers
// below.
#pragma once

// Note: the auto-generated `rust::behavior::trycatch` lives in
// `eckit_exceptions.h`, which is included by each per-topic `.cc` directly
// (not from this header). Downstream `-sys` crates have their own generated
// `<ns>_exceptions.h` and must not see eckit's transitively through here,
// or they would have two `trycatch` specializations in one translation unit.

#include "ConfigWrapper.h"
#include "DataHandleWrapper.h"
#include "MessageWrapper.h"
#include "RustLogTarget.h"
#include "StreamWrapper.h"
