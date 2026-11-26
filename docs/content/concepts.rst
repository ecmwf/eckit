.. SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
.. SPDX-License-Identifier: Apache-2.0

Concepts
========

DataHandle and Available Subclasses
-----------------------------------

.. list-table::
   :widths: 20 20 20 20 20
   :header-rows: 0

   * - TeeHandle
     - PartHandle
     - MemoryHandle
     - PeekHandle
     - AIOHandle
   * - EmptyHandle
     - AsyncHandle
     - SeekableHandle
     - SharedHandle
     - StatsHandle
   * - PartFileHandle
     - URLHandle
     - EasyCurlHandle
     - MultiSocketHandle
     - FileHandle
   * - InstantTCPSocketHandle
     - PipeHandle
     - FTPHandle
     - PooledHandle
     - MultiHandle
   * - BufferedHandle
     - TransportHandle
     - MMappedFileHandle
     - RawFileHandle
     - TCPHandle
   * - FileDescHandle
     -
     -
     -
     -

