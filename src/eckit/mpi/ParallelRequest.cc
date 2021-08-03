/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/mpi/ParallelRequest.h"
#include "eckit/log/CodeLocation.h"
#include "eckit/mpi/Parallel.h"

namespace eckit {
namespace mpi {


void MPICall(int code, const char* mpifunc, const eckit::CodeLocation& loc);
#define MPI_CALL(a) MPICall(a, #a, Here())

//----------------------------------------------------------------------------------------------------------------------

ParallelRequest::ParallelRequest() {}

ParallelRequest::ParallelRequest(MPI_Request request) :
    request_(request) {}

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
