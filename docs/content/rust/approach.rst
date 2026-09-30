.. SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
.. SPDX-License-Identifier: Apache-2.0

Approach
========

The Rust interface calls the C++ library; no eckit code is ported to Rust.
Every call ends up in the same eckit code that C++ applications run, so
behaviour, file formats and configuration stay identical across languages. The
design questions are all about the boundary: how the C++ library is built, how its types are
exposed, and how eckit concepts that span several packages (exceptions,
logging, data handles) keep working when each package has its own Rust
bindings.

High-Level and Low-Level Crates
-------------------------------

The interface follows the Rust convention of a ``-sys`` crate paired with a
safe wrapper crate.

.. mermaid::

   flowchart TD
       app["Rust application"]
       eckit["eckit<br/>safe API"]
       sys["eckit-sys<br/>cxx bridge, generated errors, build script"]
       shim["C++ shims<br/>namespace eckit_bridge"]
       cpp["eckit C++ library"]
       other["other -sys crates<br/>bindings for packages built on eckit"]

       app --> eckit
       eckit --> sys
       other --> sys
       sys --> shim
       shim --> cpp

``eckit-sys`` (low level)
   Owns everything that touches C++:

   * a build script that compiles the eckit sources, or finds an installed
     eckit, and emits the link directives;
   * the `cxx <https://cxx.rs>`_ bridge declaring the functions Rust may call;
   * thin C++ wrapper classes (``rust/crates/eckit-sys/cpp``) that adapt eckit
     signatures to types that can cross the bridge;
   * the ``Error`` enum generated from the eckit exception headers;
   * the log target that forwards ``eckit::Log`` to Rust.

   Its API mirrors the C++ signatures, with ``UniquePtr`` and ``Pin<&mut>``
   in place of ownership and borrowing. Application code should not need it.

``eckit`` (high level)
   Contains no C++ and no ``unsafe`` beyond thread-safety markers. It wraps the
   ``eckit-sys`` types in an API that follows Rust conventions:

   * fallible calls return ``eckit::Result<T>``;
   * ``DataHandle`` implements ``std::io::Read``, ``Seek`` and ``Write``;
   * ``MessageReader`` and ``Config::subs`` are iterators;
   * C++ lifetime rules are expressed as borrows, so the compiler rejects code
     that would leave a dangling reference on the C++ side;
   * rules that C++ checks at run time, such as a data handle being open for
     reading or for writing but never both, are moved into the type system.

A package that builds on eckit gets its own pair of crates, and its ``-sys`` crate depends on ``eckit-sys``, never on
the high-level ``eckit`` crate. This keeps a single copy of the C++ library,
one error vocabulary and one logging bridge in any program, however many
packages it pulls in.

The C++ Bridge
--------------

The bridge is written with ``cxx``, which generates matching declarations on
both sides from one Rust module and checks the signatures at compile time.
``cxx`` can only pass a restricted set of types, so eckit classes are not
bound directly. Each one is held by a small wrapper class in the
``eckit_bridge`` namespace:

.. list-table::
   :widths: 35 65
   :header-rows: 1

   * - Wrapper
     - Wraps
   * - ``ConfigWrapper``
     - ``eckit::LocalConfiguration``
   * - ``DataHandleWrapper``
     - ``eckit::DataHandle``
   * - ``MessageWrapper``, ``ReaderWrapper``
     - ``eckit::message::Message``, ``eckit::message::Reader``
   * - ``StreamWrapper``
     - ``eckit::Stream``
   * - ``GridWrapper``, ``SpecWrapper``
     - ``eckit::geo::Grid``, ``eckit::spec::Spec``
   * - ``RustMain``, ``RustLogTarget``
     - ``eckit::Main``, ``eckit::LogTarget``

Rust owns a wrapper through ``cxx::UniquePtr``, so the C++ object is destroyed
when the Rust value is dropped.

``eckit::geo`` has a bridge module of its own (``eckit_sys::geo``). ``cxx``
cannot switch individual bridge items on a Cargo feature, so each optional part
of eckit is a separate bridge that is compiled only when its feature is on.

The bridge also records which C++ classes it depends on. The build script
checks that list against the eckit headers it is compiling with, so a change to
the C++ API is reported at build time instead of surfacing as a link error.

Error Handling Across the Language Boundary
-------------------------------------------

EcKit reports errors by throwing subclasses of ``eckit::Exception``. Rust has
no exceptions, and a C++ exception must never unwind into Rust code. The
interface converts every exception into a value of one ``Error`` enum, and
keeps the exception *class* so that Rust code can match on it.

The enum is not written by hand. The build script of ``eckit-sys`` parses
``eckit/exception/Exceptions.h`` (and the ``eckit::spec`` and ``eckit::geo``
exception headers when ``geo`` is enabled) and generates two files:

``eckit_exceptions.h``
   A handler that ``cxx`` invokes around every bridged call. It has one
   ``catch`` block per exception class, and each one prefixes the message with
   the qualified class name:

   .. code-block:: c++

      catch (const eckit::CantOpenFile& e) {
          fail(("eckit::CantOpenFile: " + std::string(e.what())).c_str());
      }

   Anything else derived from ``std::exception`` is forwarded with its message
   unchanged.

``eckit_exceptions.rs``
   The ``Error`` enum with one variant per class, and the conversion that reads
   the prefix back:

   .. code-block:: rust

      pub enum Error {
          CantOpenFile(String),
          NotImplemented(String),
          // ... one variant per eckit exception class
          SpecError(String),
          Other(String),
      }

A new exception class added to eckit therefore becomes a new ``Error`` variant
the next time the crate is built, with no change to the bindings.
``Error::Other`` holds exceptions that match no known class, and errors raised
by the Rust wrappers themselves, such as a path that is not valid UTF-8.

