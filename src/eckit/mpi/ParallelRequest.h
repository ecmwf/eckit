// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#ifndef eckit_mpi_ParallelRequest_h
#define eckit_mpi_ParallelRequest_h

#define OMPI_SKIP_MPICXX 1
#define MPICH_SKIP_MPICXX 1

#include <mpi.h>

#include "eckit/mpi/Request.h"

namespace eckit {
namespace mpi {

//----------------------------------------------------------------------------------------------------------------------

class Parallel;

class ParallelRequest : public RequestContent {

private:  // constructor

    ParallelRequest();
    ParallelRequest(MPI_Request);

private:  // methods

    void print(std::ostream&) const override;

    int request() const override;

    bool test() override;

private:  // members

    friend class Parallel;

    MPI_Request request_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace mpi
}  // namespace eckit

#endif
