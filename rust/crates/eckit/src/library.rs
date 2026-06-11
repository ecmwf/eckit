//! Library registration — register Rust libraries with eckit's `LibraryManager`.
//!
//! Enables `~name` tilde expansion via `$NAME_HOME` env var.

use std::sync::Arc;

/// Trait for registering a library with eckit's `LibraryManager`.
///
/// Implement this to register your crate with eckit, enabling
/// `~name` path expansion (resolved via `$NAME_HOME` env var).
///
/// # Example
///
/// ```ignore
/// struct MarsClient;
///
/// impl eckit::Library for MarsClient {
///     fn name(&self) -> &str { "mars" }
///     fn version(&self) -> String { env!("CARGO_PKG_VERSION").to_string() }
/// }
///
/// eckit::register(MarsClient);
/// // Now ~mars/etc/... expands via $MARS_HOME
/// ```
pub trait Library: Send + Sync {
    /// Library name (e.g. `"mars"`). Used for `~name` path expansion.
    fn name(&self) -> &'static str;

    /// Version string.
    fn version(&self) -> String;

    /// Git SHA1 (truncated to `count` characters).
    fn git_sha1(&self, _count: u32) -> String {
        String::new()
    }

    /// Home directory. `None` = use C++ default (`$NAME_HOME` env var).
    fn home(&self) -> Option<String> {
        None
    }

    /// Library home (resolved). `None` = use C++ default.
    fn library_home(&self) -> Option<String> {
        None
    }

    /// Prefix directory. `None` = use C++ default (from `dladdr`).
    fn prefix_directory(&self) -> Option<String> {
        None
    }

    /// Expand a `~name/...` path. `None` = use C++ default.
    fn expand_path(&self, _path: &str) -> Option<String> {
        None
    }

    /// Debug flag. `None` = use C++ default.
    fn debug(&self) -> Option<bool> {
        None
    }
}

/// Register a library with eckit's `LibraryManager`.
///
/// After registration, `~name` paths expand via `$NAME_HOME` env var.
pub fn register(lib: impl Library + 'static) {
    let lib = Arc::new(lib);
    let name = lib.name().to_string();

    let boxed = eckit_sys::LibraryBox {
        name,
        version_fn: Box::new({
            let lib = Arc::clone(&lib);
            move || lib.version()
        }),
        git_sha1_fn: Box::new({
            let lib = Arc::clone(&lib);
            move |count| lib.git_sha1(count)
        }),
        home_fn: Box::new({
            let lib = Arc::clone(&lib);
            move || lib.home()
        }),
        library_home_fn: Box::new({
            let lib = Arc::clone(&lib);
            move || lib.library_home()
        }),
        prefix_directory_fn: Box::new({
            let lib = Arc::clone(&lib);
            move || lib.prefix_directory()
        }),
        expand_path_fn: Box::new({
            let lib = Arc::clone(&lib);
            move |path| lib.expand_path(path)
        }),
        debug_fn: Box::new({
            let lib = Arc::clone(&lib);
            move || lib.debug()
        }),
    };

    eckit_sys::register_library(Box::new(boxed));
}

/// Get configuration for a registered library by name.
///
/// Mirrors C++ `eckit::system::Library::lookup(name).configuration()`.
/// Loads `~name/etc/name/config.yaml` (or `$NAME_CONFIG_PATH`).
pub fn library_configuration(name: &str) -> crate::Result<crate::Config> {
    let inner = eckit_sys::library_configuration(name).map_err(eckit_sys::Error::from)?;
    Ok(crate::Config::from_raw(inner))
}

/// Snapshot of one ECMWF library registered with `eckit::system::LibraryManager`.
///
/// Re-export of the cxx shared struct from `eckit-sys` — public fields:
/// `name`, `version`, `gitsha1`, `home`.
pub use eckit_sys::LibraryVersion;

/// All ECMWF libraries currently registered with `LibraryManager`.
///
/// Mirrors C++ `Environment::library_versions()` (mars-client) — used by the
/// startup banner and by metrics reporting.
#[must_use]
pub fn library_versions() -> Vec<LibraryVersion> {
    eckit_sys::library_versions()
}
