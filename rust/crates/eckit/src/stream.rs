//! Stream — tagged binary serialization over TCP or memory.
//!
//! Wraps `eckit::Stream` (`TCPStream`, `MemoryStream`, `ResizableMemoryStream`).
//! Used by the Hermes protocol for `MarsRequest` serialization.

use crate::error::Result;

/// Tagged binary stream — primitive read/write operations.
///
/// Implemented by [`TcpStream`] and [`MemoryStream`].
/// Hermes protocol code uses `&mut dyn Stream` for testability.
pub trait Stream {
    fn write_u8(&mut self, v: u8) -> Result<()>;
    fn write_bool(&mut self, v: bool) -> Result<()>;
    fn write_i32(&mut self, v: i32) -> Result<()>;
    fn write_i64(&mut self, v: i64) -> Result<()>;
    fn write_u64(&mut self, v: u64) -> Result<()>;
    fn write_f64(&mut self, v: f64) -> Result<()>;
    fn write_string(&mut self, v: &str) -> Result<()>;
    fn write_blob(&mut self, data: &[u8]) -> Result<()>;

    fn read_u8(&mut self) -> Result<u8>;
    fn read_bool(&mut self) -> Result<bool>;
    fn read_i32(&mut self) -> Result<i32>;
    fn read_i64(&mut self) -> Result<i64>;
    fn read_u64(&mut self) -> Result<u64>;
    fn read_f64(&mut self) -> Result<f64>;
    fn read_string(&mut self) -> Result<String>;

    fn bytes_written(&self) -> i64;

    /// Access the underlying C++ stream wrapper (for cross-crate FFI).
    fn as_sys_mut(&mut self) -> std::pin::Pin<&mut eckit_sys::StreamWrapper>;
}

/// Serialize a value to a stream.
pub trait StreamWrite {
    fn write_to(&self, stream: &mut dyn Stream) -> Result<()>;
}

/// Deserialize a value from a stream.
pub trait StreamRead: Sized {
    fn read_from(stream: &mut dyn Stream) -> Result<Self>;
}

// ==================== Macro for Stream impl delegation ====================

/// Implements [`Stream`] by delegating to the inner `eckit_sys::StreamWrapper`.
macro_rules! impl_stream {
    ($ty:ty) => {
        impl Stream for $ty {
            fn write_u8(&mut self, v: u8) -> Result<()> {
                self.inner
                    .pin_mut()
                    .write_char(v)
                    .map_err(eckit_sys::Error::from)
            }
            fn write_bool(&mut self, v: bool) -> Result<()> {
                self.inner
                    .pin_mut()
                    .write_bool(v)
                    .map_err(eckit_sys::Error::from)
            }
            fn write_i32(&mut self, v: i32) -> Result<()> {
                self.inner
                    .pin_mut()
                    .write_int(v)
                    .map_err(eckit_sys::Error::from)
            }
            fn write_i64(&mut self, v: i64) -> Result<()> {
                self.inner
                    .pin_mut()
                    .write_long(v)
                    .map_err(eckit_sys::Error::from)
            }
            fn write_u64(&mut self, v: u64) -> Result<()> {
                self.inner
                    .pin_mut()
                    .write_unsigned_long(v)
                    .map_err(eckit_sys::Error::from)
            }
            fn write_f64(&mut self, v: f64) -> Result<()> {
                self.inner
                    .pin_mut()
                    .write_double(v)
                    .map_err(eckit_sys::Error::from)
            }
            fn write_string(&mut self, v: &str) -> Result<()> {
                self.inner
                    .pin_mut()
                    .write_string(v)
                    .map_err(eckit_sys::Error::from)
            }
            fn write_blob(&mut self, data: &[u8]) -> Result<()> {
                self.inner
                    .pin_mut()
                    .write_blob(data)
                    .map_err(eckit_sys::Error::from)
            }
            fn read_u8(&mut self) -> Result<u8> {
                self.inner
                    .pin_mut()
                    .read_char()
                    .map_err(eckit_sys::Error::from)
            }
            fn read_bool(&mut self) -> Result<bool> {
                self.inner
                    .pin_mut()
                    .read_bool()
                    .map_err(eckit_sys::Error::from)
            }
            fn read_i32(&mut self) -> Result<i32> {
                self.inner
                    .pin_mut()
                    .read_int()
                    .map_err(eckit_sys::Error::from)
            }
            fn read_i64(&mut self) -> Result<i64> {
                self.inner
                    .pin_mut()
                    .read_long()
                    .map_err(eckit_sys::Error::from)
            }
            fn read_u64(&mut self) -> Result<u64> {
                self.inner
                    .pin_mut()
                    .read_unsigned_long()
                    .map_err(eckit_sys::Error::from)
            }
            fn read_f64(&mut self) -> Result<f64> {
                self.inner
                    .pin_mut()
                    .read_double()
                    .map_err(eckit_sys::Error::from)
            }
            fn read_string(&mut self) -> Result<String> {
                self.inner
                    .pin_mut()
                    .read_string()
                    .map_err(eckit_sys::Error::from)
            }
            fn bytes_written(&self) -> i64 {
                self.inner.bytes_written()
            }
            fn as_sys_mut(&mut self) -> std::pin::Pin<&mut eckit_sys::StreamWrapper> {
                self.inner.pin_mut()
            }
        }
    };
}

