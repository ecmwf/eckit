/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

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
