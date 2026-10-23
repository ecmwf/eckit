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

The ``eckit::geo`` module is available with the ``geo`` Cargo feature, on by
default. A grid is identified by its *gridSpec*, a YAML or JSON mapping:

.. code-block:: rust

   use eckit::Grid;

   let grid = Grid::from_name("O1280")?;                                    // {"grid":"O1280"}
   let area = Grid::from_spec(r#"{"area":[73,-27,33,45],"grid":[4,4]}"#)?;
   let rotated = Grid::from_spec(r#"{"grid":[5,5],"rotation":[-40,10]}"#)?;

   println!("{}", grid.grid_type()?);           // reduced_gg
   println!("{}", grid.len()?);                 // 6599680
   println!("{:?}", grid.bounding_box()?);

   let spec = grid.spec();
   assert_eq!(spec.to_json_str()?, r#"{"grid":"O1280"}"#);
   assert_eq!(spec.get::<String>("grid")?, "O1280");

``from_spec`` also accepts a bare name such as ``O1280`` and reads it as
``{"grid":"O1280"}``; ``from_name`` checks that the argument is a name (no
spaces, braces or quotes) before doing the same. ``spec()`` returns the
canonical form, which can differ from the input: ``o16`` comes back as
``O16``. Two grids compare equal when their canonical specs match, however
they were described.

.. list-table::
   :widths: 40 60
   :header-rows: 1

   * - Method
     - Returns
   * - ``spec()``, ``uid()``, ``grid_type()``, ``name()``, ``catalog()``
     - Identity of the grid; ``catalog()`` is a ``Spec`` too, carrying the data
       URL for grids shipped as files (ORCA, FESOM, ICON)
   * - ``len()``, ``is_empty()``, ``shape()``, ``is_regular()``
     - Number of points; ``[ny, nx]`` for regular grids, ``[size]`` for reduced
   * - ``pl()``, ``ny()``, ``nx()``, ``is_structured()``
     - Points per latitude row; empty and ``0`` for grids without rows
   * - ``order()``, ``reorder(to)``
     - Scanning order, e.g. ``i+j-``, and the renumbering into another order
   * - ``bounding_box()``, ``first_point()``, ``last_point()``
     - Extent as a ``BoundingBox``, and the end points in iteration order
   * - ``distinct_latitudes()``, ``distinct_longitudes()``, ``x_values()``,
       ``y_values()``
     - Coordinate axes, where the grid has them
   * - ``fill_latlons()``, ``to_latlons()``, ``fill_xyz()``, ``to_xyz()``,
       ``to_points()``
     - The coordinates of every point
   * - ``projection()``, ``figure()``
     - The coordinate system the points are in
   * - ``crop(area)``, ``cropped(area)``, ``cropped_unstructured_ll(area)``
     - The points in an area, as indices or as a new grid
   * - ``search()``, ``search_with_spec(spec)``
     - A nearest-neighbour search on the points

``first_point()``, ``last_point()`` and ``to_points()`` return ``Point``, an
enum that mirrors the ``eckit::geo::Point`` variant: ``LonLat``, ``LonLatR``,
``Xy`` or ``Xyz``, depending on the coordinate system of the grid.
``point_coordinates()`` names the coordinates, ``["longitude", "latitude"]``
or ``["x", "y"]``.

For large grids prefer ``fill_latlons``, which writes into buffers the caller
owns and can reuse. ``to_latlons`` allocates two new vectors on every call, and
O1280 has 6.6 million points. ``fill_latlons`` fails with ``Error::GridError``
on a grid whose points are not geographic; ``fill_xyz`` gives geocentric
coordinates for every grid.

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
   cargo run -p eckit --example grid_info -- '{"grid":[5,5],"rotation":[-40,10]}'

Areas and Cropping
^^^^^^^^^^^^^^^^^^

An ``Area`` selects grid points. ``BoundingBox`` is the area between two
parallels and two meridians; polygons and shapefile features are built through
``Area::from_spec``.

.. code-block:: rust

   use eckit::geo::{Area, BoundingBox};

   let bbox = BoundingBox::new(73.0, -27.0, 33.0, 45.0)?;      // north, west, south, east
   let polygon = Area::from_spec(
       r#"{"type":"polygon","latitudes":[60,60,40,40],"longitudes":[-10,30,30,-10]}"#,
   )?;

   let indices: Vec<usize> = grid.crop(&bbox)?;     // points inside, in cropped-grid order
   let europe = grid.cropped(&bbox)?;               // a grid of those points

``cropped`` keeps the grid type when the area is a bounding box on a geographic
grid, and returns an unstructured grid otherwise. An empty intersection is an
``Error::GridError``. Both areas and bounding boxes compare equal by uid, and
report ``contains(lon, lat)``, ``intersects`` and ``surface_area()``.

Projections and Figures
^^^^^^^^^^^^^^^^^^^^^^^

``grid.projection()`` is the ``Projection`` between the grid's geographic
coordinates and the coordinates its points are in: the identity for a plain
lon/lat grid, a rotation, a Lambert conformal conic, and so on. ``fwd`` and
``inv`` project one ``Point``; ``fwd_vectors`` and ``inv_vectors`` project
one slice per coordinate.

.. code-block:: rust

   use eckit::geo::Point;

   let rotation = rotated.projection();
   assert_eq!(rotation.projection_type()?, "rotation");

   // the rotated sphere's south pole, back on the unrotated sphere
   let south_pole = rotation.inv(Point::LonLat { lon: 0.0, lat: -90.0 })?;
   let rotated_coords = rotation.fwd_vectors(&[&[0.0, 10.0], &[0.0, 20.0]])?;

``grid.figure()`` is the sphere or spheroid the points are on, with
``radius()``, the semi-axes ``a()`` and ``b()``, ``area()``, ``eccentricity()``
and ``flattening()``. Both carry a ``spec()`` and a ``proj_str()`` for the
types with a PROJ equivalent. The two are borrowed from the grid and live as
long as it does.

Point Searches
^^^^^^^^^^^^^^

``grid.search()`` builds a k-d tree over the grid points and answers nearest
neighbour queries. Queries are ``Point::LonLat`` in degrees or ``Point::Xyz``
geocentric; results are a ``Neighbour`` with the point's ``index``, its chord
``distance`` in metres and its geocentric ``point``, ordered by distance.

.. code-block:: rust

   use eckit::geo::Point;

   let search = grid.search()?;
   let london = Point::LonLat { lon: -0.1, lat: 51.5 };

   let nearest = search.nearest(london)?;
   let four = search.nearest_k(london, 4)?;
   let within = search.within_radius(london, 50_000.0)?;

``search_with_spec`` configures the search from a spec (``search``,
``search-k``, ``search-radius`` and the k-d tree ``search-loader``), so that
``Search::search`` runs the configured query. Searches are shared in a
process-wide cache by grid and configuration; ``search_cache_statistics()``
reports hits, misses and memory, and ``search_cache_clear()`` empties it.

Grids and Threads
^^^^^^^^^^^^^^^^^

``Grid``, ``Area`` and ``Search`` can be moved to another thread but not shared
between threads: they are ``Send`` and not ``Sync``. The C++ objects fill
internal caches from ``const`` methods without locking. Either build one grid
per thread from the gridSpec, or put the grid behind a ``Mutex``.

Grid Caches
^^^^^^^^^^^

``eckit::geo`` keeps process-wide memory caches (Gaussian latitudes, HEALPix
tables, downloaded grid data) that grow as distinct grids are built and are
never evicted. A long-running service should monitor
``eckit::geo::cache_footprint()`` and call ``eckit::geo::cache_purge()`` when
needed. Existing grids keep working after a purge and recompute on next use.
Do not purge while another thread is inside a grid call.

Unstructured grids are a separate case. ``Grid::from_latlons``,
``to_unstructured_ll``, ``cropped_unstructured_ll``, the cropping of projected
grids and polygon areas keep their coordinates in eckit geo's *disk* cache,
which is off unless enabled:

.. list-table::
   :widths: 30 35 35
   :header-rows: 1

   * - Setting
     - Resource
     - Environment variable
   * - Enable the cache (default: the ``geo-caching`` feature of ``eckit-sys``)
     - ``eckit-geo-caching``
     - ``ECKIT_GEO_CACHING``
   * - Cache directory (default ``~/.cache/eckit/geo``)
     - ``eckit-geo-cache-path``
     - ``ECKIT_GEO_CACHE_PATH``
   * - Record compression (default ``lz4``)
     - ``eckit-codec-compression``
     - ``ECKIT_CODEC_COMPRESSION``

Without the cache those grids fail to build with ``CacheManager cannot create
key``. The records are LZ4-compressed by default, which needs eckit built with
the ``lz4`` feature of ``eckit-sys``; otherwise set the compression resource
empty, or the build fails with ``No CompressorBuilder called lz4``.

eckit registers every unstructured grid by uid in a process-wide registry as it
is built. Building grids of the same points from several threads at once races
on that registry, so build a shared unstructured grid once and pass the
``Grid`` around.
