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
        #[Self = "DataHandleWrapper"]
        fn from_file(path: &str) -> Result<UniquePtr<DataHandleWrapper>>;

        /// Open a byte range of a file as a DataHandle.
        #[Self = "DataHandleWrapper"]
        fn from_part(path: &str, offset: i64, length: i64) -> Result<UniquePtr<DataHandleWrapper>>;

        /// Create a DataHandle from an in-memory buffer (copies the data).
        #[Self = "DataHandleWrapper"]
        fn from_buffer(data: &[u8]) -> Result<UniquePtr<DataHandleWrapper>>;

        /// Create a MultiHandle from multiple file paths.
        #[Self = "DataHandleWrapper"]
        fn from_multi(paths: &[String]) -> Result<UniquePtr<DataHandleWrapper>>;

        /// Create a TeeHandle from multiple file paths — writes all targets in parallel.
        #[Self = "DataHandleWrapper"]
        fn tee(paths: &[String]) -> Result<UniquePtr<DataHandleWrapper>>;

        /// Create a DataHandle that forwards `read()` calls to a Rust
        /// `std::io::Read` source wrapped in a [`ReaderBox`].
        #[Self = "DataHandleWrapper"]
        fn from_reader(reader: Box<ReaderBox>) -> Result<UniquePtr<DataHandleWrapper>>;

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
    }

    extern "Rust" {
        /// Called from C++ RustLogTarget to emit log messages via Rust's log crate.
        /// `target` is the tracing/log target — "eckit" for global channels,
        /// the library name (e.g. "metkit", "mir") for per-library debug channels.
        fn rust_log(level: LogLevel, target: &str, msg: &str);

        /// Opaque Rust box holding a `dyn ReadSeek + Send` source.
        ///
        /// Constructed via [`make_reader_box`]; the C++ `RustReaderHandle`
        /// holds it by `rust::Box<ReaderBox>` and forwards `read()` / `seek()`
        /// calls through [`invoke_reader_read`] / [`invoke_reader_seek`].
        type ReaderBox;

        /// Called by the C++ `RustReaderHandle::read` shim to fill the next
        /// chunk from the wrapped Rust `Read` source.
        ///
        /// Returns the number of bytes written into `buf`, or `-1` on error.
        /// A return of `0` signals EOF.
        fn invoke_reader_read(reader: &mut ReaderBox, buf: &mut [u8]) -> i64;

        /// Called by the C++ `RustReaderHandle::seek` / `openForRead` shim to
        /// reposition the wrapped Rust source. `offset` is from the start.
        ///
        /// Returns the new absolute position, or `-1` on error.
        fn invoke_reader_seek(reader: &mut ReaderBox, offset: i64) -> i64;
    }
}

// Public re-exports for the safe wrapper crate
pub use cxx::{Exception, UniquePtr};
pub use ffi::*;

// ==================== Read → DataHandle adapter ====================

/// Trait alias for `Read + Seek`.
///
/// Required so the trait object inside [`ReaderBox`] exposes both `read`
/// (for streaming bytes into C++) and `seek` (for rewind on `openForRead`,
/// matching `eckit::FileHandle`'s `fopen("r")` semantics).
pub trait ReadSeek: std::io::Read + std::io::Seek {}
impl<T: std::io::Read + std::io::Seek + ?Sized> ReadSeek for T {}

/// Opaque wrapper holding a `Box<dyn ReadSeek + Send>`.
///
/// The C++ `RustReaderHandle` (declared in `eckit_bridge.h` as `struct
/// ReaderBox`) carries this by `rust::Box<ReaderBox>` and forwards each C++
/// `read(void*, long)` / `seek(Offset)` call via [`invoke_reader_read`] /
/// [`invoke_reader_seek`].
pub struct ReaderBox(Box<dyn ReadSeek + Send>);

/// Wrap a Rust `Read + Seek` source for [`ffi::data_handle_from_reader`].
///
/// The `Seek` bound is load-bearing: eckit's `DataHandle::openForRead`
/// contract is "leave at offset 0", so the C++ side rewinds the source on
/// every re-open.
pub fn make_reader_box<R>(reader: R) -> Box<ReaderBox>
where
    R: std::io::Read + std::io::Seek + Send + 'static,
{
    Box::new(ReaderBox(Box::new(reader)))
}

/// Called from C++ `RustReaderHandle::read` to fill the next chunk from the
/// wrapped Rust reader. Returns the byte count, `0` on EOF, or `-1` on error.
fn invoke_reader_read(reader: &mut ReaderBox, buf: &mut [u8]) -> i64 {
    reader
        .0
        .read(buf)
        .map_or(-1, |n| i64::try_from(n).unwrap_or(i64::MAX))
}

/// Called from C++ `RustReaderHandle::seek` / `openForRead` to reposition
/// the inner Rust source from the start. Returns the new position, or `-1`
/// on error.
fn invoke_reader_seek(reader: &mut ReaderBox, offset: i64) -> i64 {
    let Ok(off_u64) = u64::try_from(offset) else {
        return -1;
    };
    reader
        .0
        .seek(std::io::SeekFrom::Start(off_u64))
        .map_or(-1, |n| i64::try_from(n).unwrap_or(i64::MAX))
}

/// Called from C++ `RustLogTarget::write()` — routes to Rust `log` crate.
fn rust_log(level: ffi::LogLevel, target: &str, msg: &str) {
    if target.is_empty() {
        match level {
            ffi::LogLevel::Error => log::error!("{msg}"),
            ffi::LogLevel::Warn => log::warn!("{msg}"),
            ffi::LogLevel::Info => log::info!("{msg}"),
            ffi::LogLevel::Debug => log::debug!("{msg}"),
            _ => log::trace!("{msg}"),
        }
    } else {
        match level {
            ffi::LogLevel::Error => log::error!(target: target, "{msg}"),
            ffi::LogLevel::Warn => log::warn!(target: target, "{msg}"),
            ffi::LogLevel::Info => log::info!(target: target, "{msg}"),
            ffi::LogLevel::Debug => log::debug!(target: target, "{msg}"),
            _ => log::trace!(target: target, "{msg}"),
        }
    }
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
