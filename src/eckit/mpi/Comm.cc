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
  comm_ = MPI_COMM_WORLD;
}

MPI_Fint Comm::fortran_communicator()
{
  MPI_Fint fortran_comm = MPI_Comm_c2f(*this);
  return fortran_comm;
}

void Comm::attach_fortran_communicator( MPI_Fint fcomm )
{
  MPI_Comm comm = MPI_Comm_f2c( fcomm );
  attach_communicator( comm );
}

void Comm::attach_communicator( MPI_Comm comm )
{
  int result;
  if( comm_ )
  {
      ECKIT_MPI_CHECK_RESULT( MPI_Comm_compare( comm, comm_, &result ) );
    if( result == MPI_UNEQUAL )
    {
      ECKIT_MPI_CHECK_RESULT( MPI_Comm_compare( comm_, MPI_COMM_WORLD, &result ) );
      if( result == MPI_UNEQUAL )
      {
        ECKIT_MPI_CHECK_RESULT( MPI_Comm_free( &comm_ ) );
      }
    }
  }
  comm_ = comm;
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
