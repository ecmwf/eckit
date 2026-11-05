.. SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
.. SPDX-License-Identifier: Apache-2.0

Approach
========

This page gives the reasons behind the shape of the interface. How to use it
is on :doc:`usage`.

Two Crates
----------

``eckit-sys``
   Unsafe bindings to the C++ library, built on `cxx <https://cxx.rs>`_.

   * :ref:`Builds libeckit from source or links an installed one <Building the Rust Crates>`.
   * :ref:`Exposes the eckit CMake options as Cargo features <Cargo Features>`.
   * Converts C++ exceptions into ``eckit::Error``.

``eckit``
   Safe Rust API over ``eckit-sys``.

   * :ref:`eckit::Result and a matchable eckit::Error <Errors>`.
   * :ref:`std::io traits on data handles <Data Handles>`.
   * Iterators for :ref:`readers <Messages>` and
     :ref:`sub-configurations <Configuration>`.
   * Lifetimes for C++ references.

Error Type
----------

``eckit::Error`` keeps the exception class as its variant, not only the
message, so Rust code matches on the class the way C++ code catches it.

Log Output
----------

EcKit writes its diagnostics through its own log channels. ``eckit::init()``
redirects them to the `log <https://docs.rs/log>`_ crate, so the application
configures one backend for C++ and Rust output alike.
