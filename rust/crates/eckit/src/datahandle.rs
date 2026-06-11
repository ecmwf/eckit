//! `DataHandle` wrapper — abstract I/O with typestate for read/write mode.
//!
//! The handle is modal: opened for read OR write, never both.
//! The typestate pattern encodes this at compile time:
//!
//! - `DataHandle<Closed>` — not yet opened, call `open_for_read()` or `open_for_write()`
//! - `DataHandle<Reading>` — opened for read, implements `Read + Seek`
//! - `DataHandle<Writing>` — opened for write, implements `Write`

use std::io::{Read, Seek, SeekFrom, Write};
use std::marker::PhantomData;
use std::path::Path;

use crate::error::Result;

// ==================== State types ====================

mod sealed {
    pub trait Sealed {}
}

/// Trait for handle states. Sealed — cannot be implemented outside this crate.
pub trait HandleState: sealed::Sealed {
    /// Whether the handle needs closing on drop.
    const NEEDS_CLOSE: bool;
}

/// Handle is not yet opened.
pub struct Closed;
impl sealed::Sealed for Closed {}
impl HandleState for Closed {
    const NEEDS_CLOSE: bool = false;
}

/// Handle is opened for reading.
pub struct Reading;
impl sealed::Sealed for Reading {}
impl HandleState for Reading {
    const NEEDS_CLOSE: bool = true;
}

/// Handle is opened for writing.
pub struct Writing;
impl sealed::Sealed for Writing {}
impl HandleState for Writing {
    const NEEDS_CLOSE: bool = true;
}

// ==================== DataHandle ====================

/// Wraps `eckit::DataHandle` with compile-time read/write mode enforcement.
pub struct DataHandle<S: HandleState = Closed> {
    // Option so we can .take() when transitioning states (Drop still runs on consumed self)
    inner: Option<eckit_sys::UniquePtr<eckit_sys::DataHandleWrapper>>,
    _state: PhantomData<S>,
}

// SAFETY: The underlying C++ DataHandle is accessed through &mut self only.
#[allow(clippy::non_send_fields_in_send_ty)]
unsafe impl<S: HandleState> Send for DataHandle<S> {}

impl DataHandle<Closed> {
    /// Create from a raw eckit-sys wrapper.
    #[must_use]
    pub const fn from_raw(inner: eckit_sys::UniquePtr<eckit_sys::DataHandleWrapper>) -> Self {
        Self {
            inner: Some(inner),
            _state: PhantomData,
        }
    }

    /// Open a file as a `DataHandle`.
    pub fn from_path(path: impl AsRef<Path>) -> Result<Self> {
        let path_str = path_to_str(path.as_ref())?;
        let inner = eckit_sys::data_handle_from_file(path_str).map_err(eckit_sys::Error::from)?;
        Ok(Self::from_raw(inner))
    }

    /// Open a byte range of a file as a `DataHandle`.
    pub fn from_part(path: impl AsRef<Path>, offset: i64, length: i64) -> Result<Self> {
        let path_str = path_to_str(path.as_ref())?;
        let inner = eckit_sys::data_handle_from_part(path_str, offset, length)
            .map_err(eckit_sys::Error::from)?;
        Ok(Self::from_raw(inner))
    }

    /// Create a `DataHandle` from an in-memory buffer (copies the data).
    pub fn from_buffer(data: &[u8]) -> Result<Self> {
        let inner = eckit_sys::data_handle_from_buffer(data).map_err(eckit_sys::Error::from)?;
        Ok(Self::from_raw(inner))
    }

    /// Create a `MultiHandle` from multiple file paths.
    ///
    /// Reads from multiple files sequentially. Mirrors C++ `eckit::MultiHandle`.
    pub fn from_multi(paths: &[impl AsRef<Path>]) -> Result<Self> {
        let strings = paths_to_strings(paths)?;
        let inner = eckit_sys::data_handle_from_multi(&strings).map_err(eckit_sys::Error::from)?;
        Ok(Self::from_raw(inner))
    }

    /// Create a `TeeHandle` from multiple file paths — writes to all targets in parallel.
    ///
    /// One write call fans out to every destination. Mirrors C++ `eckit::TeeHandle`.
    pub fn tee(paths: &[impl AsRef<Path>]) -> Result<Self> {
        let strings = paths_to_strings(paths)?;
        let inner = eckit_sys::data_handle_tee(&strings).map_err(eckit_sys::Error::from)?;
        Ok(Self::from_raw(inner))
    }

