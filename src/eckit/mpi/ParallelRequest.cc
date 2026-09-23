// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/mpi/ParallelRequest.h"

#include <string_view>

#include "eckit/log/CodeLocation.h"
#include "eckit/mpi/Parallel.h"

namespace eckit {
namespace mpi {


void MPICall(int code, std::string_view mpifunc, const eckit::CodeLocation& loc);
#define MPI_CALL(a) MPICall(a, #a, Here())

//----------------------------------------------------------------------------------------------------------------------

ParallelRequest::ParallelRequest() {}

ParallelRequest::ParallelRequest(MPI_Request request) : request_(request) {}

void ParallelRequest::print(std::ostream& os) const {
    os << "ParallelRequest("
       << ")";
}

int ParallelRequest::request() const {
    return MPI_Request_c2f(request_);
}

bool ParallelRequest::test() {
    int requestCompleted;
    MPI_CALL(MPI_Test(&request_, &requestCompleted, MPI_STATUS_IGNORE));
    return requestCompleted;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace mpi
}  // namespace eckit