From the application's point of view:

.. code-block:: rust

   use eckit::{DataHandle, Error};

   match DataHandle::from_path("field.grib")?.open_for_read() {
       Ok((handle, length)) => { /* ... */ }
       Err(Error::CantOpenFile(msg)) => eprintln!("missing input: {msg}"),
       Err(other) => return Err(other),
   }

``Error`` implements ``std::error::Error`` and ``Display``, so it works with
``?``, ``Box<dyn Error>`` and error-reporting crates. Its ``Display`` output
keeps the class name, for example ``eckit::CantOpenFile: ...``.

Errors in Packages That Build on EcKit
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Code in another package throws eckit exceptions as well as its own. If each set
of bindings only knew its own classes, an ``eckit::NotFound`` thrown from
inside another library would arrive in Rust as an untyped string.

``eckit-sys`` avoids this by publishing the list of exception headers it
parsed. The build script of a downstream ``-sys`` crate reads that list and
passes it to the same generator as *inherited* sources, which gives the
downstream handler the eckit ``catch`` blocks in addition to its own. On the
Rust side the downstream error type tries its own prefixes first and then
delegates to ``eckit_sys::Error::try_from_cxx``, which returns ``None`` for any
message that does not start with ``eckit::``. The eckit error is carried
through intact, whichever library threw it.

Two consequences for anyone writing such bindings:

* ``EckitBridge.h`` does not include ``eckit_exceptions.h``. A
  translation unit may contain only one handler, so each ``-sys`` crate
  includes its own generated header from its ``.cc`` files.
* The high-level crate of a downstream package can expose eckit errors as a
  variant of its own error type, because ``eckit::Error`` is the same type
  everywhere.

Errors and ``std::io``
^^^^^^^^^^^^^^^^^^^^^^

The ``Read``, ``Seek`` and ``Write`` implementations on ``DataHandle`` must
return ``std::io::Error``. The exception class chooses the ``ErrorKind``:

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

Dropping an open ``DataHandle`` closes it and discards any error from the
close. Call ``close()`` explicitly when a failed flush must be noticed.

Logging Through the Rust ``log`` Crate
--------------------------------------

EcKit writes diagnostics through ``eckit::Log`` channels. ``eckit::init()``
creates an ``eckit::Main`` subclass whose log targets forward each line to the
Rust `log <https://docs.rs/log>`_ crate, so C++ and Rust output end up in the
same place, with the same formatting and filtering. The application picks the
backend (``env_logger``, ``tracing``, and so on) as it would for any Rust
library.

.. list-table::
   :widths: 40 25 35
   :header-rows: 1

   * - eckit channel
     - ``log`` level
     - ``log`` target
   * - ``Log::error()``
     - ``Error``
     - the program name
   * - ``Log::warning()``
     - ``Warn``
     - the program name
   * - ``Log::info()``
     - ``Info``
     - the program name
   * - ``Log::debug()``
     - ``Debug``
     - the program name
   * - ``Log::metrics()``
     - ``Trace``
     - the program name
   * - a library's debug channel
     - ``Debug``
     - the library name, for example ``eckit``

Because each library's debug channel uses the library name as its target,
debug output can be enabled per library with the usual ``log`` filters.

``eckit::init()`` must be called before any other eckit call. It is safe to
call more than once and from several crates; only the first call has an effect.
Threads created later get the same log targets.

Sharing EcKit Types Between Packages
------------------------------------

EcKit types appear in the interfaces of the packages built on top of it: a
library hands back a ``DataHandle``, takes a ``Configuration``, or writes to a
``Stream``. The Rust interface keeps these as the *same* Rust types across
packages.

On the Rust side, each high-level type can be converted to and from its
``eckit-sys`` wrapper:

* ``DataHandle::from_raw`` takes ownership of a ``DataHandleWrapper`` returned
  by another package's bridge, giving the caller a full ``eckit::DataHandle``;
* ``Config::as_sys``, ``Message::as_sys``, ``Grid::as_sys``,
  ``DataHandle::as_sys_mut`` and ``Stream::as_sys_mut`` lend the wrapper to
  another bridge that expects an eckit object as an argument.

On the build side, ``eckit-sys`` declares ``links = "eckit_sys"`` and exports
to the build scripts of crates that depend on it:

.. list-table::
   :widths: 40 60
   :header-rows: 1

   * - Variable
     - Content
   * - ``DEP_ECKIT_SYS_ROOT``
     - Install prefix of the eckit that was built or found
   * - ``DEP_ECKIT_SYS_INCLUDE``
     - The eckit include directory
   * - ``DEP_ECKIT_SYS_CPP_DIR``
     - Directory holding ``EckitBridge.h`` and the wrapper headers

A downstream ``-sys`` crate compiles its own C++ package against that prefix,
which guarantees that every library in the final program was built against one
eckit.

Extending the Bindings
----------------------

To expose another part of eckit:

#. Add a wrapper class to ``rust/crates/eckit-sys/cpp`` in the
   ``eckit_bridge`` namespace. Include ``eckit_exceptions.h`` first in the
   ``.cc`` file so exceptions are translated.
#. Declare the wrapper and its methods in the ``cxx`` bridge in
   ``rust/crates/eckit-sys/src``. Methods that can throw return ``Result``.
   Use a separate bridge module if the functionality sits behind a CMake
   option.
#. List the new sources in ``rust/crates/eckit-sys/build.rs``.
#. Add the safe API to ``rust/crates/eckit``, converting errors with
   ``eckit_sys::Error::from`` and providing ``as_sys`` if other packages need
   to pass the type across their own bridges.
