//! FFI bindings to ECMWF eckit C++ library.
//!
//! This crate builds eckit and provides:
//! - Auto-generated exception types from `eckit/exception/Exceptions.h`
//! - Logging bridge (C++ `eckit::Log` → Rust `log` crate)
//! - Build paths for downstream -sys crates

use bindman::track_cpp_api;

// Auto-generated exception Error enum + From<cxx::Exception> impl
include!(concat!(env!("OUT_DIR"), "/eckit_exceptions.rs"));

#[track_cpp_api(
    ("eckit/config/LocalConfiguration.h", class = "LocalConfiguration"),
    ("eckit/io/DataHandle.h", class = "DataHandle"),
    ("eckit/message/Message.h", class = "Message"),
    ("eckit/message/Reader.h", class = "Reader"),
)]
#[cxx::bridge(namespace = "eckit_bridge")]
mod ffi {
    /// Log levels shared between C++ and Rust.
    /// Compile-time guarantee that both sides match.
    #[derive(Debug, Clone, Copy, PartialEq, Eq)]
    enum LogLevel {
        Error = 1,
        Warn = 2,
        Info = 3,
        Debug = 4,
        Trace = 5,
    }

    /// One ECMWF library reported by `eckit::system::LibraryManager`.
    #[derive(Debug, Clone)]
    struct LibraryVersion {
        name: String,
        version: String,
        gitsha1: String,
        home: String,
    }

