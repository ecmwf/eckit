/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_mpi_Serial_h
#define eckit_mpi_Serial_h

#include "eckit/mpi/Environment.h"

namespace eckit {
namespace mpi {

//----------------------------------------------------------------------------------------------------------------------

class Serial : public eckit::mpi::Environment {

public:  // methods

    /// @brief Initialize MPI
    virtual void initialize();

    /// @brief Finalze MPI
    virtual void finalize();

protected:

    /// @brief is MPI initialized
    virtual bool initialized();

    Serial();

    virtual ~Serial();

};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace mpi
}  // namespace eckit

#endif
