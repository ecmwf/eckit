.. SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
.. SPDX-License-Identifier: Apache-2.0

Using the ``eckit`` Crate
=========================

Every fallible call in the high-level crate returns ``eckit::Result<T>``, an
alias for ``Result<T, eckit::Error>``; see :doc:`approach` for how the error
type relates to eckit exceptions.

Runtime Initialisation
----------------------

Call ``eckit::init()`` once, before anything else:

.. code-block:: rust

   fn main() -> eckit::Result<()> {
       env_logger::init();   // or any other `log` backend
       eckit::init();
       // ...
       Ok(())
   }

It sets up the eckit runtime and routes ``eckit::Log`` output to the Rust
``log`` crate. Repeated calls are harmless, so a library can call it without
knowing whether the application already has.

Configuration Access
--------------------

``Config`` wraps ``eckit::LocalConfiguration``.

.. code-block:: rust

   use eckit::Config;

   let cfg = Config::from_path("config.yaml")?;
   let cfg: Config = "{name: fdb, port: 9000}".parse()?;   // from a YAML string

   let name: String = cfg.get("name", String::new())?;
   let port: i64 = cfg.get("port", 9000)?;

   for sub in cfg.subs(Some("databases"))? {
       let sub = sub?;
       let class: String = sub.get("class", String::new())?;
   }

``get`` takes the default as its second argument, and the type of the default
selects the C++ getter. Supported types are ``String``, ``i64``, ``i32``,
``bool``, ``f64`` and ``Vec<String>``. ``set`` accepts the scalar types and
returns ``&mut Self`` for chaining:

.. code-block:: rust

   let mut cfg = Config::new();
   cfg.set("host", "localhost").set("port", 9000_i64);

``sub(key)`` returns one nested configuration. ``subs(Some(key))`` iterates a
list under a key, and ``subs(None)`` iterates a document whose root is a list.

Data Handles
------------

``DataHandle`` wraps ``eckit::DataHandle``. A C++ handle is opened either for
reading or for writing, and using it in the wrong mode is a run-time error. In
Rust the mode is part of the type, so the mistake does not compile:

.. list-table::
   :widths: 35 65
   :header-rows: 1

   * - Type
     - Available operations
   * - ``DataHandle<Closed>``
     - ``open_for_read()``, ``open_for_write()``
   * - ``DataHandle<Reading>``
     - ``std::io::Read``, ``std::io::Seek``, ``save_into()``, ``close()``
   * - ``DataHandle<Writing>``
     - ``std::io::Write``, ``close()``

Opening consumes the closed handle and returns the opened one:

.. code-block:: rust

   use std::io::Read;
   use eckit::DataHandle;

   let (mut input, length) = DataHandle::from_path("in.grib")?.open_for_read()?;
   let mut output = DataHandle::from_path("out.grib")?.open_for_write(length)?;

   input.save_into(&mut output)?;
   output.close()?;        // reports flush errors; dropping would discard them

Because an open handle implements the ``std::io`` traits, it can be passed to
any Rust code that takes a reader or writer.

.. list-table::
   :widths: 40 60
   :header-rows: 1

   * - Constructor
     - C++ equivalent
   * - ``DataHandle::from_path(path)``
     - ``FileHandle``
   * - ``DataHandle::from_part(path, offset, length)``
     - ``PartFileHandle``
   * - ``DataHandle::from_buffer(bytes)``
     - ``MemoryHandle`` (the bytes are copied)
   * - ``DataHandle::from_multi(paths)``
     - ``MultiHandle``, reads the files in sequence
   * - ``DataHandle::tee(paths)``
     - ``TeeHandle``, one write goes to every file
   * - ``DataHandle::from_reader(reader)``
     - A handle that calls back into a Rust ``Read + Seek`` value
   * - ``DataHandle::try_from(tcp_stream)``
     - The connection of a ``TcpStream``, read on demand

``from_reader`` lets C++ code consume a Rust data source, for example an object
store client, without staging the data in memory or on disk.

Messages
--------

``MessageReader`` iterates the messages found in a handle that is open for
reading, and ``Message`` gives typed access to their metadata:

.. code-block:: rust

   use eckit::{DataHandle, MessageReader};

   let (mut handle, _) = DataHandle::from_path("fields.grib")?.open_for_read()?;

   for msg in MessageReader::new(&mut handle)? {
       let msg = msg?;
       let param: String = msg.get("shortName")?;
       println!("{param}: {} bytes at offset {}", msg.length(), msg.offset());
   }

``get`` reads a key as ``String``, ``i64`` or ``f64``. ``data()`` returns the
encoded bytes and ``write_to()`` copies the message to a handle open for
writing.

The reader borrows the handle, so the handle cannot be dropped or used
elsewhere while the reader is alive. This mirrors the C++ ``Reader``, which
keeps a reference to its ``DataHandle``.

