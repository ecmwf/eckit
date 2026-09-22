// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#ifndef eckit_mpi_ParallelStatus_h
#define eckit_mpi_ParallelStatus_h

#define OMPI_SKIP_MPICXX 1
#define MPICH_SKIP_MPICXX 1

#include <mpi.h>

#include <iosfwd>

#include "eckit/mpi/Status.h"

namespace eckit {
namespace mpi {

//----------------------------------------------------------------------------------------------------------------------

class Parallel;

class ParallelStatus : public StatusContent {
private:  // methods

    int source() const override { return status_.MPI_SOURCE; }
    int tag() const override { return status_.MPI_TAG; }
    int error() const override { return status_.MPI_ERROR; }

    void print(std::ostream&) const override;

private:  // members

    friend class Parallel;

    MPI_Status status_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace mpi
}  // namespace eckit

#endif
