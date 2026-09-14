// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

//! Error types for eckit operations.
//!
//! Re-exports the auto-generated `Error` enum from `eckit-sys`.

pub use eckit_sys::Error;

/// Result type alias for eckit operations.
pub type Result<T> = std::result::Result<T, Error>;
