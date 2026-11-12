.. SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
.. SPDX-License-Identifier: Apache-2.0

Using the ``eckit`` Crate
=========================

The code on this page is quoted from the examples of the ``eckit`` crate;
:ref:`Running the Examples` says how to run them. The methods of each type
are listed on `docs.rs <https://docs.rs/eckit>`_.

Runtime Initialisation
----------------------

Call ``eckit::init()`` before any other eckit call:

.. code-block:: rust

   fn main() -> eckit::Result<()> {
       env_logger::init();
       eckit::init();
       // ...
       Ok(())
   }

Calling it again, from the application or from a library, has no effect, and
threads created later log through the same targets. ``Log::error``,
``warning``, ``info`` and ``debug`` arrive at the ``log`` level of the same
name with the program name as target, ``Log::metrics`` at ``Trace`` and a
library's own debug channel at ``Debug`` with the library name as target, so
``RUST_LOG=eckit=debug`` switches on one library's debug output.

Errors
------

Every fallible call returns ``eckit::Result<T>``, an alias for
``Result<T, eckit::Error>``. ``eckit::Error`` has one variant per eckit
exception class, so match on the class:

.. literalinclude:: ../../../rust/crates/eckit/examples/errors.rs
   :language: rust
   :start-after: // [match]
   :end-before: // [end match]
   :dedent:

``eckit::Error`` implements ``std::error::Error``. Its ``Display`` output
keeps the class name, for example ``eckit::CantOpenFile: ...``.
``eckit::Error::Other`` holds exceptions that are not eckit classes and
errors raised on the Rust side, such as a path that is not valid UTF-8.

Reading and writing through the ``std::io`` traits returns ``std::io::Error``
instead, with the class name kept in the message:

.. list-table::
   :widths: 60 40
   :header-rows: 1

   * - eckit exception
     - ``std::io::ErrorKind``
   * - ``NotImplemented``, ``FunctionalityNotSupported``
     - ``Unsupported``
   * - ``ShortFile``
     - ``UnexpectedEof``
   * - ``TimeOut``
     - ``TimedOut``
   * - ``OutOfMemory``
     - ``OutOfMemory``
   * - ``OutOfRange``, ``BadParameter``, ``BadValue``
     - ``InvalidInput``
   * - anything else
     - ``Other``

A short read raises ``ReadError``, so it arrives as ``Other``:

.. literalinclude:: ../../../rust/crates/eckit/examples/errors.rs
   :language: rust
   :start-after: // [io]
   :end-before: // [end io]
   :dedent:

Configuration
-------------

A ``Config`` is loaded from a YAML file or parsed from a string and read with
``get``, whose second argument is the default and selects the type:
``String``, ``i64``, ``i32``, ``bool``, ``f64`` or ``Vec<String>``.
``subs(Some(key))`` iterates a list under a key, ``subs(None)`` a document
whose root is a list and ``sub(key)`` returns one nested configuration:

.. literalinclude:: ../../../rust/crates/eckit/examples/config.rs
   :language: rust
   :start-after: // [read]
   :end-before: // [end read]
   :dedent:

.. literalinclude:: ../../../rust/crates/eckit/examples/config.rs
   :language: rust
   :start-after: // [parse]
   :end-before: // [end parse]
   :dedent:

``set`` accepts the scalar types and returns ``&mut Self``:

.. literalinclude:: ../../../rust/crates/eckit/examples/config.rs
   :language: rust
   :start-after: // [set]
   :end-before: // [end set]
   :dedent:

Data Handles
------------

A handle is opened either for reading or for writing, and the type says
which: opening consumes the ``DataHandle`` and returns a
``DataHandle<Reading>`` or ``DataHandle<Writing>``. ``close()`` reports flush
errors;
dropping the handle discards them:

.. literalinclude:: ../../../rust/crates/eckit/examples/handles.rs
   :language: rust
   :start-after: // [copy]
   :end-before: // [end copy]
   :dedent:

A reading handle implements ``Read`` and ``Seek``, a writing one ``Write``:

.. literalinclude:: ../../../rust/crates/eckit/examples/handles.rs
   :language: rust
   :start-after: // [read]
   :end-before: // [end read]
   :dedent:

``from_reader`` and ``from_writer`` go the other way: eckit reads from or
writes to a Rust ``Read + Seek`` or ``Write`` value, for example an object
store client, without staging the data on disk. Closing a ``from_writer``
handle flushes the sink:

