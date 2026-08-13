//! Build script for eckit-sys
//!
//! Supports two build modes:
//! - `vendored` (default): Clone and build eckit from source using ecbuild
//! - `system`: Use `CMake` `find_package` to find system-installed eckit

fn main() {
    println!("cargo:rerun-if-changed=build.rs");
    println!("cargo:rerun-if-env-changed=ECKIT_DIR");
    println!("cargo:rerun-if-env-changed=CMAKE_PREFIX_PATH");
    println!("cargo:rerun-if-env-changed=DOCS_RS");

    if bindman_utils::is_docs_rs() {
        generate_exceptions(&docs_source_include());
        return;
    }

    bindman_utils::validate_build_mode(cfg!(feature = "system"), cfg!(feature = "vendored"));

    let include = if cfg!(feature = "system") {
        build_system()
    } else {
        build_vendored()
    };

    generate_exceptions(&include);
    build_cxx_bridge(&include);

    let crate_dir =
        std::path::PathBuf::from(std::env::var("CARGO_MANIFEST_DIR").expect("CARGO_MANIFEST_DIR"));
    bindman_build::check_cpp_api(&include, &crate_dir.join("src/lib.rs"));

    // Export cpp directory for downstream crates that include EckitBridge.h
    println!("cargo:cpp_dir={}", crate_dir.join("cpp").display());
}

/// Compile the CXX bridge
fn build_cxx_bridge(include: &std::path::Path) {
    let crate_dir = std::path::PathBuf::from(
        std::env::var("CARGO_MANIFEST_DIR").expect("CARGO_MANIFEST_DIR not set"),
    );
    let out_dir = std::path::PathBuf::from(std::env::var("OUT_DIR").expect("OUT_DIR not set"));

    println!("cargo:rerun-if-changed=cpp/EckitBridge.h");
    println!("cargo:rerun-if-changed=cpp/RustLogTarget.h");
    println!("cargo:rerun-if-changed=cpp/RustLogTarget.cc");
    println!("cargo:rerun-if-changed=cpp/ConfigWrapper.h");
    println!("cargo:rerun-if-changed=cpp/ConfigWrapper.cc");
    println!("cargo:rerun-if-changed=cpp/DataHandleWrapper.h");
    println!("cargo:rerun-if-changed=cpp/DataHandleWrapper.cc");
    println!("cargo:rerun-if-changed=cpp/MessageWrapper.h");
    println!("cargo:rerun-if-changed=cpp/MessageWrapper.cc");
    println!("cargo:rerun-if-changed=cpp/StreamWrapper.h");
    println!("cargo:rerun-if-changed=cpp/StreamWrapper.cc");

    cxx_build::bridge("src/lib.rs")
        .file(crate_dir.join("cpp/RustLogTarget.cc"))
        .file(crate_dir.join("cpp/ConfigWrapper.cc"))
        .file(crate_dir.join("cpp/DataHandleWrapper.cc"))
        .file(crate_dir.join("cpp/MessageWrapper.cc"))
        .file(crate_dir.join("cpp/StreamWrapper.cc"))
        .include(include)
        .include(crate_dir.join("cpp"))
        .include(&out_dir) // for eckit_exceptions.h
        .flag_if_supported("-std=c++17")
        .compile("eckit_sys_bridge");

    bindman_utils::link_cpp_stdlib();
}

/// Generate exception bridge files from eckit's `Exceptions.h`. Publishes the
/// source list so downstream `-sys` crates can re-parse and inherit eckit's
/// catch blocks into their own generated bridge.
fn generate_exceptions(include: &std::path::Path) {
    let out_dir = std::path::PathBuf::from(std::env::var("OUT_DIR").expect("OUT_DIR not set"));

    let own = vec![bindman_build::ExceptionSource {
        header: include.join("eckit/exception/Exceptions.h"),
        include_path: "eckit/exception/Exceptions.h".to_string(),
        cpp_namespace: "eckit".to_string(),
        message_prefix: "eckit".to_string(),
        base_class: "Exception".to_string(),
        recursive: true,
    }];

    bindman_build::generate_exception_bridge(&bindman_build::ExceptionBridgeConfig {
        primary_namespace: "eckit",
        out_dir: &out_dir,
        own: &own,
        inherited: &[],
    });

    // Publish for downstream `-sys` crates that inherit eckit's exceptions.
    bindman_build::publish_exception_sources(&own, &out_dir);
}