    unsafe extern "C++" {
        include!("eckit_bridge.h");

        /// Initialize eckit runtime with Rust log bridge.
        /// Safe to call multiple times — only the first call has effect.
        fn init();

        // ==================== Configuration ====================

        type ConfigWrapper;

        /// Create empty configuration.
        #[rust_name = "config_create"]
        #[must_use]
        fn create() -> UniquePtr<ConfigWrapper>;

        /// Load from YAML file path.
        #[rust_name = "config_from_path"]
        fn from_path(path: &str) -> Result<UniquePtr<ConfigWrapper>>;

        /// Parse from YAML string.
        #[rust_name = "config_from_yaml"]
        fn from_yaml(yaml: &str) -> Result<UniquePtr<ConfigWrapper>>;

        /// Copy a configuration.
        #[rust_name = "config_clone"]
        fn clone(src: &ConfigWrapper) -> UniquePtr<ConfigWrapper>;

        // Read
        fn has(self: &ConfigWrapper, key: &str) -> bool;
        fn is_list(self: &ConfigWrapper, key: &str) -> bool;
        fn is_empty(self: &ConfigWrapper) -> bool;
        fn get_string(self: &ConfigWrapper, key: &str, default_val: &str) -> Result<String>;
        fn get_long(self: &ConfigWrapper, key: &str, default_val: i64) -> Result<i64>;
        fn get_int(self: &ConfigWrapper, key: &str, default_val: i32) -> Result<i32>;
        fn get_bool(self: &ConfigWrapper, key: &str, default_val: bool) -> Result<bool>;
        fn get_double(self: &ConfigWrapper, key: &str, default_val: f64) -> Result<f64>;
        fn get_string_vector(
            self: &ConfigWrapper,
            key: &str,
            default_val: &Vec<String>,
        ) -> Result<Vec<String>>;

        // Sub-configurations (by key)
        fn get_sub(self: &ConfigWrapper, key: &str) -> Result<UniquePtr<ConfigWrapper>>;
        fn sub_count(self: &ConfigWrapper, key: &str) -> Result<usize>;
        fn sub_at(
            self: &ConfigWrapper,
            key: &str,
            index: usize,
        ) -> Result<UniquePtr<ConfigWrapper>>;

        // Sub-configurations (root-level list — no-arg getSubConfigurations())
        fn root_sub_count(self: &ConfigWrapper) -> Result<usize>;
        fn root_sub_at(self: &ConfigWrapper, index: usize) -> Result<UniquePtr<ConfigWrapper>>;

        // Write
        fn set_string(self: Pin<&mut ConfigWrapper>, key: &str, value: &str);
        fn set_long(self: Pin<&mut ConfigWrapper>, key: &str, value: i64);
        fn set_int(self: Pin<&mut ConfigWrapper>, key: &str, value: i32);
        fn set_bool(self: Pin<&mut ConfigWrapper>, key: &str, value: bool);
        fn set_double(self: Pin<&mut ConfigWrapper>, key: &str, value: f64);
        fn remove(self: Pin<&mut ConfigWrapper>, key: &str);

        // ==================== DataHandle ====================

        type DataHandleWrapper;

        fn open_for_read(self: Pin<&mut DataHandleWrapper>) -> Result<i64>;
        fn open_for_write(self: Pin<&mut DataHandleWrapper>, estimated_length: i64) -> Result<()>;
        fn read(self: Pin<&mut DataHandleWrapper>, buf: &mut [u8]) -> Result<i64>;
        fn write(self: Pin<&mut DataHandleWrapper>, buf: &[u8]) -> Result<i64>;
        fn close(self: Pin<&mut DataHandleWrapper>) -> Result<()>;
        fn position(self: &DataHandleWrapper) -> Result<i64>;
        fn seek(self: Pin<&mut DataHandleWrapper>, offset: i64) -> Result<i64>;
        fn can_seek(self: &DataHandleWrapper) -> bool;
        fn estimate(self: &DataHandleWrapper) -> Result<i64>;

        /// Copy all data from this handle into another (both must be open).
        fn save_into(
            self: Pin<&mut DataHandleWrapper>,
            target: Pin<&mut DataHandleWrapper>,
        ) -> Result<i64>;

        /// Open a file as a DataHandle for reading.
        fn data_handle_from_file(path: &str) -> Result<UniquePtr<DataHandleWrapper>>;

        /// Open a byte range of a file as a DataHandle.
        fn data_handle_from_part(
            path: &str,
            offset: i64,
            length: i64,
        ) -> Result<UniquePtr<DataHandleWrapper>>;

        /// Create a DataHandle from an in-memory buffer (copies the data).
        fn data_handle_from_buffer(data: &[u8]) -> Result<UniquePtr<DataHandleWrapper>>;

        /// Create a MultiHandle from multiple file paths.
        fn data_handle_from_multi(paths: &[String]) -> Result<UniquePtr<DataHandleWrapper>>;

        /// Create a TeeHandle from multiple file paths — writes all targets in parallel.
        fn data_handle_tee(paths: &[String]) -> Result<UniquePtr<DataHandleWrapper>>;

        // ==================== Message + Reader ====================

        type MessageWrapper;

        fn is_valid(self: &MessageWrapper) -> bool;
        fn length(self: &MessageWrapper) -> usize;
        /// Byte offset of this message within the source data handle, as
        /// reported by `eckit::message::Reader`.
        fn offset(self: &MessageWrapper) -> i64;
        fn get_string(self: &MessageWrapper, key: &str) -> Result<String>;
        fn get_long(self: &MessageWrapper, key: &str) -> Result<i64>;
        fn get_double(self: &MessageWrapper, key: &str) -> Result<f64>;
        fn data(self: &MessageWrapper) -> &[u8];
        fn write_to(self: &MessageWrapper, handle: Pin<&mut DataHandleWrapper>) -> Result<()>;
        #[rust_name = "clone_message"]
        fn clone(self: &MessageWrapper) -> Result<UniquePtr<MessageWrapper>>;

        type ReaderWrapper;

        fn new_reader(handle: Pin<&mut DataHandleWrapper>) -> Result<UniquePtr<ReaderWrapper>>;
        fn next(self: Pin<&mut ReaderWrapper>) -> Result<UniquePtr<MessageWrapper>>;

        // ==================== Stream ====================

        type StreamWrapper;

        // Write
        fn write_char(self: Pin<&mut StreamWrapper>, c: u8) -> Result<()>;
        fn write_bool(self: Pin<&mut StreamWrapper>, v: bool) -> Result<()>;
        fn write_int(self: Pin<&mut StreamWrapper>, v: i32) -> Result<()>;
        fn write_long(self: Pin<&mut StreamWrapper>, v: i64) -> Result<()>;
        fn write_unsigned_long(self: Pin<&mut StreamWrapper>, v: u64) -> Result<()>;
        fn write_double(self: Pin<&mut StreamWrapper>, v: f64) -> Result<()>;
        fn write_string(self: Pin<&mut StreamWrapper>, v: &str) -> Result<()>;
        fn write_blob(self: Pin<&mut StreamWrapper>, data: &[u8]) -> Result<()>;

        // Read
        fn read_char(self: Pin<&mut StreamWrapper>) -> Result<u8>;
        fn read_bool(self: Pin<&mut StreamWrapper>) -> Result<bool>;
        fn read_int(self: Pin<&mut StreamWrapper>) -> Result<i32>;
        fn read_long(self: Pin<&mut StreamWrapper>) -> Result<i64>;
        fn read_unsigned_long(self: Pin<&mut StreamWrapper>) -> Result<u64>;
        fn read_double(self: Pin<&mut StreamWrapper>) -> Result<f64>;
        fn read_string(self: Pin<&mut StreamWrapper>) -> Result<String>;

        // Raw byte read from socket
        fn read_bytes(self: Pin<&mut StreamWrapper>, buf: &mut [u8]) -> Result<i64>;

        /// Hand off the underlying connection as a streaming `DataHandle`.
        ///
        /// Only TCP streams support this; memory streams throw. After the
        /// call the source stream is left in an unspecified state and must
        /// be dropped — only the returned `DataHandleWrapper` holds the
        /// connection.
        fn into_data_handle(self: Pin<&mut StreamWrapper>) -> Result<UniquePtr<DataHandleWrapper>>;

        // Buffer access (memory streams)
        fn buffer(self: Pin<&mut StreamWrapper>) -> Result<&[u8]>;
        fn bytes_written(self: &StreamWrapper) -> i64;

        /// Connect to a TCP host:port and return a stream.
        fn stream_connect(host: &str, port: i32) -> Result<UniquePtr<StreamWrapper>>;

        /// Create a resizable memory stream for writing.
        #[must_use]
        fn stream_memory_write() -> UniquePtr<StreamWrapper>;

        /// Create a fixed memory stream for reading from existing data.
        #[must_use]
        fn stream_memory_read(data: &[u8]) -> UniquePtr<StreamWrapper>;

        // ==================== Library registration ====================

        /// Register a Rust-implemented library with eckit's LibraryManager.
        /// Enables `~name` tilde expansion via `$NAME_HOME` env var.
        fn register_library(lib: Box<LibraryBox>);

        /// Get configuration for a registered library by name.
        fn library_configuration(name: &str) -> Result<UniquePtr<ConfigWrapper>>;

        /// Snapshot of every ECMWF library registered with
        /// `eckit::system::LibraryManager` (e.g. eckit, metkit, fdb5, mir).
        /// Mirrors C++ `Environment::library_versions()` (mars-client).
        #[must_use]
        fn library_versions() -> Vec<LibraryVersion>;
    }

