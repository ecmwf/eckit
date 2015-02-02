/*
 * (C) Copyright 1996-2014 ECMWF.
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

class Comm
{
public:

  Comm();

  Comm( MPI_Comm );

  operator MPI_Comm() const { return comm_; }

  MPI_Fint fortran_handle();

  void set_with_fortran_handle( MPI_Fint );

  void set_with_C_handle( MPI_Comm );

  size_t size() const;

  size_t rank() const;

  void barrier() const;

private:
  MPI_Comm comm_;
};

class DefaultComm : public Comm
{
public:
  static Comm& instance();
};

class CommWorld : private Comm
{
public:
  CommWorld() : Comm(MPI_COMM_WORLD) {}
  static const Comm& instance();
};

} // namespace mpi
} // namespace eckit

#endif // ECKIT_MPI_Comm_h
