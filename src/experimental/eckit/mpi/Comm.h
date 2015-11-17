/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef ECKIT_MPI_Comm_h
#define ECKIT_MPI_Comm_h

#include "eckit/mpi/mpi.h"


namespace eckit {
namespace mpi {

// --------------------------------------------------------------------

/// @brief MPI Communicator type
class Comm
{
public:

  /// @brief Constructor, defaults to MPI_COMM_WORLD
  Comm();

  /// @brief Implicit conversion to MPI_Comm
  operator MPI_Comm() const { return comm_; }

  /// @brief Fortran handle (integer value)
  MPI_Fint fortran_communicator();

  /// @brief Modify internal communicator with fortran communicator handle
  void attach_fortran_communicator( MPI_Fint );

  /// @brief Modify internal communicator
  void attach_communicator( MPI_Comm );

  /// @brief Size of communicator (number of mpi tasks participating)
  size_t size() const;

  /// @brief Rank of current mpi task within communicator
  size_t rank() const;

  /// @brief Synchronize all mpi tasks
  void barrier() const;

private:
  MPI_Comm comm_;
};

// --------------------------------------------------------------------

/// @brief Default communicator singleton, which can be modified.
class DefaultComm : public Comm
{
public:
  static Comm& instance();
};

// --------------------------------------------------------------------

/// @brief World communicator singleton
class CommWorld : private Comm
{
public:
  CommWorld() : Comm() {}
  static const Comm& instance();
};

// --------------------------------------------------------------------

} // namespace mpi
} // namespace eckit

#endif // ECKIT_MPI_Comm_h
