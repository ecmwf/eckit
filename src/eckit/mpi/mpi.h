/*
 * (C) Copyright 1996-2014 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef ECKIT_MPI_MPI_h
#define ECKIT_MPI_MPI_h

#include <stdlib.h>

#include "eckit/eckit_config.h"
#if defined(ECKIT_HAVE_MPI) && !defined(ECKIT_MPISTUBS)
#include <mpi.h>
#else
#include "mpistubs.h"
#endif

#include "eckit/mpi/Comm.h"

namespace eckit {
namespace mpi {

class Comm;

/// @brief World MPI communicator
const Comm& world();

/// @brief Default MPI communicator instance.
/// If not modified, this is identical to world()
Comm& comm();

/// @brief Return true if MPI is already initialized
bool initialized();

/// @brief Return true if MPI is already finalized
bool finalized();

/// @brief Initialize mpi
void init(int argc=0, char *argv[]=0);

/// @brief Finalze mpi
/// This must be the last MPI call. The only MPI call allowed after this
/// call is finalized()
void finalize();

/// @brief MPI rank of default communicator
/// Equivalent to comm().rank()
size_t rank();

/// @brief MPI size of default communicator
/// Equivalent to comm().size()
size_t size();

/// @brief MPI barrier for default communicator
/// Equivalent to comm().barrier()
void barrier();

/// @brief Return MPI data-type for given DATA_TYPE
template<typename DATA_TYPE>
MPI_Datatype datatype();

/// @brief Return MPI data-type of given argument
template<typename DATA_TYPE>
MPI_Datatype datatype(DATA_TYPE&);

} // namespace mpi
} // namespace eckit

#endif // ECKIT_MPI_MPI_h