    extern "Rust" {
        /// Called from C++ RustLogTarget to emit log messages via Rust's log crate.
        fn rust_log(level: LogLevel, msg: &str);

        /// Opaque Rust box holding a `dyn Library` trait object.
        type LibraryBox;

        // Callbacks from C++ RustLibrary into Rust trait methods
        fn library_name(lib: &LibraryBox) -> &str;
        fn library_version(lib: &LibraryBox) -> String;
        fn library_git_sha1(lib: &LibraryBox, count: u32) -> String;
        fn library_home(lib: &LibraryBox) -> String;
        fn library_home_is_set(lib: &LibraryBox) -> bool;
        fn library_library_home(lib: &LibraryBox) -> String;
        fn library_library_home_is_set(lib: &LibraryBox) -> bool;
        fn library_prefix_directory(lib: &LibraryBox) -> String;
        fn library_prefix_directory_is_set(lib: &LibraryBox) -> bool;
        fn library_expand_path(lib: &LibraryBox, path: &str) -> String;
        fn library_expand_path_is_set(lib: &LibraryBox, path: &str) -> bool;
        fn library_debug(lib: &LibraryBox) -> bool;
        fn library_debug_is_set(lib: &LibraryBox) -> bool;
    }
}

// Public re-exports for the safe wrapper crate
pub use cxx::{Exception, UniquePtr};
pub use ffi::*;