.. literalinclude:: ../../../rust/crates/eckit/examples/handles.rs
   :language: rust
   :start-after: // [writer]
   :end-before: // [end writer]
   :dedent:

Messages
--------

``MessageReader`` iterates the messages in a handle open for reading;
``get`` reads a key as ``String``, ``i64`` or ``f64``, ``data()`` returns the
encoded bytes and ``write_to()`` copies the message to a handle open for
writing:

.. literalinclude:: ../../../rust/crates/eckit/examples/messages.rs
   :language: rust
   :start-after: // [read]
   :end-before: // [end read]
   :dedent:

.. note::

   EcKit defines the message abstraction only. A library that can decode the
   format, metkit for GRIB and BUFR, must be linked into the program and
   called at least once. With eckit alone, ``MessageReader::new`` fails with
   ``eckit::Error::AssertionFailed``.

Streams
-------

``eckit::Stream`` is the tagged binary encoding eckit uses for object
serialisation and network protocols. In Rust it is the ``Stream`` trait, with
``TcpStream`` for a connection and ``MemoryStream`` for a buffer:

.. literalinclude:: ../../../rust/crates/eckit/examples/streams.rs
   :language: rust
   :start-after: // [roundtrip]
   :end-before: // [end roundtrip]
   :dedent:

The bytes are what C++ reads and writes through ``eckit::Stream``. The
encoding is tagged, so the integer width must match the C++ overload used on
the other side:

.. list-table::
   :widths: 30 70
   :header-rows: 1

   * - Rust
     - ``eckit::Stream`` overload
   * - ``i32``
     - ``int``
   * - ``u32``
     - ``unsigned long`` (4 bytes on the wire)
   * - ``i64``
     - ``long long``
   * - ``u64``
     - ``unsigned long long``

``start_object()``, ``end_object()`` and ``next_object()`` write and scan the
object tags used by ``eckit::Streamable`` classes.

Application types describe their own encoding by implementing ``StreamWrite``
and ``StreamRead`` against ``&mut dyn Stream``:

.. literalinclude:: ../../../rust/crates/eckit/examples/streams.rs
   :language: rust
   :start-after: // [types]
   :end-before: // [end types]
   :dedent:

.. literalinclude:: ../../../rust/crates/eckit/examples/streams.rs
   :language: rust
   :start-after: // [typed]
   :end-before: // [end typed]
   :dedent:

A ``MemoryStream``'s buffer is plain bytes, so a stream can go through any
Rust writer and come back through any Rust reader, a file here:

.. literalinclude:: ../../../rust/crates/eckit/examples/streams.rs
   :language: rust
   :start-after: // [file]
   :end-before: // [end file]
   :dedent:

A ``TcpStream`` can be handed over as a ``DataHandle`` with ``try_from``,
after which the connection is read through ``std::io::Read`` instead.

Grids
-----

A grid is identified by a name or by its *gridSpec*, a YAML or JSON mapping:

.. literalinclude:: ../../../rust/crates/eckit/examples/grids.rs
   :language: rust
   :start-after: // [grids]
   :end-before: // [end grids]
   :dedent:

``from_spec`` also accepts a bare name such as ``O1280`` and reads it as
``{"grid":"O1280"}``; ``from_name`` accepts only a name. ``spec()`` returns
the canonical form, which can differ from the input, and two grids compare
equal when their canonical specs match:

.. literalinclude:: ../../../rust/crates/eckit/examples/grids.rs
   :language: rust
   :start-after: // [canonical]
   :end-before: // [end canonical]
   :dedent:

Points are the ``Point`` enum: ``LonLat`` in degrees or ``LonLatR`` in
radians for geographic grids, ``Xy`` or ``Xyz`` for projected and Cartesian
ones. For large grids prefer ``fill_latlons``, which writes into buffers the
caller owns; ``to_latlons`` allocates two vectors on every call:

.. literalinclude:: ../../../rust/crates/eckit/examples/grids.rs
   :language: rust
   :start-after: // [latlons]
   :end-before: // [end latlons]
   :dedent:

``fill_latlons`` fails with ``eckit::Error::GridError`` on a grid whose points are
not geographic; ``fill_xyz`` gives geocentric coordinates for every grid.

The ``grid_info`` example prints everything ``eckit::geo`` reports about a
grid:

