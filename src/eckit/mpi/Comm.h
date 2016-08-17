/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_mpi_Comm_h
#define eckit_mpi_Comm_h

#include <cstddef>

#include "eckit/memory/NonCopyable.h"

namespace eckit {
namespace mpi {

//----------------------------------------------------------------------------------------------------------------------

class Comm : private eckit::NonCopyable {

public:  // methods

  /// @brief MPI rank of default communicator
  /// Equivalent to comm().rank()
  virtual size_t rank() const = 0;

  /// @brief MPI size of default communicator
  /// Equivalent to comm().size()
  virtual size_t size() const = 0;

  /// @brief MPI barrier for default communicator
  /// Equivalent to comm().barrier()
  virtual void barrier() = 0;

  /// @brief MPI abort for default communicator
  /// Equivalent to comm().abort()
  virtual void abort() = 0;

protected:

  Comm();

  virtual ~Comm();

};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace mpi
}  // namespace eckit

#endif
