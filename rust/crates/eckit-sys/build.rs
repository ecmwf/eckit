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

    // Export OUT_DIR for downstream crates that need eckit_exceptions.h
    let out_dir = std::path::PathBuf::from(std::env::var("OUT_DIR").expect("OUT_DIR"));
    println!("cargo:out_dir={}", out_dir.display());

    // Export cpp directory for downstream crates that include eckit_bridge.h
    println!("cargo:cpp_dir={}", crate_dir.join("cpp").display());
}

/// Compile the CXX bridge
fn build_cxx_bridge(include: &std::path::Path) {
    let crate_dir = std::path::PathBuf::from(
        std::env::var("CARGO_MANIFEST_DIR").expect("CARGO_MANIFEST_DIR not set"),
    );
    let out_dir = std::path::PathBuf::from(std::env::var("OUT_DIR").expect("OUT_DIR not set"));

    println!("cargo:rerun-if-changed=cpp/eckit_bridge.h");
    println!("cargo:rerun-if-changed=cpp/eckit_bridge.cpp");

    cxx_build::bridge("src/lib.rs")
        .file(crate_dir.join("cpp/eckit_bridge.cpp"))
        .include(include)
        .include(crate_dir.join("cpp"))
        .include(&out_dir) // for eckit_exceptions.h
        .flag_if_supported("-std=c++17")
        .compile("eckit_sys_bridge");

    bindman_utils::link_cpp_stdlib();
}

/// Generate exception bridge files from eckit's `Exceptions.h`.
fn generate_exceptions(include: &std::path::Path) {
    let out_dir = std::path::PathBuf::from(std::env::var("OUT_DIR").expect("OUT_DIR not set"));
    let header = include.join("eckit/exception/Exceptions.h");

    bindman_build::generate_exception_bridge(&bindman_build::ExceptionBridgeConfig {
        header: &header,
        base_class: "Exception",
        namespace: "eckit",
        out_dir: &out_dir,
    });

    // Export path to generated header for downstream -sys crates
    println!(
        "cargo:exceptions_header={}",
        out_dir.join("eckit_exceptions.h").display()
    );
}

/// Minimum eckit version this crate's bridge is known to compile against.
/// `system` builds require >= this version; `vendored` builds clone exactly
/// this tag — keeping both modes pinned to the same source revision so
/// downstream code can rely on the same API surface either way.
const ECKIT_VERSION: &str = "2.0.7";

#[cfg(feature = "system")]
fn build_system() -> std::path::PathBuf {
    let (root, include, lib_dir) = bindman_utils::cmake_find_package("eckit", ECKIT_VERSION);

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
    const ECKIT_REPO: &str = "https://github.com/ecmwf/eckit.git";
    // Pinned via the shared `ECKIT_VERSION` const above — `system` mode
    // requires the same minimum so both modes give the same API surface.
    const ECKIT_TAG: &str = ECKIT_VERSION;

    let out_dir = PathBuf::from(env::var("OUT_DIR").expect("OUT_DIR not set"));
    let src_dir = out_dir.join("src");
    let build_dir = out_dir.join("build");
    let install_dir = out_dir.join("install");

    fs::create_dir_all(&src_dir).expect("Failed to create src directory");
    fs::create_dir_all(&build_dir).expect("Failed to create build directory");

    // Clone ecbuild and eckit
    let ecbuild_src = bindman_utils::git_clone(ECBUILD_REPO, ECBUILD_TAG, &src_dir.join("ecbuild"));
    let eckit_src = bindman_utils::git_clone(ECKIT_REPO, ECKIT_TAG, &src_dir.join("eckit"));

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
