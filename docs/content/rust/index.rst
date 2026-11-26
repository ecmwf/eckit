.. SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
.. SPDX-License-Identifier: Apache-2.0

Rust Interface
==============

The Rust interface lives in the ``rust/`` directory of the eckit repository
and calls the eckit C++ library. This chapter is for developers writing
applications against the crates.

``eckit`` is the crate applications depend on; it gives the C++ library a
Rust API, with an ``eckit::Error`` enum to match on and the ``std::io``
traits on data handles. ``eckit-sys`` builds or links the C++ library
underneath it.

.. note::

   Use ``eckit`` in your code. ``eckit-sys`` goes into a ``Cargo.toml`` only
   to switch on a build feature of the C++ library (:doc:`building`); calling
   it directly means handling ``cxx`` pointers and raw exception strings
   yourself.

Coverage
--------

.. list-table::
   :widths: 30 35 35
   :header-rows: 1

   * - Rust
     - C++
     - Notes
   * - ``eckit::init``
     - ``eckit::Main``
     - Runtime initialisation and logging
   * - ``eckit::Error``
     - ``eckit::Exception`` and subclasses
     - One enum variant per exception class
   * - ``eckit::Config``
     - ``eckit::LocalConfiguration``
     - YAML loading, typed access, sub-configurations
   * - ``eckit::DataHandle``
     - ``eckit::DataHandle``
     - Files, buffers, byte ranges and Rust readers or writers
   * - ``eckit::Message``, ``eckit::MessageReader``
     - ``eckit::message::Message``, ``eckit::message::Reader``
     - Iterate the messages in a data handle
   * - ``eckit::Stream``, ``TcpStream``, ``MemoryStream``
     - ``eckit::Stream`` and subclasses
     - Tagged binary serialisation
   * - ``eckit::geo::Grid``, ``eckit::geo::Spec``
     - ``eckit::geo::Grid``, ``eckit::spec::Spec``
     - Grids by gridSpec; feature ``geo``, on by default
   * - ``eckit::geo::Area``, ``eckit::geo::BoundingBox``
     - ``eckit::geo::Area``, ``eckit::geo::area::BoundingBox``
     - Select and crop grid points
   * - ``eckit::geo::Projection``, ``eckit::geo::Figure``
     - ``eckit::geo::Projection``, ``eckit::geo::Figure``
     - The coordinate system of a grid's points
   * - ``eckit::geo::Search``
     - ``eckit::geo::Search``
     - Nearest-neighbour queries on grid points

Quick Start
-----------

Add the crate:

.. code-block:: bash

   cargo add eckit

The crate version is the version of the eckit release it wraps.
:doc:`building` lists the requirements and the alternative of linking an
installed eckit.

.. literalinclude:: ../../../rust/crates/eckit/examples/quickstart.rs
   :language: rust
   :start-after: // [main]
   :end-before: // [end main]

.. toctree::
   :maxdepth: 2
   :caption: Contents

   approach
   usage
   building

The per-item API reference is on docs.rs: `eckit <https://docs.rs/eckit>`_ and
`eckit-sys <https://docs.rs/eckit-sys>`_.