.. note::

   EcKit defines the message abstraction only. Recognising and decoding a
   format such as GRIB or BUFR is done by a library that registers the
   corresponding splitter and decoder, for example metkit. Such a library must
   be linked into the program for the reader to find any messages.

Streams
-------

``Stream`` is a trait over ``eckit::Stream``, the tagged binary encoding eckit
uses for object serialisation and network protocols. Two implementations are
provided: ``TcpStream`` for a connection and ``MemoryStream`` for a buffer.

.. code-block:: rust

   use eckit::{MemoryStream, Stream};

   let mut out = MemoryStream::writer();
   out.write_string("retrieve")?;
   out.write_i64(42)?;

   let bytes = out.buffer()?.to_vec();

   let mut input = MemoryStream::reader(&bytes);
   assert_eq!(input.read_string()?, "retrieve");
   assert_eq!(input.read_i64()?, 42);

The bytes are compatible with what C++ reads and writes through
``eckit::Stream``. Because the encoding is tagged, the integer width must match
the C++ overload used on the other side:

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

The ``StreamWrite`` and ``StreamRead`` traits are implemented
for the primitive types and can be implemented for application types to
describe how they are encoded. Protocol code written against ``&mut dyn
Stream`` can be tested with a ``MemoryStream`` and run over a ``TcpStream``.

Grids
-----

The ``eckit::geo`` module is available with the ``geo`` Cargo feature. A grid
is identified by its *gridSpec*, a YAML or JSON mapping:

.. code-block:: rust

   use eckit::Grid;

   let grid = Grid::from_name("O1280")?;                                    // {"grid":"O1280"}
   let area = Grid::from_spec(r#"{"area":[73,-27,33,45],"grid":[4,4]}"#)?;

   println!("{}", grid.grid_type()?);           // reduced_gg
   println!("{}", grid.len()?);                 // 6599680
   println!("{:?}", grid.bounding_box()?);

   let spec = grid.spec();
   assert_eq!(spec.to_json()?, r#"{"grid":"O1280"}"#);
   assert_eq!(spec.get::<String>("grid")?, "O1280");

``from_spec`` requires a mapping. A bare name such as ``O1280`` is not a
gridSpec and is rejected with ``Error::SpecError``; use ``from_name`` for
names. ``spec()`` returns the canonical form, which may differ from the input
(``o16`` comes back as ``O16``).

.. list-table::
   :widths: 40 60
   :header-rows: 1

   * - Method
     - Returns
   * - ``spec()``, ``uid()``, ``grid_type()``, ``catalog()``
     - Identity of the grid
   * - ``len()``, ``is_empty()``, ``shape()``
     - Number of points; ``[ny, nx]`` for regular grids, ``[size]`` for reduced
   * - ``pl()``, ``ny()``, ``is_structured()``
     - Points per latitude row; empty and ``0`` for grids without rows
   * - ``bounding_box()``, ``first_point()``, ``last_point()``
     - Extent, and the end points in iteration order
   * - ``distinct_latitudes()``, ``distinct_longitudes()``
     - Coordinate axes, where the grid has them
   * - ``fill_latlons()``, ``to_latlons()``
     - The coordinates of every point

``first_point()`` and ``last_point()`` return a ``Point``, an enum that mirrors
the ``eckit::geo::Point`` variant: ``LonLat``, ``LonLatR``, ``Xy`` or ``Xyz``,
depending on the coordinate system of the grid.

For large grids prefer ``fill_latlons``, which writes into buffers the caller
owns and can reuse. ``to_latlons`` allocates two new vectors on every call, and
O1280 has 6.6 million points.

.. code-block:: rust

   let n = grid.len()?;
   let mut lat = vec![0.0; n];
   let mut lon = vec![0.0; n];
   grid.fill_latlons(&mut lat, &mut lon)?;

The ``grid_info`` example prints everything ``eckit::geo`` knows about a grid:

.. code-block:: bash

   cd rust
   cargo run -p eckit --example grid_info -- N320
   cargo run -p eckit --example grid_info -- '{"area":[73,-27,33,45],"grid":[4,4]}'

Grids and Threads
^^^^^^^^^^^^^^^^^

A ``Grid`` can be moved to another thread but not shared between threads: it is
``Send`` and not ``Sync``. The C++ object fills internal caches from ``const``
methods without locking. Either build one grid per thread from the gridSpec, or
put the grid behind a ``Mutex``.

Grid Caches
^^^^^^^^^^^

``eckit::geo`` keeps process-wide caches (Gaussian latitudes, HEALPix tables,
downloaded grid data) that grow as distinct grids are built and are never
evicted. A long-running service should monitor ``eckit::geo::cache_footprint()``
and call ``eckit::geo::cache_purge()`` when needed. Existing grids keep working
after a purge and recompute on next use. Do not purge while another thread is
inside a grid call.
