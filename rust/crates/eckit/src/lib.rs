//! Safe Rust wrapper for ECMWF's eckit C++ library.
//!
//! Provides:
//! - [`Config`] — YAML configuration loading and access
//! - [`DataHandle`] — abstract I/O (`impl Read + Seek`)
//! - [`Message`] / [`MessageReader`] — GRIB message abstraction
//! - [`Error`] — auto-generated exception types from eckit
//! - [`init`] — runtime initialization with Rust log bridge

pub mod config;
pub mod datahandle;
pub mod error;
pub mod library;
pub mod message;
pub mod stream;

pub use config::{Config, ConfigGet, ConfigSet, SubConfigs};
pub use datahandle::{Closed, DataHandle, HandleState, Reading, Writing};
pub use error::{Error, Result};
pub use library::{Library, LibraryVersion, library_configuration, library_versions, register};
pub use message::{Message, MessageGet, MessageReader};
pub use stream::{MemoryStream, Stream, StreamRead, StreamWrite, TcpStream};

/// Initialize eckit runtime with Rust log bridge.
///
/// Must be called before any eckit API usage. Safe to call multiple times.
///
/// Routes all C++ `eckit::Log` output through Rust's `log` crate.
/// The application chooses the backend (`env_logger`, `tracing-subscriber`, etc.).
pub fn init() {
    static ONCE: std::sync::Once = std::sync::Once::new();
    ONCE.call_once(eckit_sys::init);
}
