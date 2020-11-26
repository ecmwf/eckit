/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Tiago Quintino
/// @date   Dec 2018

#ifndef eckit_io_FDataSync_h
#define eckit_io_FDataSync_h

namespace eckit {

/// An fsync that retries when interrupted by system
int fsync(int fd);

/// A platform independent fdatasync
int fdatasync(int fd);

}  // namespace eckit

#endif
