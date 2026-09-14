// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

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
