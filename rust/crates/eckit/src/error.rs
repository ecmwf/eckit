//! Error types for eckit operations.
//!
//! Re-exports the auto-generated `Error` enum from `eckit-sys`.

pub use eckit_sys::Error;

/// Result type alias for eckit operations.
pub type Result<T> = std::result::Result<T, Error>;