    /// Wrap a Rust `std::io::Read` source as a `DataHandle`.
    ///
    /// The C++ side calls back into the Rust reader on each `read()`; no
    /// intermediate buffer or temp file is staged. Forward-only — the
    /// resulting handle cannot be seeked or written to.
    pub fn from_reader<R>(reader: R) -> Result<Self>
    where
        R: std::io::Read + std::io::Seek + Send + 'static,
    {
        let inner = eckit_sys::data_handle_from_reader(eckit_sys::make_reader_box(reader))
            .map_err(eckit_sys::Error::from)?;
        Ok(Self::from_raw(inner))
    }

    /// Open for reading. Returns the opened handle and estimated data length.
    pub fn open_for_read(mut self) -> Result<(DataHandle<Reading>, i64)> {
        let mut inner = self
            .inner
            .take()
            .ok_or_else(|| eckit_sys::Error::SeriousBug("handle already consumed".to_string()))?;
        let len = inner
            .pin_mut()
            .open_for_read()
            .map_err(eckit_sys::Error::from)?;
        Ok((
            DataHandle {
                inner: Some(inner),
                _state: PhantomData,
            },
            len,
        ))
    }

    /// Open for writing with estimated length.
    pub fn open_for_write(mut self, estimated_length: i64) -> Result<DataHandle<Writing>> {
        let mut inner = self
            .inner
            .take()
            .ok_or_else(|| eckit_sys::Error::SeriousBug("handle already consumed".to_string()))?;
        inner
            .pin_mut()
            .open_for_write(estimated_length)
            .map_err(eckit_sys::Error::from)?;
        Ok(DataHandle {
            inner: Some(inner),
            _state: PhantomData,
        })
    }
}

impl<S: HandleState> DataHandle<S> {
    /// Current position in the data.
    pub fn position(&self) -> Result<i64> {
        self.inner_ref()?.position().map_err(eckit_sys::Error::from)
    }

    /// Whether seeking is supported.
    pub fn can_seek(&self) -> Result<bool> {
        Ok(self.inner_ref()?.can_seek())
    }

    /// Estimated total size.
    pub fn estimate(&self) -> Result<i64> {
        self.inner_ref()?.estimate().map_err(eckit_sys::Error::from)
    }

    fn inner_ref(&self) -> Result<&eckit_sys::DataHandleWrapper> {
        self.inner
            .as_deref()
            .ok_or_else(|| eckit_sys::Error::SeriousBug("handle already consumed".to_string()))
    }

    /// Access the underlying C++ `DataHandleWrapper` for FFI interop.
    pub fn inner_mut(&mut self) -> Result<std::pin::Pin<&mut eckit_sys::DataHandleWrapper>> {
        Ok(self
            .inner
            .as_mut()
            .ok_or_else(|| eckit_sys::Error::SeriousBug("handle already consumed".to_string()))?
            .pin_mut())
    }
}

impl DataHandle<Reading> {
    /// Copy all data from this handle into a writing handle.
    ///
    /// Mirrors C++ `DataHandle::saveInto()`.
    pub fn save_into(&mut self, target: &mut DataHandle<Writing>) -> Result<i64> {
        self.inner_mut()?
            .save_into(target.inner_mut()?)
            .map_err(eckit_sys::Error::from)
    }

    /// Close and return to closed state.
    ///
    /// Surfaces underlying close errors to the caller (the `Drop` impl
    /// silently swallows them). Prefer calling `close()` explicitly when
    /// you care about commit/flush failures.
    pub fn close(mut self) -> Result<DataHandle<Closed>> {
        if let Some(ref mut inner) = self.inner {
            inner.pin_mut().close().map_err(eckit_sys::Error::from)?;
        }
        Ok(DataHandle {
            inner: self.inner.take(),
            _state: PhantomData,
        })
    }

    /// Access the underlying wrapper (for `MessageReader`).
    pub(crate) fn pin_mut(&mut self) -> Result<std::pin::Pin<&mut eckit_sys::DataHandleWrapper>> {
        self.inner_mut()
    }
}

