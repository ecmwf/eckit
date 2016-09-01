/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/mpi/Parallel.h"

#include <strstream>

#include <mpi.h>

#include "eckit/runtime/Context.h"

#include "eckit/exception/Exceptions.h"

//----------------------------------------------------------------------------------------------------------------------

class MPIError : public eckit::Exception {
public:
  MPIError(const std::string& msg, const eckit::CodeLocation& loc) : eckit::Exception(msg, loc)
  {
    std::ostringstream s;
    s << "MPI Error: " << msg << " in " << loc;
    reason(s.str());
  }
};

//----------------------------------------------------------------------------------------------------------------------

static inline void MPICall(int code, const char *msg, const eckit::CodeLocation& loc)
{
    if (code != MPI_SUCCESS) {

        char error[10240];
        int len = sizeof(error) - 1;
        MPI_Error_string(code, error, &len);
        error[len] = 0;

        std::ostringstream oss;
        oss << "MPI Call failed with error '" << error << "' -- " << msg;
        throw MPIError(oss.str(), loc);
    }
}

#define MPI_CALL(a) MPICall(a,#a,Here())

namespace eckit {
namespace mpi {

//----------------------------------------------------------------------------------------------------------------------

Parallel::Parallel() {
}

Parallel::~Parallel() {
}

void Parallel::initialize() {
    NOTIMP;
}

void Parallel::finalize() {
    NOTIMP;
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace mpi
} // namepsace eckit
