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

#define ECKIT_MPI_CHECK_RESULT( MPI_CALL )\
do { \
  int ierr = MPI_CALL; \
  if (ierr != MPI_SUCCESS) { \
    char errstr [MPI_MAX_ERROR_STRING]; \
    int errsize = 0; \
    MPI_Error_string(ierr,errstr,&errsize); \
    std::string mpistr( errstr, errsize ); \
    throw eckit::mpi::Error( std::string("MPI call: ") + \
      std::string(#MPI_CALL) + \
      std::string(" did not return MPI_SUCCESS:\n")+mpistr, Here() ); \
  } \
} while(0)

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

namespace eckit {
namespace mpi {

//----------------------------------------------------------------------------------------------------------------------

class Error : public eckit::Exception {
public:
  Error(const std::string& msg, const eckit::CodeLocation& loc)
  {
    std::ostringstream s;
    s << "MPI Error: " << msg << " " << " in " << loc;
    reason(s.str());
  }
};

//----------------------------------------------------------------------------------------------------------------------

Parallel::Parallel() {
}

Parallel::~Parallel() {
}

bool Parallel::initialized() {
    int initialized;
    ECKIT_MPI_CHECK_RESULT( MPI_Initialized( &initialized ) );
    return initialized;
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