impl DataHandle<Writing> {
    /// Close and return to closed state.
    ///
    /// Surfaces underlying close/flush errors to the caller (the `Drop`
    /// impl silently swallows them). Prefer calling `close()` explicitly
    /// when you care about commit/flush failures — e.g. write targets
    /// where a buffered tail of bytes might not yet be persisted.
    pub fn close(mut self) -> Result<DataHandle<Closed>> {
        if let Some(ref mut inner) = self.inner {
            inner.pin_mut().close().map_err(eckit_sys::Error::from)?;
        }
        Ok(DataHandle {
            inner: self.inner.take(),
            _state: PhantomData,
        })
    }
}

// ==================== std::io trait impls ====================

impl Read for DataHandle<Reading> {
    fn read(&mut self, buf: &mut [u8]) -> std::io::Result<usize> {
        let n = self
            .inner_mut()
            .map_err(|e| std::io::Error::other(e.to_string()))?
            .read(buf)
            .map_err(|e| std::io::Error::other(e.to_string()))?;
        usize::try_from(n).map_err(|e| std::io::Error::other(e.to_string()))
    }
}

impl Seek for DataHandle<Reading> {
    fn seek(&mut self, pos: SeekFrom) -> std::io::Result<u64> {
        let new_pos = match pos {
            SeekFrom::Start(offset) => offset,
            SeekFrom::End(offset) => {
                let size = self
                    .estimate()
                    .map_err(|e| std::io::Error::other(e.to_string()))?;
                let new = size
                    .checked_add(offset)
                    .ok_or_else(|| std::io::Error::other("seek position overflow"))?;
                if new < 0 {
                    return Err(std::io::Error::other("seek to negative position"));
                }
                new.cast_unsigned()
            }
            SeekFrom::Current(offset) => {
                let current = self
                    .position()
                    .map_err(|e| std::io::Error::other(e.to_string()))?;
                let new = current
                    .checked_add(offset)
                    .ok_or_else(|| std::io::Error::other("seek position overflow"))?;
                if new < 0 {
                    return Err(std::io::Error::other("seek to negative position"));
                }
                new.cast_unsigned()
            }
        };

        let actual = self
            .inner_mut()
            .map_err(|e| std::io::Error::other(e.to_string()))?
            .seek(i64::try_from(new_pos).map_err(|e| std::io::Error::other(e.to_string()))?)
            .map_err(|e| std::io::Error::other(e.to_string()))?;

        // Some `eckit::DataHandle` subclasses can land at an offset
        // different from the requested one (e.g. `MultiHandle` clamps to
        // the closest part boundary). Trust the C++ return value rather
        // than echoing what we asked for.
        if actual < 0 {
            return Err(std::io::Error::other(format!(
                "DataHandle::seek returned negative offset: {actual}"
            )));
        }
        Ok(actual.cast_unsigned())
    }
}

impl Write for DataHandle<Writing> {
    fn write(&mut self, buf: &[u8]) -> std::io::Result<usize> {
        let n = self
            .inner_mut()
            .map_err(|e| std::io::Error::other(e.to_string()))?
            .write(buf)
            .map_err(|e| std::io::Error::other(e.to_string()))?;
        usize::try_from(n).map_err(|e| std::io::Error::other(e.to_string()))
    }

    /// `eckit::DataHandle::flush()` is virtual but its base implementation
    /// throws `NotImplemented`, so most handle types do not actually
    /// support a mid-stream flush. The commit boundary for write-side
    /// handles is [`DataHandle::close`] — call it explicitly to surface
    /// any flush/commit errors. This `Write::flush` impl is therefore a
    /// no-op by design.
    fn flush(&mut self) -> std::io::Result<()> {
        Ok(())
    }
}

// ==================== Drop ====================

impl<S: HandleState> Drop for DataHandle<S> {
    fn drop(&mut self) {
        if let Some(ref mut inner) = self.inner
            && S::NEEDS_CLOSE
        {
            let _ = inner.pin_mut().close();
        }
    }
}

// ==================== Helpers ====================

fn path_to_str(path: &Path) -> Result<&str> {
    path.to_str().ok_or_else(|| {
        eckit_sys::Error::Other(format!("path is not valid UTF-8: {}", path.display()))
    })
}

fn paths_to_strings(paths: &[impl AsRef<Path>]) -> Result<Vec<String>> {
    paths
        .iter()
        .map(|p| path_to_str(p.as_ref()).map(String::from))
        .collect()
}
