.. SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
.. SPDX-License-Identifier: Apache-2.0

Building the Rust Crates
========================

.. code-block:: bash

   cargo add eckit
   cargo build

This builds eckit from source as part of ``cargo build`` (the ``vendored``
feature, on by default). It needs Rust 1.90 or later, a C++17 compiler, CMake
and git. The first build also needs network access, to download ecbuild and
the eckit release matching the crate version. When the crate comes from a checkout of
the eckit repository, as a path or git dependency, the C++ sources of that
checkout are compiled instead.

To link an installed eckit, 2.0.7 or later, switch to the ``system`` feature
and point CMake at it with ``ECKIT_DIR`` or ``CMAKE_PREFIX_PATH``:

.. code-block:: bash

   cargo add eckit --no-default-features --features system,geo
   ECKIT_DIR=/path/to/eckit/install cargo build

Use ``system`` when the program must share one eckit installation with other
software, for example on the HPC or in a bundle build and ``vendored`` for a
self-contained build.

Running the Examples
--------------------

The programs quoted in :doc:`usage` are examples of the ``eckit`` crate, in
``rust/crates/eckit/examples``. From a checkout of the repository:

.. code-block:: bash

   cd rust
   cargo run -p eckit --example quickstart
   cargo run -p eckit --example search

Cargo Features
--------------

Besides ``vendored`` and ``system``, the ``eckit`` crate has ``geo``, on by
default, which adds the ``eckit::geo`` module.

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
       ``geo-area-shapefile``, ``proj``
     - off
   * - Other
     - ``rados``, ``jemalloc``, ``rsync``, ``convex-hull``, ``experimental``,
       ``sandbox``
     - off

Features that are off by default need the corresponding external library to
be installed. To switch one on, add ``eckit-sys`` as a dependency next to
``eckit`` and enable the feature there:

.. code-block:: bash

   cargo add eckit
   cargo add eckit-sys --features geo-caching,lz4
