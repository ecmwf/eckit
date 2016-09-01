/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/mpi/Comm.h"

namespace eckit {
namespace mpi {

//----------------------------------------------------------------------------------------------------------------------

#if 0
static inline void MpiCall(int code, const char *msg, const eckit::CodeLocation& loc )
{
    if (code != MPI_SUCCESS) {
        std::ostringstream oss;

        char error[10240];
        int len = sizeof(error) - 1;
        MPI_Error_string(code, error, &len);
        error[len] = 0;

        oss << "MPI Call failed: " << error << " " << msg << " " << loc;
        throw eckit::SeriousBug(msg, loc);
    }
}

#define MPI_CALL(a)    MpiCall(a,#a,Here())

#endif

Comm::Comm() {
}

Comm::~Comm() {
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace mpi
} // namepsace eckit
