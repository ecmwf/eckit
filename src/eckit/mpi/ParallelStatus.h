/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_mpi_ParallelStatus_h
#define eckit_mpi_ParallelStatus_h

#include <mpi.h>

#include "eckit/mpi/Status.h"

namespace eckit {
namespace mpi {

//----------------------------------------------------------------------------------------------------------------------

class Parallel;

class ParallelStatus : public StatusContent {

private: // methods

    virtual int source() const { return status_.MPI_SOURCE; }
    virtual int tag() const    { return status_.MPI_TAG; }
    virtual int error() const  { return status_.MPI_ERROR; }

    virtual void print(std::ostream&) const;

private: // members

    friend class Parallel;

    MPI_Status status_;

};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace mpi
}  // namespace eckit

#endif