// ==================== TcpStream ====================

/// TCP stream — connects to a Hermes server.
pub struct TcpStream {
    inner: eckit_sys::UniquePtr<eckit_sys::StreamWrapper>,
}

#[allow(clippy::non_send_fields_in_send_ty)]
unsafe impl Send for TcpStream {}

impl TcpStream {
    /// Connect to a TCP host:port.
    pub fn connect(host: &str, port: i32) -> Result<Self> {
        let inner = eckit_sys::stream_connect(host, port).map_err(eckit_sys::Error::from)?;
        Ok(Self { inner })
    }

    /// Read raw bytes from the underlying socket (for data transfer after protocol handshake).
    pub fn read_bytes(&mut self, buf: &mut [u8]) -> Result<i64> {
        self.inner
            .pin_mut()
            .read_bytes(buf)
            .map_err(eckit_sys::Error::from)
    }
}

/// Hand off the connected socket as a streaming `DataHandle`.
///
/// Use after a protocol handshake when the remainder of the connection should
/// be drained as raw bytes — the resulting `DataHandle` reads on demand from
/// the socket, with no in-memory buffering. Consumes the [`TcpStream`].
impl TryFrom<TcpStream> for crate::DataHandle {
    type Error = crate::Error;

    fn try_from(mut stream: TcpStream) -> Result<Self> {
        let inner = stream
            .inner
            .pin_mut()
            .into_data_handle()
            .map_err(eckit_sys::Error::from)?;
        Ok(Self::from_raw(inner))
    }
}

impl_stream!(TcpStream);

// ==================== MemoryStream ====================

/// Memory-backed stream — for testing and in-memory serialization.
pub struct MemoryStream {
    inner: eckit_sys::UniquePtr<eckit_sys::StreamWrapper>,
}

#[allow(clippy::non_send_fields_in_send_ty)]
unsafe impl Send for MemoryStream {}

impl MemoryStream {
    /// Create a resizable memory stream for writing.
    #[must_use]
    pub fn writer() -> Self {
        Self {
            inner: eckit_sys::stream_memory_write(),
        }
    }

    /// Create a memory stream for reading from existing data.
    #[must_use]
    pub fn reader(data: &[u8]) -> Self {
        Self {
            inner: eckit_sys::stream_memory_read(data),
        }
    }

    /// Get the written data (writer streams only).
    pub fn buffer(&mut self) -> Result<&[u8]> {
        self.inner
            .pin_mut()
            .buffer()
            .map_err(eckit_sys::Error::from)
    }
}

impl_stream!(MemoryStream);

// ==================== StreamWrite impls for primitive types ====================

impl StreamWrite for u8 {
    fn write_to(&self, stream: &mut dyn Stream) -> Result<()> {
        stream.write_u8(*self)
    }
}

impl StreamWrite for bool {
    fn write_to(&self, stream: &mut dyn Stream) -> Result<()> {
        stream.write_bool(*self)
    }
}

impl StreamWrite for i32 {
    fn write_to(&self, stream: &mut dyn Stream) -> Result<()> {
        stream.write_i32(*self)
    }
}

impl StreamWrite for i64 {
    fn write_to(&self, stream: &mut dyn Stream) -> Result<()> {
        stream.write_i64(*self)
    }
}

impl StreamWrite for u64 {
    fn write_to(&self, stream: &mut dyn Stream) -> Result<()> {
        stream.write_u64(*self)
    }
}

impl StreamWrite for f64 {
    fn write_to(&self, stream: &mut dyn Stream) -> Result<()> {
        stream.write_f64(*self)
    }
}

impl StreamWrite for str {
    fn write_to(&self, stream: &mut dyn Stream) -> Result<()> {
        stream.write_string(self)
    }
}

impl StreamWrite for String {
    fn write_to(&self, stream: &mut dyn Stream) -> Result<()> {
        stream.write_string(self)
    }
}

impl StreamWrite for [u8] {
    fn write_to(&self, stream: &mut dyn Stream) -> Result<()> {
        stream.write_blob(self)
    }
}

// ==================== StreamRead impls for primitive types ====================

impl StreamRead for u8 {
    fn read_from(stream: &mut dyn Stream) -> Result<Self> {
        stream.read_u8()
    }
}

impl StreamRead for bool {
    fn read_from(stream: &mut dyn Stream) -> Result<Self> {
        stream.read_bool()
    }
}

impl StreamRead for i32 {
    fn read_from(stream: &mut dyn Stream) -> Result<Self> {
        stream.read_i32()
    }
}

impl StreamRead for i64 {
    fn read_from(stream: &mut dyn Stream) -> Result<Self> {
        stream.read_i64()
    }
}

impl StreamRead for u64 {
    fn read_from(stream: &mut dyn Stream) -> Result<Self> {
        stream.read_u64()
    }
}

impl StreamRead for f64 {
    fn read_from(stream: &mut dyn Stream) -> Result<Self> {
        stream.read_f64()
    }
}

impl StreamRead for String {
    fn read_from(stream: &mut dyn Stream) -> Result<Self> {
        stream.read_string()
    }
}
