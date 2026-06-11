//! Configuration wrapper for `eckit::LocalConfiguration`.

use std::path::Path;
use std::pin::Pin;
use std::str::FromStr;

use crate::error::Result;

/// Wraps `eckit::LocalConfiguration`.
///
/// # Example
///
/// ```ignore
/// let cfg = Config::from_path("config.yaml")?;
/// let name: String = cfg.get("name", "")?;
/// let port: i64 = cfg.get("port", 9000)?;
///
/// for sub in cfg.subs("databases")? {
///     let sub = sub?;
///     let class: String = sub.get("class", "")?;
/// }
/// ```
pub struct Config {
    inner: eckit_sys::UniquePtr<eckit_sys::ConfigWrapper>,
}

// SAFETY: The underlying C++ `LocalConfiguration` is accessed through
// `&mut self` only on mutation paths (mirroring the `DataHandle` Send
// marker — same rationale). `Sync` follows because the `&self` getters
// read immutable state with no interior mutability on the C++ side.
#[allow(clippy::non_send_fields_in_send_ty)]
unsafe impl Send for Config {}
unsafe impl Sync for Config {}

impl Config {
    /// Create an empty configuration.
    #[must_use]
    pub fn new() -> Self {
        Self {
            inner: eckit_sys::config_create(),
        }
    }

    /// Create from a raw eckit-sys wrapper.
    pub(crate) const fn from_raw(inner: eckit_sys::UniquePtr<eckit_sys::ConfigWrapper>) -> Self {
        Self { inner }
    }

    /// Load configuration from a YAML file.
    pub fn from_path(path: impl AsRef<Path>) -> Result<Self> {
        let path_str = path.as_ref().to_str().ok_or_else(|| {
            eckit_sys::Error::Other(format!(
                "path is not valid UTF-8: {}",
                path.as_ref().display()
            ))
        })?;
        let inner = eckit_sys::config_from_path(path_str).map_err(eckit_sys::Error::from)?;
        Ok(Self { inner })
    }

    /// Check if a key exists.
    #[must_use]
    pub fn has(&self, key: &str) -> bool {
        self.inner.has(key)
    }

    /// Check if a key's value is a list.
    #[must_use]
    pub fn is_list(&self, key: &str) -> bool {
        self.inner.is_list(key)
    }

    /// Check if configuration is empty.
    #[must_use]
    pub fn is_empty(&self) -> bool {
        self.inner.is_empty()
    }

    /// Get a typed value with a default.
    pub fn get<T: ConfigGet>(&self, key: &str, default: T) -> Result<T> {
        T::get_from(&self.inner, key, default)
    }

    /// Set a typed value. Returns `&mut Self` for chaining.
    pub fn set<T: ConfigSet>(&mut self, key: &str, value: T) -> &mut Self {
        value.set_on(self.inner.pin_mut(), key);
        self
    }

    /// Get a sub-configuration.
    pub fn sub(&self, key: &str) -> Result<Self> {
        let inner = self.inner.get_sub(key).map_err(eckit_sys::Error::from)?;
        Ok(Self { inner })
    }

    /// Access the underlying eckit-sys wrapper (for FFI interop with other -sys crates).
    #[must_use]
    pub fn as_sys(&self) -> &eckit_sys::ConfigWrapper {
        &self.inner
    }

    /// Iterate sub-configurations.
    ///
    /// `None` iterates root-level list entries (e.g. `databases.yaml`).
    /// `Some("key")` iterates entries under a named key.
    pub fn subs(&self, key: Option<&str>) -> Result<SubConfigs<'_>> {
        let count = match key {
            None => self
                .inner
                .root_sub_count()
                .map_err(eckit_sys::Error::from)?,
            Some(k) => self.inner.sub_count(k).map_err(eckit_sys::Error::from)?,
        };
        Ok(SubConfigs {
            config: self,
            key: key.map(String::from),
            count,
            index: 0,
        })
    }
}

impl Clone for Config {
    fn clone(&self) -> Self {
        Self {
            inner: eckit_sys::config_clone(&self.inner),
        }
    }
}

impl Default for Config {
    fn default() -> Self {
        Self::new()
    }
}

impl FromStr for Config {
    type Err = eckit_sys::Error;

    /// Parse a YAML string into a configuration.
    fn from_str(yaml: &str) -> std::result::Result<Self, Self::Err> {
        let inner = eckit_sys::config_from_yaml(yaml).map_err(eckit_sys::Error::from)?;
        Ok(Self { inner })
    }
}