.. code-block:: bash

   cd rust
   cargo run -p eckit --example grid_info -- N320
   cargo run -p eckit --example grid_info -- '{"area":[73,-27,33,45],"grid":[4,4]}'
   cargo run -p eckit --example grid_info -- '{"grid":[5,5],"rotation":[-40,10]}'

Cropping
^^^^^^^^

A ``BoundingBox`` is the area between two parallels and two meridians.
``crop`` returns the indices of the grid points inside it, ``cropped`` a grid
of those points:

.. literalinclude:: ../../../rust/crates/eckit/examples/areas.rs
   :language: rust
   :start-after: // [crop]
   :end-before: // [end crop]
   :dedent:

``cropped`` keeps the grid type when the area is a bounding box on a
geographic grid and returns an unstructured grid otherwise. An empty
intersection is an ``eckit::Error::GridError``. Polygon areas are built with
``Area::from_spec``; shapefile areas need ``eckit-sys`` built with
``geo-area-shapefile``.

Coordinate Systems
^^^^^^^^^^^^^^^^^^

``grid.projection()`` is the ``Projection`` between the grid's geographic
coordinates and the coordinates its points are in: the identity for a plain
lon/lat grid, a rotation, a Lambert conformal conic and so on. ``fwd`` and
``inv`` project one ``Point``; ``fwd_vectors`` and ``inv_vectors`` project
one slice per coordinate:

.. literalinclude:: ../../../rust/crates/eckit/examples/projection.rs
   :language: rust
   :start-after: // [projection]
   :end-before: // [end projection]
   :dedent:

``grid.figure()`` is the sphere or spheroid the points are on:

.. literalinclude:: ../../../rust/crates/eckit/examples/projection.rs
   :language: rust
   :start-after: // [figure]
   :end-before: // [end figure]
   :dedent:

Point Searches
^^^^^^^^^^^^^^

``grid.search()`` answers nearest-neighbour queries on the grid points.
Queries are ``Point::LonLat`` in degrees or ``Point::Xyz`` geocentric;
results are ``Neighbour`` values with the point's ``index``, its chord
``distance`` in metres and its geocentric ``point``, ordered by distance:

.. literalinclude:: ../../../rust/crates/eckit/examples/search.rs
   :language: rust
   :start-after: // [search]
   :end-before: // [end search]
   :dedent:

``search_with_spec`` configures the search from a spec (``search``,
``search-k``, ``search-radius``, ``search-tolerance``, ``search-fast-build``)
and ``Search::search`` runs it; the k-d tree loader is the library's
(``ECKIT_GEO_SEARCH_LOADER``). Searches are shared in a process-wide cache by
grid and configuration; ``search_cache_statistics()`` reports on it and
``search_cache_clear()`` empties it.

Grid Caches
^^^^^^^^^^^

The memory caches of ``eckit::geo`` are exposed as
``eckit::geo::cache_footprint()`` and ``eckit::geo::cache_purge()``. Do not
purge while another thread is inside a grid call.

Searches, crops and the coordinates of unstructured grids
(``Grid::from_latlons``, ``to_unstructured_ll``, ``cropped_unstructured_ll``,
cropping of projected grids, polygon areas) are kept in memory, and also in
cache files when caching is enabled, which is off by default.
``ECKIT_GEO_CACHING=1`` enables it (the ``geo-caching`` feature of
``eckit-sys`` changes the default), and ``ECKIT_GEO_CACHE_PATH``
(``path1:path2:...``, searched in order, written to the first writable) moves
it from ``~/.cache/eckit/geo``. Without caching, existing cache files (e.g. a
provisioned read-only cache) are still found, but nothing is written and
misses are computed in memory; ``ECKIT_GEO_CACHE_THROW_ON_MISS=1`` makes a
missing search or crop cache file an error instead. The cached coordinates
are LZ4-compressed, which needs ``eckit-sys`` built with ``lz4`` unless
``ECKIT_CODEC_COMPRESSION`` is set empty; otherwise writing them fails with
``No CompressorBuilder called lz4``.

Threads
-------

``Config`` is ``Send`` and ``Sync``.

``DataHandle``, ``TcpStream``, ``MemoryStream``, ``Grid``, ``Area`` and
``Search`` are ``Send`` but not ``Sync``. A ``Grid`` can also be rebuilt per
thread from its gridSpec.

``Message``, ``MessageReader``, ``Projection``, ``Figure`` and ``Spec`` are
neither.

Building the same unstructured grid from several threads at once is a race;
build it once and pass the ``Grid`` around.