/// Header root for docs builds (`DOCS_RS=1`), where the native eckit build
/// — normally the provider of the include tree — is skipped. `docs-headers/`
/// mirrors the include-tree layout, holding a symlink into this repo's
/// `src/`; `cargo package` embeds the linked file's content, so the same
/// path serves in-repo checkouts and published crates alike. Docs builds
/// consume only the generated Rust side; the C++ catch-block header is
/// never compiled.
fn docs_source_include() -> std::path::PathBuf {
    std::path::PathBuf::from(std::env::var("CARGO_MANIFEST_DIR").expect("CARGO_MANIFEST_DIR"))
        .join("docs-headers")
}

/// Build using system-installed eckit via `CMake` `find_package`
#[cfg(feature = "system")]
fn build_system() -> std::path::PathBuf {
    // Minimum supported system version, independent of the crate version
    // (which tracks the vendored eckit release).
    let (root, include, lib_dir) = bindman_utils::cmake_find_package("eckit", "2.0.7");

    println!("cargo:rustc-link-search=native={}", lib_dir.display());
    println!("cargo:rustc-link-lib=dylib=eckit");
    bindman_utils::link_cpp_stdlib();

    // Export for downstream crates
    println!("cargo:root={}", root.display());
    println!("cargo:include={}", include.display());

    include
}

#[cfg(not(feature = "system"))]
fn build_system() -> std::path::PathBuf {
    unreachable!("build_system called without system feature");
}

/// Locate the eckit C++ sources. When the crate lives inside the eckit
/// repository (path dependency, or a git dependency — cargo checks out the
/// whole repo), the sources are three levels up from the crate and we build
/// them directly: branch changes take effect and no tag/network is required.
/// Cloning the release tag is the fallback for the packaged (crates.io) case,
/// where the crate ships without the C++ tree.
#[cfg(feature = "vendored")]
fn resolve_eckit_src(src_dir: &std::path::Path) -> std::path::PathBuf {
    const ECKIT_REPO: &str = "https://github.com/ecmwf/eckit.git";
    const ECKIT_TAG: &str = env!("CARGO_PKG_VERSION");

    let manifest_dir = std::path::PathBuf::from(
        std::env::var("CARGO_MANIFEST_DIR").expect("CARGO_MANIFEST_DIR not set"),
    );
    if let Some(root) = manifest_dir.ancestors().nth(3)
        && root.join("CMakeLists.txt").exists()
        && root.join("VERSION").exists()
        && root.join("src/eckit").is_dir()
    {
        eprintln!("eckit-sys: building in-tree sources at {}", root.display());

        // Retrigger on C++ source edits.
        println!("cargo:rerun-if-changed={}", root.join("src").display());
        println!(
            "cargo:rerun-if-changed={}",
            root.join("CMakeLists.txt").display()
        );
        println!("cargo:rerun-if-changed={}", root.join("VERSION").display());

        // Diverging is legitimate mid-development (unreleased C++ changes are
        // the point of in-tree builds), but should never go unnoticed. The
        // crate-version test enforces equality at release time.
        let tree_version = std::fs::read_to_string(root.join("VERSION"))
            .map(|s| s.trim().to_string())
            .unwrap_or_default();
        if tree_version != ECKIT_TAG {
            println!(
                "cargo:warning=eckit-sys {ECKIT_TAG} is building in-tree eckit {tree_version} (versions differ)"
            );
        }

        return root.to_path_buf();
    }
    bindman_utils::git_clone(ECKIT_REPO, ECKIT_TAG, &src_dir.join("eckit"))
}