/// Lazy iterator over sub-configurations.
pub struct SubConfigs<'a> {
    config: &'a Config,
    key: Option<String>,
    count: usize,
    index: usize,
}

impl Iterator for SubConfigs<'_> {
    type Item = Result<Config>;

    fn next(&mut self) -> Option<Self::Item> {
        if self.index >= self.count {
            return None;
        }
        let result = match &self.key {
            None => self.config.inner.root_sub_at(self.index),
            Some(k) => self.config.inner.sub_at(k, self.index),
        }
        .map(|inner| Config { inner })
        .map_err(eckit_sys::Error::from);
        self.index += 1;
        Some(result)
    }

    fn size_hint(&self) -> (usize, Option<usize>) {
        let remaining = self.count - self.index;
        (remaining, Some(remaining))
    }
}

impl ExactSizeIterator for SubConfigs<'_> {}

// ==================== Generic get/set traits ====================

/// Trait for types that can be read from a configuration.
pub trait ConfigGet: Sized {
    /// Get a value from the configuration with a default.
    fn get_from(
        config: &eckit_sys::UniquePtr<eckit_sys::ConfigWrapper>,
        key: &str,
        default: Self,
    ) -> Result<Self>;
}

/// Trait for types that can be written to a configuration.
pub trait ConfigSet {
    /// Set a value on the configuration.
    fn set_on(self, config: Pin<&mut eckit_sys::ConfigWrapper>, key: &str);
}

// --- ConfigGet impls ---

impl ConfigGet for String {
    fn get_from(
        config: &eckit_sys::UniquePtr<eckit_sys::ConfigWrapper>,
        key: &str,
        default: Self,
    ) -> Result<Self> {
        config
            .get_string(key, &default)
            .map_err(eckit_sys::Error::from)
    }
}

impl ConfigGet for i64 {
    fn get_from(
        config: &eckit_sys::UniquePtr<eckit_sys::ConfigWrapper>,
        key: &str,
        default: Self,
    ) -> Result<Self> {
        config
            .get_long(key, default)
            .map_err(eckit_sys::Error::from)
    }
}

impl ConfigGet for i32 {
    fn get_from(
        config: &eckit_sys::UniquePtr<eckit_sys::ConfigWrapper>,
        key: &str,
        default: Self,
    ) -> Result<Self> {
        config.get_int(key, default).map_err(eckit_sys::Error::from)
    }
}

impl ConfigGet for bool {
    fn get_from(
        config: &eckit_sys::UniquePtr<eckit_sys::ConfigWrapper>,
        key: &str,
        default: Self,
    ) -> Result<Self> {
        config
            .get_bool(key, default)
            .map_err(eckit_sys::Error::from)
    }
}

impl ConfigGet for f64 {
    fn get_from(
        config: &eckit_sys::UniquePtr<eckit_sys::ConfigWrapper>,
        key: &str,
        default: Self,
    ) -> Result<Self> {
        config
            .get_double(key, default)
            .map_err(eckit_sys::Error::from)
    }
}

impl ConfigGet for Vec<String> {
    fn get_from(
        config: &eckit_sys::UniquePtr<eckit_sys::ConfigWrapper>,
        key: &str,
        default: Self,
    ) -> Result<Self> {
        config
            .get_string_vector(key, &default)
            .map_err(eckit_sys::Error::from)
    }
}

// --- ConfigSet impls ---

impl ConfigSet for &str {
    fn set_on(self, config: Pin<&mut eckit_sys::ConfigWrapper>, key: &str) {
        config.set_string(key, self);
    }
}

impl ConfigSet for String {
    fn set_on(self, config: Pin<&mut eckit_sys::ConfigWrapper>, key: &str) {
        config.set_string(key, &self);
    }
}

impl ConfigSet for i64 {
    fn set_on(self, config: Pin<&mut eckit_sys::ConfigWrapper>, key: &str) {
        config.set_long(key, self);
    }
}

impl ConfigSet for i32 {
    fn set_on(self, config: Pin<&mut eckit_sys::ConfigWrapper>, key: &str) {
        config.set_int(key, self);
    }
}

impl ConfigSet for bool {
    fn set_on(self, config: Pin<&mut eckit_sys::ConfigWrapper>, key: &str) {
        config.set_bool(key, self);
    }
}

impl ConfigSet for f64 {
    fn set_on(self, config: Pin<&mut eckit_sys::ConfigWrapper>, key: &str) {
        config.set_double(key, self);
    }
}
