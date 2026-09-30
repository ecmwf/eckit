.. SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
.. SPDX-License-Identifier: Apache-2.0

Building the Rust Crates
========================

The crates build with Cargo. CMake is driven by the build script of
``eckit-sys`` and does not need to be run by hand.

Toolchain Requirements
----------------------

* Rust 1.90 or later
* A C++17 compiler
* CMake and git

Vendored and System Builds
--------------------------

Two Cargo features select where the C++ library comes from. They are mutually
exclusive.

``vendored`` (default)
   Builds eckit from source into Cargo's output directory, using ecbuild, which
   is downloaded automatically. When the crate is used from a checkout of the
   eckit repository, by path or as a git dependency, the C++ sources of that
   same checkout are compiled, so Rust and C++ changes can be developed and
   tested together. Otherwise the release tag matching the ``eckit-sys``
   version is cloned.

``system``
   Links an installed eckit, located with CMake ``find_package(eckit)``.
   Requires eckit 2.0.7 or later. Set ``ECKIT_DIR`` or ``CMAKE_PREFIX_PATH``
   if it is not in a default location:

   .. code-block:: toml

      [dependencies]
      eckit = { version = "2.3", default-features = false, features = ["system"] }

   .. code-block:: bash

      ECKIT_DIR=/path/to/eckit/install cargo build

Use ``system`` when the program must share one eckit installation with other
software, for example on the HPC or in a bundle build. Use ``vendored`` for a
self-contained build.

The shared libraries of a vendored build are found at run time through rpath
entries on the final binary. An application adds them from its build script
with ``bindman_utils::emit_rpaths()``.

Crate Versions
--------------

Both crates carry the version of the eckit release they wrap. The version is
set once for the workspace in ``rust/Cargo.toml``, and a test in each crate
fails if it drifts from the ``VERSION`` file of the repository.

Cargo Features
--------------

The ``eckit`` crate has three features:

.. list-table::
   :widths: 25 15 60
   :header-rows: 1

   * - Feature
     - Default
     - Effect
   * - ``vendored``
     - on
     - Build eckit from source
   * - ``system``
     - off
     - Link an installed eckit
   * - ``geo``
     - on
     - The ``eckit::geo`` module (``Grid``, ``Spec``), in releases that include
       the geo bindings

``eckit-sys`` exposes the CMake options of eckit as features, so a vendored
build can be configured from ``Cargo.toml``. A feature switches on the
``ENABLE_*`` option of the same name, for example ``eckit-sql`` sets
``ENABLE_ECKIT_SQL=ON``.

.. list-table::
   :widths: 25 50 25
   :header-rows: 1

   * - Group
     - Features
     - Default
   * - Libraries
     - ``eckit-codec``, ``eckit-spec``, ``eckit-geo``
     - on
   * - Libraries
     - ``eckit-sql``
     - off
   * - Platform
     - ``unicode``, ``aio``
     - on
   * - MPI
     - ``mpi``
     - off
   * - Compression
     - ``bzip2``, ``snappy``, ``lz4``, ``aec``, ``zip``
     - off
   * - Hashing
     - ``xxhash``
     - off
   * - Linear algebra
     - ``eigen``, ``lapack``, ``mkl``, ``omp``
     - off
   * - Network
     - ``curl``, ``ssl``
     - off
   * - GPU
     - ``cuda``, ``hip``
     - off
   * - Geo
     - ``geo-codec-grids``, ``geo-caching``, ``geo-bitreproducible``,
       ``geo-projection-proj-default``, ``geo-area-shapefile``, ``proj``
     - off
   * - Other
     - ``rados``, ``jemalloc``, ``rsync``, ``convex-hull``, ``experimental``,
       ``sandbox``
     - off

Features that are off by default need the corresponding external library to
be installed. To switch one on, add ``eckit-sys`` as a dependency next to
``eckit`` and enable the feature there:

.. code-block:: toml

   [dependencies]
   eckit = "2.3"
   eckit-sys = { version = "2.3", features = ["lz4"] }

Build Environment Variables
---------------------------

.. list-table::
   :widths: 30 70
   :header-rows: 1

   * - Variable
     - Effect
   * - ``ECKIT_DIR``
     - Install prefix of eckit, used by ``system`` builds
   * - ``CMAKE_PREFIX_PATH``
     - Additional CMake search paths
   * - ``DOCS_RS``
     - When set, the C++ build is skipped and only the generated error type is
       produced, which is enough for ``cargo doc``

Working on the Crates
---------------------

All commands run from the ``rust/`` directory of the repository:

.. code-block:: bash

   cargo clippy --all-targets     # lint; the workspace enables pedantic lints
   cargo fmt                      # format Rust sources
   cargo test                     # tests
   cargo doc --no-deps --open     # API reference

The first build compiles the whole C++ library. Later builds reuse it and
recompile only when C++ sources change.
