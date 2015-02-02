/*
 * (C) Copyright 1996-2014 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/mpi/Comm.h"
#include "eckit/mpi/Exceptions.h"

namespace eckit {
namespace mpi {


Comm::Comm()
{
  if( ! initialized() )
    throw mpi::Error( "Trying to construct MPI communicator without MPI being initialized", Here() );
  set_with_C_handle(MPI_COMM_WORLD);
}

Comm::Comm( MPI_Comm comm )
{
  if( ! initialized() )
    throw mpi::Error( "Trying to construct MPI communicator without MPI being initialized", Here() );
  set_with_C_handle(comm);
}

MPI_Fint Comm::fortran_handle()
{
  MPI_Fint fortran_comm = MPI_Comm_c2f(*this);
  return fortran_comm;
}

void Comm::set_with_fortran_handle( MPI_Fint handle )
{
  comm_ = MPI_Comm_f2c( handle );
}

void Comm::set_with_C_handle( MPI_Comm handle )
{
  comm_ = handle;
}

size_t Comm::rank() const
{
  if( !initialized() ) throw mpi::Error( "MPI not initialized when calling eckit::mpi::rank()", Here() );
  int rank;
  ECKIT_MPI_CHECK_RESULT( MPI_Comm_rank( *this, &rank ) );
  return rank;
}

size_t Comm::size() const
{
  if( !initialized() ) throw mpi::Error( "MPI not initialized when calling eckit::mpi::size()", Here() );
  int nproc;
  ECKIT_MPI_CHECK_RESULT( MPI_Comm_size( *this, &nproc ) );
  return nproc;
}

void Comm::barrier() const
{
  ECKIT_MPI_CHECK_RESULT( MPI_Barrier(*this) );
}

Comm& DefaultComm::instance()
{
  static DefaultComm comm_instance;
  return comm_instance;
}

const Comm& CommWorld::instance()
{
  static CommWorld world_instance;
  return world_instance;
}


} // namespace mpi
} // namepsace eckit
