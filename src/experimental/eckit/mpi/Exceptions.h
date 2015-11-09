/*
 * (C) Copyright 1996-2014 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef ECKIT_MPI_Exceptions_h
#define ECKIT_MPI_Exceptions_h

#include <iostream>

#include "eckit/mpi/mpi.h"
#include "eckit/exception/Exceptions.h"

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

namespace eckit {
namespace mpi {

class Error : public eckit::Exception {
public:
  Error(const std::string& msg, const eckit::CodeLocation& loc)
  {
    std::ostringstream s;
    s << "MPI Error: " << msg << " " << " in " << loc;
    reason(s.str());
  }
};

} // namespace mpi
} // namespace eckit

#endif // ECKIT_MPI_Exceptions_h
