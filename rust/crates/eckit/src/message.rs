//! Message and `MessageReader` — GRIB message abstraction.

use crate::datahandle::DataHandle;
use crate::error::Result;

/// A GRIB/BUFR message read from a `DataHandle`.
pub struct Message {
    inner: eckit_sys::UniquePtr<eckit_sys::MessageWrapper>,
}

impl Message {
    /// Create from a raw eckit-sys wrapper.
    pub(crate) const fn from_raw(inner: eckit_sys::UniquePtr<eckit_sys::MessageWrapper>) -> Self {
        Self { inner }
    }

    /// Create a Message from raw bytes.
    ///
    /// Uses `DataHandle::from_buffer` + `MessageReader` internally.
    pub fn from_data(data: &[u8]) -> Result<Self> {
        let handle = DataHandle::from_buffer(data)?;
        let (mut reading, _) = handle.open_for_read()?;
        let mut reader = MessageReader::new(&mut reading)?;
        reader
            .next()
            .ok_or_else(|| eckit_sys::Error::SeriousBug("no message in data".to_string()))?
    }

    /// Whether this message contains valid data.
    #[must_use]
    pub fn is_valid(&self) -> bool {
        self.inner.is_valid()
    }

    /// Total size of the message in bytes.
    #[must_use]
    pub fn length(&self) -> usize {
        self.inner.length()
    }

    /// Byte offset of this message within its source data handle, as reported
    /// by [`MessageReader`]. Mirrors C++ `eckit::message::Message::offset()`.
    #[must_use]
    pub fn offset(&self) -> i64 {
        self.inner.offset()
    }

    /// Raw message data.
    #[must_use]
    pub fn data(&self) -> &[u8] {
        self.inner.data()
    }

    /// Access the underlying `MessageWrapper` for FFI interop.
    #[must_use]
    pub fn as_sys(&self) -> &eckit_sys::MessageWrapper {
        // inner is always Some for a valid Message
        &self.inner
    }

    /// Get a typed metadata value.
    pub fn get<T: MessageGet>(&self, key: &str) -> Result<T> {
        T::get_from(&self.inner, key)
    }

    /// Write this message to a `DataHandle` opened for writing.
    pub fn write_to(&self, handle: &mut DataHandle<crate::Writing>) -> Result<()> {
        self.inner
            .write_to(handle.inner_mut()?)
            .map_err(eckit_sys::Error::from)
    }
}

impl Message {
    /// Clone this message (ref-counted internally, cheap).
    pub fn try_clone(&self) -> Result<Self> {
        let inner = self.inner.clone_message().map_err(eckit_sys::Error::from)?;
        Ok(Self { inner })
    }
}

/// Iterator over messages from a `DataHandle`.
///
/// # Example
///
/// ```ignore
/// let handle = /* DataHandle<Closed> from FDB or file */;
/// let (mut handle, _len) = handle.open_for_read()?;
/// for msg in MessageReader::new(&mut handle)? {
///     let msg = msg?;
///     let param: String = msg.get("shortName")?;
///     println!("{param}: {} bytes", msg.length());
/// }
/// ```
/// A streaming reader over GRIB messages in a [`DataHandle<Reading>`].
///
/// The reader is tied to the lifetime of the borrow on the source handle
/// (`'h`). The underlying C++ `eckit::message::Reader` stores a reference to
/// the `DataHandle` (via an internal `BufferedHandle` constructed with
/// `opened=true`), so dropping the `DataHandle` before the reader would
/// leave a dangling reference inside C++. Encoding the borrow in the Rust
/// type makes the compiler enforce the rule.
pub struct MessageReader<'h> {
    inner: eckit_sys::UniquePtr<eckit_sys::ReaderWrapper>,
    _handle: std::marker::PhantomData<&'h mut DataHandle<crate::Reading>>,
}

impl<'h> MessageReader<'h> {
    /// Create a reader over an opened `DataHandle<Reading>`. The returned
    /// reader borrows the handle for `'h` — it cannot outlive the handle.
    pub fn new(handle: &'h mut DataHandle<crate::Reading>) -> Result<Self> {
        let inner = eckit_sys::new_reader(handle.pin_mut()?).map_err(eckit_sys::Error::from)?;
        Ok(Self {
            inner,
            _handle: std::marker::PhantomData,
        })
    }
}

impl Iterator for MessageReader<'_> {
    type Item = Result<Message>;

    fn next(&mut self) -> Option<Self::Item> {
        match self.inner.pin_mut().next() {
            Ok(wrapper) => {
                let msg = Message::from_raw(wrapper);
                if msg.is_valid() { Some(Ok(msg)) } else { None }
            }
            Err(e) => Some(Err(eckit_sys::Error::from(e))),
        }
    }
}

// ==================== Generic get trait ====================

/// Trait for types that can be read from message metadata.
pub trait MessageGet: Sized {
    /// Get a value from the message.
    fn get_from(msg: &eckit_sys::UniquePtr<eckit_sys::MessageWrapper>, key: &str) -> Result<Self>;
}

impl MessageGet for String {
    fn get_from(msg: &eckit_sys::UniquePtr<eckit_sys::MessageWrapper>, key: &str) -> Result<Self> {
        msg.get_string(key).map_err(eckit_sys::Error::from)
    }
}

impl MessageGet for i64 {
    fn get_from(msg: &eckit_sys::UniquePtr<eckit_sys::MessageWrapper>, key: &str) -> Result<Self> {
        msg.get_long(key).map_err(eckit_sys::Error::from)
    }
}

impl MessageGet for f64 {
    fn get_from(msg: &eckit_sys::UniquePtr<eckit_sys::MessageWrapper>, key: &str) -> Result<Self> {
        msg.get_double(key).map_err(eckit_sys::Error::from)
    }
}
