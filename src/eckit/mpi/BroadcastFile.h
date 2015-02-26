/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_mpi_ReadFile_h
#define eckit_mpi_ReadFile_h

#include "eckit/filesystem/PathName.h"
#include "eckit/mpi/Comm.h"

//-----------------------------------------------------------------------------

namespace eckit {
namespace mpi {

//-----------------------------------------------------------------------------

/// @brief Read file with 1 MPI task, broadcast content, and append to stream
/// @return True if file was read
bool broadcast_file( const PathName&, std::ostream&,
                     const int root = 0,
                     const mpi::Comm& = mpi::DefaultComm::instance() );

//-----------------------------------------------------------------------------

} // namespace mpi
} // namespace eckit

#endif // eckit_mpi_ParallelContextBehavior_h