/// Called from C++ `RustLogTarget::write()` — routes to Rust `log` crate.
fn rust_log(level: ffi::LogLevel, msg: &str) {
    match level {
        ffi::LogLevel::Error => log::error!(target: "eckit", "{msg}"),
        ffi::LogLevel::Warn => log::warn!(target: "eckit", "{msg}"),
        ffi::LogLevel::Info => log::info!(target: "eckit", "{msg}"),
        ffi::LogLevel::Debug => log::debug!(target: "eckit", "{msg}"),
        // Trace + wildcard for cxx non-exhaustive enum
        _ => log::trace!(target: "eckit", "{msg}"),
    }
}

// ==================== Library registration (internal plumbing) ====================

type OptStringFn = Box<dyn Fn() -> Option<String> + Send + Sync>;
type OptStringArgFn = Box<dyn Fn(&str) -> Option<String> + Send + Sync>;
type OptBoolFn = Box<dyn Fn() -> Option<bool> + Send + Sync>;

/// Opaque box holding library callbacks for FFI. Constructed by the `eckit` crate.
pub struct LibraryBox {
    pub name: String,
    pub version_fn: Box<dyn Fn() -> String + Send + Sync>,
    pub git_sha1_fn: Box<dyn Fn(u32) -> String + Send + Sync>,
    pub home_fn: OptStringFn,
    pub library_home_fn: OptStringFn,
    pub prefix_directory_fn: OptStringFn,
    pub expand_path_fn: OptStringArgFn,
    pub debug_fn: OptBoolFn,
}

// Callbacks from C++ RustLibrary into Rust closures

fn library_name(lib: &LibraryBox) -> &str {
    &lib.name
}

fn library_version(lib: &LibraryBox) -> String {
    (lib.version_fn)()
}

fn library_git_sha1(lib: &LibraryBox, count: u32) -> String {
    (lib.git_sha1_fn)(count)
}

fn library_home(lib: &LibraryBox) -> String {
    (lib.home_fn)().unwrap_or_default()
}

fn library_home_is_set(lib: &LibraryBox) -> bool {
    (lib.home_fn)().is_some()
}

fn library_library_home(lib: &LibraryBox) -> String {
    (lib.library_home_fn)().unwrap_or_default()
}

fn library_library_home_is_set(lib: &LibraryBox) -> bool {
    (lib.library_home_fn)().is_some()
}

fn library_prefix_directory(lib: &LibraryBox) -> String {
    (lib.prefix_directory_fn)().unwrap_or_default()
}

fn library_prefix_directory_is_set(lib: &LibraryBox) -> bool {
    (lib.prefix_directory_fn)().is_some()
}

fn library_expand_path(lib: &LibraryBox, path: &str) -> String {
    (lib.expand_path_fn)(path).unwrap_or_default()
}

fn library_expand_path_is_set(lib: &LibraryBox, path: &str) -> bool {
    (lib.expand_path_fn)(path).is_some()
}

fn library_debug(lib: &LibraryBox) -> bool {
    (lib.debug_fn)().unwrap_or(false)
}

fn library_debug_is_set(lib: &LibraryBox) -> bool {
    (lib.debug_fn)().is_some()
}

/// Initialize eckit runtime with Rust log bridge.
///
/// Must be called before any eckit API usage. Safe to call multiple times.
///
/// This creates a `RustMain` C++ object that overrides eckit's log target
/// factory methods, ensuring every thread gets log output routed through
/// Rust's `log` crate.
pub fn init() {
    ffi::init();
}
