/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_mpi_Environment_h
#define eckit_mpi_Environment_h

#include <string>

namespace eckit {
namespace mpi {

//----------------------------------------------------------------------------------------------------------------------

class Comm;

class Environment {

public:  // methods

  /// @returns the communicator registered with associated name
  static Comm& comm(const char* name = 0);

  static void initialize();


protected:

  /// @brief Initialize MPI
  virtual void initialize() = 0;

  /// @brief Finalze MPI
  virtual void finalize() = 0;

  /// @brief is MPI initialized?
  virtual bool initialized() = 0;

protected:

  Environment();

  virtual ~Environment();

};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace mpi
}  // namespace eckit

#endif
