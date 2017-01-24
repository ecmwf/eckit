/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/mpi/ParallelRequest.h"

namespace eckit {
namespace mpi {

//----------------------------------------------------------------------------------------------------------------------

ParallelRequest::ParallelRequest() {
}

ParallelRequest::ParallelRequest(MPI_Request request) : request_(request) {
}

void ParallelRequest::print(std::ostream& os) const {
    os << "ParallelRequest("
       << ")";
}

int ParallelRequest::request() const {
    return MPI_Request_c2f(request_);
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace mpi
} // namepsace eckit