/// Build eckit from source using ecbuild
#[cfg(feature = "vendored")]
#[allow(clippy::too_many_lines)]
fn build_vendored() -> std::path::PathBuf {
    use std::env;
    use std::fs;
    use std::path::PathBuf;
    use std::process::Command;

    const ECBUILD_REPO: &str = "https://github.com/ecmwf/ecbuild.git";
    const ECBUILD_TAG: &str = "3.13.1";

    let out_dir = PathBuf::from(env::var("OUT_DIR").expect("OUT_DIR not set"));
    let src_dir = out_dir.join("src");
    let build_dir = out_dir.join("build");
    let install_dir = out_dir.join("install");

    fs::create_dir_all(&src_dir).expect("Failed to create src directory");
    fs::create_dir_all(&build_dir).expect("Failed to create build directory");

    // Clone ecbuild (always external); eckit comes from the in-tree checkout
    // when available, falling back to a clone of the release tag.
    let ecbuild_src = bindman_utils::git_clone(ECBUILD_REPO, ECBUILD_TAG, &src_dir.join("ecbuild"));
    let eckit_src = resolve_eckit_src(&src_dir);

    // CMakeCache.txt pins the source path the build dir was configured with;
    // cmake hard-errors if it changes (e.g. switching between cloned and
    // in-tree sources). Wipe the build dir when the cached path is stale.
    if let Ok(cache) = fs::read_to_string(build_dir.join("CMakeCache.txt")) {
        let cached_src = cache
            .lines()
            .find_map(|l| l.strip_prefix("CMAKE_HOME_DIRECTORY:INTERNAL="));
        if cached_src != eckit_src.to_str() {
            fs::remove_dir_all(&build_dir).expect("Failed to remove stale eckit build directory");
            fs::create_dir_all(&build_dir).expect("Failed to create build directory");
        }
    }

    // Configure with ecbuild
    let ecbuild_bin = ecbuild_src.join("bin/ecbuild");

    let mut cmd = Command::new(&ecbuild_bin);
    cmd.current_dir(&build_dir)
        .arg(format!("--prefix={}", install_dir.display()))
        .arg("--")
        .arg(&eckit_src)
        .arg(format!(
            "-DCMAKE_BUILD_TYPE={}",
            bindman_utils::cmake_build_type()
        ))
        // Always disabled (no features)
        .arg("-DENABLE_TESTS=OFF")
        .arg("-DENABLE_DOCS=OFF")
        .arg("-DENABLE_BUILD_TOOLS=OFF")
        .arg(format!(
            "-DENABLE_ECKIT_SQL={}",
            bindman_utils::on_off(cfg!(feature = "eckit-sql"))
        ))
        .arg("-DENABLE_ECKIT_CMD=OFF")
        .arg("-DENABLE_EXTRA_TESTS=OFF");

    // Core libraries
    cmd.arg(format!(
        "-DENABLE_ECKIT_CODEC={}",
        bindman_utils::on_off(cfg!(feature = "eckit-codec"))
    ));
    cmd.arg(format!(
        "-DENABLE_ECKIT_SPEC={}",
        bindman_utils::on_off(cfg!(feature = "eckit-spec"))
    ));
    cmd.arg(format!(
        "-DENABLE_ECKIT_GEO={}",
        bindman_utils::on_off(cfg!(feature = "eckit-geo"))
    ));

    // MPI
    cmd.arg(format!(
        "-DENABLE_MPI={}",
        bindman_utils::on_off(cfg!(feature = "mpi"))
    ));

    // Compression codecs
    cmd.arg(format!(
        "-DENABLE_BZIP2={}",
        bindman_utils::on_off(cfg!(feature = "bzip2"))
    ));
    cmd.arg(format!(
        "-DENABLE_SNAPPY={}",
        bindman_utils::on_off(cfg!(feature = "snappy"))
    ));
    cmd.arg(format!(
        "-DENABLE_LZ4={}",
        bindman_utils::on_off(cfg!(feature = "lz4"))
    ));
    cmd.arg(format!(
        "-DENABLE_AEC={}",
        bindman_utils::on_off(cfg!(feature = "aec"))
    ));
    cmd.arg(format!(
        "-DENABLE_ZIP={}",
        bindman_utils::on_off(cfg!(feature = "zip"))
    ));

    // Hashing
    cmd.arg(format!(
        "-DENABLE_XXHASH={}",
        bindman_utils::on_off(cfg!(feature = "xxhash"))
    ));

    // Linear algebra
    cmd.arg(format!(
        "-DENABLE_EIGEN={}",
        bindman_utils::on_off(cfg!(feature = "eigen"))
    ));
    cmd.arg(format!(
        "-DENABLE_LAPACK={}",
        bindman_utils::on_off(cfg!(feature = "lapack"))
    ));
    cmd.arg(format!(
        "-DENABLE_MKL={}",
        bindman_utils::on_off(cfg!(feature = "mkl"))
    ));
    cmd.arg(format!(
        "-DENABLE_OMP={}",
        bindman_utils::on_off(cfg!(feature = "omp"))
    ));

    // Network
    cmd.arg(format!(
        "-DENABLE_CURL={}",
        bindman_utils::on_off(cfg!(feature = "curl"))
    ));
    cmd.arg(format!(
        "-DENABLE_SSL={}",
        bindman_utils::on_off(cfg!(feature = "ssl"))
    ));

    // Other features
    cmd.arg(format!(
        "-DENABLE_UNICODE={}",
        bindman_utils::on_off(cfg!(feature = "unicode"))
    ));
    cmd.arg(format!(
        "-DENABLE_AIO={}",
        bindman_utils::on_off(cfg!(feature = "aio"))
    ));
    cmd.arg(format!(
        "-DENABLE_PROJ={}",
        bindman_utils::on_off(cfg!(feature = "proj"))
    ));
    cmd.arg(format!(
        "-DENABLE_RADOS={}",
        bindman_utils::on_off(cfg!(feature = "rados"))
    ));
    cmd.arg(format!(
        "-DENABLE_JEMALLOC={}",
        bindman_utils::on_off(cfg!(feature = "jemalloc"))
    ));
    cmd.arg(format!(
        "-DENABLE_RSYNC={}",
        bindman_utils::on_off(cfg!(feature = "rsync"))
    ));

    // GPU support
    cmd.arg(format!(
        "-DENABLE_CUDA={}",
        bindman_utils::on_off(cfg!(feature = "cuda"))
    ));
    cmd.arg(format!(
        "-DENABLE_HIP={}",
        bindman_utils::on_off(cfg!(feature = "hip"))
    ));

    // Geo sub-features
    cmd.arg(format!(
        "-DENABLE_GEO_CACHING={}",
        bindman_utils::on_off(cfg!(feature = "geo-caching"))
    ));
    cmd.arg(format!(
        "-DENABLE_GEO_BITREPRODUCIBLE={}",
        bindman_utils::on_off(cfg!(feature = "geo-bitreproducible"))
    ));
    cmd.arg(format!(
        "-DENABLE_GEO_PROJECTION_PROJ_DEFAULT={}",
        bindman_utils::on_off(cfg!(feature = "geo-projection-proj-default"))
    ));
    cmd.arg(format!(
        "-DENABLE_GEO_AREA_SHAPEFILE={}",
        bindman_utils::on_off(cfg!(feature = "geo-area-shapefile"))
    ));

    // Advanced
    cmd.arg(format!(
        "-DENABLE_CONVEX_HULL={}",
        bindman_utils::on_off(cfg!(feature = "convex-hull"))
    ));

    // Experimental
    cmd.arg(format!(
        "-DENABLE_EXPERIMENTAL={}",
        bindman_utils::on_off(cfg!(feature = "experimental"))
    ));
    cmd.arg(format!(
        "-DENABLE_SANDBOX={}",
        bindman_utils::on_off(cfg!(feature = "sandbox"))
    ));

    // Use @rpath install names — the leaf binary sets rpaths via bindman_utils::emit_rpaths()
    #[cfg(target_os = "macos")]
    cmd.arg("-DCMAKE_INSTALL_NAME_DIR=@rpath");

    #[cfg(target_os = "linux")]
    {
        cmd.arg("-DCMAKE_INSTALL_RPATH=$ORIGIN:$ORIGIN/../lib");
        cmd.arg("-DCMAKE_BUILD_WITH_INSTALL_RPATH=ON");
    }

    bindman_utils::run_command(&mut cmd, "ecbuild configure");

    // Build
    let num_jobs = bindman_utils::build_parallelism();

    bindman_utils::run_command(
        Command::new("cmake")
            .args(["--build", ".", "--parallel", &num_jobs])
            .current_dir(&build_dir),
        "cmake build",
    );

    // Install
    bindman_utils::run_command(
        Command::new("cmake")
            .args(["--install", "."])
            .current_dir(&build_dir),
        "cmake install",
    );

    // Link directives
    let lib_dir = bindman_utils::resolve_lib_dir(&install_dir);

    println!("cargo:rustc-link-search=native={}", lib_dir.display());
    println!("cargo:rustc-link-lib=dylib=eckit");
    bindman_utils::link_cpp_stdlib();

    // Export for downstream crates
    let include = install_dir.join("include");
    println!("cargo:root={}", install_dir.display());
    println!("cargo:include={}", include.display());

    include
}

#[cfg(not(feature = "vendored"))]
fn build_vendored() -> std::path::PathBuf {
    unreachable!("build_vendored called without vendored feature");
}
