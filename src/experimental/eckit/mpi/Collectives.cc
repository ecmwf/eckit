/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/mpi/Collectives.h"
#include "eckit/mpi/Exceptions.h"

namespace eckit {
namespace mpi {

// ----------------------------------------------------------------------------------
// Broadcast

template<typename DATA_TYPE>
int broadcast_pod( const Comm& comm,
                   DATA_TYPE& data,
                   size_t root )
{
  ECKIT_MPI_CHECK_RESULT( MPI_Bcast(&data,1,eckit::mpi::datatype<DATA_TYPE>(),int(root),comm) );
  return MPI_SUCCESS;
}

template<typename DATA_TYPE>
int broadcast_buffer( const Comm& comm,
                      DATA_TYPE data[], size_t size,
                      size_t root )
{
  ECKIT_MPI_CHECK_RESULT( MPI_Bcast(data,int(size),eckit::mpi::datatype<DATA_TYPE>(),int(root),comm) );
  return MPI_SUCCESS;
}

template<typename DATA_TYPE>
int broadcast_vector( const Comm& comm,
                      std::vector<DATA_TYPE>& data,
                      size_t root, bool resize )
{
  if( resize )
  {
    int size = data.size();
    broadcast_pod(comm,size,root);
    if( comm.rank() != root ) data.resize(size);
  }
  ECKIT_MPI_CHECK_RESULT( MPI_Bcast(data.data(),int(data.size()),eckit::mpi::datatype<DATA_TYPE>(),int(root),comm) );
  return MPI_SUCCESS;
}

template<> int broadcast( const Comm& comm, int    &data, size_t root, bool resize ) { return broadcast_pod(comm,data,root); }
template<> int broadcast( const Comm& comm, float  &data, size_t root, bool resize ) { return broadcast_pod(comm,data,root); }
template<> int broadcast( const Comm& comm, double &data, size_t root, bool resize ) { return broadcast_pod(comm,data,root); }
template<> int broadcast( const Comm& comm, long   &data, size_t root, bool resize ) { return broadcast_pod(comm,data,root); }

template<> int broadcast( int    &data, size_t root, bool resize ) { return broadcast_pod(comm(),data,root); }
template<> int broadcast( float  &data, size_t root, bool resize ) { return broadcast_pod(comm(),data,root); }
template<> int broadcast( double &data, size_t root, bool resize ) { return broadcast_pod(comm(),data,root); }
template<> int broadcast( long   &data, size_t root, bool resize ) { return broadcast_pod(comm(),data,root); }

template<> int broadcast( const Comm& comm, int    data[], size_t size, size_t root ) { return broadcast_buffer(comm,data,size,root); }
template<> int broadcast( const Comm& comm, float  data[], size_t size, size_t root ) { return broadcast_buffer(comm,data,size,root); }
template<> int broadcast( const Comm& comm, double data[], size_t size, size_t root ) { return broadcast_buffer(comm,data,size,root); }
template<> int broadcast( const Comm& comm, long   data[], size_t size, size_t root ) { return broadcast_buffer(comm,data,size,root); }

template<> int broadcast( int    data[], size_t size, size_t root ) { return broadcast_buffer(comm(),data,size,root); }
template<> int broadcast( float  data[], size_t size, size_t root ) { return broadcast_buffer(comm(),data,size,root); }
template<> int broadcast( double data[], size_t size, size_t root ) { return broadcast_buffer(comm(),data,size,root); }
template<> int broadcast( long   data[], size_t size, size_t root ) { return broadcast_buffer(comm(),data,size,root); }

template<> int broadcast( const Comm& comm, std::vector<int>    &data, size_t root, bool resize ) { return broadcast_vector(comm,data,root,resize); }
template<> int broadcast( const Comm& comm, std::vector<float>  &data, size_t root, bool resize ) { return broadcast_vector(comm,data,root,resize); }
template<> int broadcast( const Comm& comm, std::vector<double> &data, size_t root, bool resize ) { return broadcast_vector(comm,data,root,resize); }
template<> int broadcast( const Comm& comm, std::vector<long>   &data, size_t root, bool resize ) { return broadcast_vector(comm,data,root,resize); }

template<> int broadcast( std::vector<int>    &data, size_t root, bool resize ) { return broadcast_vector(comm(),data,root,resize); }
template<> int broadcast( std::vector<float>  &data, size_t root, bool resize ) { return broadcast_vector(comm(),data,root,resize); }
template<> int broadcast( std::vector<double> &data, size_t root, bool resize ) { return broadcast_vector(comm(),data,root,resize); }
template<> int broadcast( std::vector<long>   &data, size_t root, bool resize ) { return broadcast_vector(comm(),data,root,resize); }

// ------------------------------------------------------------------------------------------
// all_reduce


template<typename DATA_TYPE>
int all_reduce_pod( const Comm& comm,
                    const DATA_TYPE& send,
                          DATA_TYPE& recv,
                    const Operation& mpi_op )
{
  ECKIT_MPI_CHECK_RESULT( MPI_Allreduce(&const_cast<DATA_TYPE&>(send),&recv,1,datatype<DATA_TYPE>(),mpi_op,comm) );
  return MPI_SUCCESS;
}

template<typename DATA_TYPE>
int all_reduce_pod_inplace( const Comm& comm,
                            DATA_TYPE& sendrecv,
                            const Operation& mpi_op)
{
  ECKIT_MPI_CHECK_RESULT( MPI_Allreduce(MPI_IN_PLACE,&sendrecv,1,datatype<DATA_TYPE>(),mpi_op,comm) );
  return MPI_SUCCESS;
}

template<typename DATA_TYPE>
int all_reduce_buffer_inplace( const Comm& comm,
                               DATA_TYPE sendrecv[], size_t size,
                               const Operation& mpi_op)
{
  ECKIT_MPI_CHECK_RESULT( MPI_Allreduce(MPI_IN_PLACE,sendrecv,int(size),datatype<DATA_TYPE>(),mpi_op,comm) );
  return MPI_SUCCESS;
}

template<typename DATA_TYPE>
int all_reduce_vector( const Comm& comm,
                       const std::vector<DATA_TYPE>& send,
                             std::vector<DATA_TYPE>& recv,
                       const Operation& mpi_op )
{
  recv.resize(send.size());
  ECKIT_MPI_CHECK_RESULT( MPI_Allreduce(const_cast<DATA_TYPE*>(send.data()),recv.data(),send.size(),datatype<DATA_TYPE>(),mpi_op,comm) );
  return MPI_SUCCESS;
}

template<typename DATA_TYPE>
int all_reduce_vector_inplace( const Comm& comm,
                               std::vector<DATA_TYPE>& sendrecv,
                               const Operation& mpi_op )
{
  ECKIT_MPI_CHECK_RESULT( MPI_Allreduce(MPI_IN_PLACE,sendrecv.data(),sendrecv.size(),datatype<DATA_TYPE>(),mpi_op,comm) );
  return MPI_SUCCESS;
}


template<typename DATA_TYPE_1, typename DATA_TYPE_2>
int all_reduce_pair( const Comm& comm,
                     const std::pair<DATA_TYPE_1,DATA_TYPE_2>& send,
                           std::pair<DATA_TYPE_1,DATA_TYPE_2>& recv,
                     const Operation& mpi_op )
{
  ECKIT_MPI_CHECK_RESULT( MPI_Allreduce(const_cast<DATA_TYPE_1*>(&send.first),&recv.first,1,datatype<DATA_TYPE_1,DATA_TYPE_2>(),mpi_op,comm) );
  return MPI_SUCCESS;
}

template<typename DATA_TYPE_1, typename DATA_TYPE_2>
int all_reduce_vector_pair( const Comm& comm,
                            const std::vector< std::pair<DATA_TYPE_1,DATA_TYPE_2> >& send,
                                  std::vector< std::pair<DATA_TYPE_1,DATA_TYPE_2> >& recv,
                            const Operation& mpi_op )
{
  recv.resize(send.size());
  ECKIT_MPI_CHECK_RESULT( MPI_Allreduce(const_cast<DATA_TYPE_1*>(&send[0].first),&recv[0].first,send.size(),datatype<DATA_TYPE_1,DATA_TYPE_2>(),mpi_op,comm) );
  return MPI_SUCCESS;
}

template<> int all_reduce( const Comm& comm, const int    &send, int    &recv, const Operation& op) { return all_reduce_pod(comm,send,recv,op); }
template<> int all_reduce( const Comm& comm, const float  &send, float  &recv, const Operation& op) { return all_reduce_pod(comm,send,recv,op); }
template<> int all_reduce( const Comm& comm, const double &send, double &recv, const Operation& op) { return all_reduce_pod(comm,send,recv,op); }
template<> int all_reduce( const Comm& comm, const long   &send, long   &recv, const Operation& op) { return all_reduce_pod(comm,send,recv,op); }

template<> int all_reduce( const int    &send, int    &recv, const Operation& op) { return all_reduce_pod(comm(),send,recv,op); }
template<> int all_reduce( const float  &send, float  &recv, const Operation& op) { return all_reduce_pod(comm(),send,recv,op); }
template<> int all_reduce( const double &send, double &recv, const Operation& op) { return all_reduce_pod(comm(),send,recv,op); }
template<> int all_reduce( const long   &send, long   &recv, const Operation& op) { return all_reduce_pod(comm(),send,recv,op); }

template<> int all_reduce( const Comm& comm, const std::vector<int>    &send, std::vector<int>    &recv, const Operation& op) { return all_reduce_vector(comm,send,recv,op); }
template<> int all_reduce( const Comm& comm, const std::vector<float>  &send, std::vector<float>  &recv, const Operation& op) { return all_reduce_vector(comm,send,recv,op); }
template<> int all_reduce( const Comm& comm, const std::vector<double> &send, std::vector<double> &recv, const Operation& op) { return all_reduce_vector(comm,send,recv,op); }
template<> int all_reduce( const Comm& comm, const std::vector<long>   &send, std::vector<long>   &recv, const Operation& op) { return all_reduce_vector(comm,send,recv,op); }

template<> int all_reduce( const std::vector<int>    &send, std::vector<int>    &recv, const Operation& op) { return all_reduce_vector(comm(),send,recv,op); }
template<> int all_reduce( const std::vector<float>  &send, std::vector<float>  &recv, const Operation& op) { return all_reduce_vector(comm(),send,recv,op); }
template<> int all_reduce( const std::vector<double> &send, std::vector<double> &recv, const Operation& op) { return all_reduce_vector(comm(),send,recv,op); }
template<> int all_reduce( const std::vector<long>   &send, std::vector<long>   &recv, const Operation& op) { return all_reduce_vector(comm(),send,recv,op); }

template<> int all_reduce( const Comm& comm, int    &sendrecv, const Operation& op) { return all_reduce_pod_inplace(comm,sendrecv,op); }
template<> int all_reduce( const Comm& comm, float  &sendrecv, const Operation& op) { return all_reduce_pod_inplace(comm,sendrecv,op); }
template<> int all_reduce( const Comm& comm, double &sendrecv, const Operation& op) { return all_reduce_pod_inplace(comm,sendrecv,op); }
template<> int all_reduce( const Comm& comm, long   &sendrecv, const Operation& op) { return all_reduce_pod_inplace(comm,sendrecv,op); }

template<> int all_reduce( int    &sendrecv, const Operation& op) { return all_reduce_pod_inplace(comm(),sendrecv,op); }
template<> int all_reduce( float  &sendrecv, const Operation& op) { return all_reduce_pod_inplace(comm(),sendrecv,op); }
template<> int all_reduce( double &sendrecv, const Operation& op) { return all_reduce_pod_inplace(comm(),sendrecv,op); }
template<> int all_reduce( long   &sendrecv, const Operation& op) { return all_reduce_pod_inplace(comm(),sendrecv,op); }

template<> int all_reduce( const Comm& comm, std::vector<int>    &sendrecv, const Operation& op) { return all_reduce_vector_inplace(comm,sendrecv,op); }
template<> int all_reduce( const Comm& comm, std::vector<float>  &sendrecv, const Operation& op) { return all_reduce_vector_inplace(comm,sendrecv,op); }
template<> int all_reduce( const Comm& comm, std::vector<double> &sendrecv, const Operation& op) { return all_reduce_vector_inplace(comm,sendrecv,op); }
template<> int all_reduce( const Comm& comm, std::vector<long>   &sendrecv, const Operation& op) { return all_reduce_vector_inplace(comm,sendrecv,op); }

template<> int all_reduce( int    sendrecv[], size_t size, const Operation& op) { return all_reduce_buffer_inplace(comm(),sendrecv,size,op); }
template<> int all_reduce( float  sendrecv[], size_t size, const Operation& op) { return all_reduce_buffer_inplace(comm(),sendrecv,size,op); }
template<> int all_reduce( double sendrecv[], size_t size, const Operation& op) { return all_reduce_buffer_inplace(comm(),sendrecv,size,op); }
template<> int all_reduce( long   sendrecv[], size_t size, const Operation& op) { return all_reduce_buffer_inplace(comm(),sendrecv,size,op); }

template<> int all_reduce( const Comm& comm, int    sendrecv[], size_t size, const Operation& op) { return all_reduce_buffer_inplace(comm,sendrecv,size,op); }
template<> int all_reduce( const Comm& comm, float  sendrecv[], size_t size, const Operation& op) { return all_reduce_buffer_inplace(comm,sendrecv,size,op); }
template<> int all_reduce( const Comm& comm, double sendrecv[], size_t size, const Operation& op) { return all_reduce_buffer_inplace(comm,sendrecv,size,op); }
template<> int all_reduce( const Comm& comm, long   sendrecv[], size_t size, const Operation& op) { return all_reduce_buffer_inplace(comm,sendrecv,size,op); }

template<> int all_reduce( std::vector<int>    &sendrecv, const Operation& op) { return all_reduce_vector_inplace(comm(),sendrecv,op); }
template<> int all_reduce( std::vector<float>  &sendrecv, const Operation& op) { return all_reduce_vector_inplace(comm(),sendrecv,op); }
template<> int all_reduce( std::vector<double> &sendrecv, const Operation& op) { return all_reduce_vector_inplace(comm(),sendrecv,op); }
template<> int all_reduce( std::vector<long>   &sendrecv, const Operation& op) { return all_reduce_vector_inplace(comm(),sendrecv,op); }

template<> int all_reduce( const Comm& comm, const std::pair<int,int>    &send, std::pair<int,int>    &recv, const Operation& op) { return all_reduce_pair(comm,send,recv,op); }
template<> int all_reduce( const Comm& comm, const std::pair<long,int>   &send, std::pair<long,int>   &recv, const Operation& op) { return all_reduce_pair(comm,send,recv,op); }
template<> int all_reduce( const Comm& comm, const std::pair<float,int>  &send, std::pair<float,int>  &recv, const Operation& op) { return all_reduce_pair(comm,send,recv,op); }
template<> int all_reduce( const Comm& comm, const std::pair<double,int> &send, std::pair<double,int> &recv, const Operation& op) { return all_reduce_pair(comm,send,recv,op); }

template<> int all_reduce( const std::pair<int,int>    &send, std::pair<int,int>    &recv, const Operation& op) { return all_reduce_pair(comm(),send,recv,op); }
template<> int all_reduce( const std::pair<long,int>   &send, std::pair<long,int>   &recv, const Operation& op) { return all_reduce_pair(comm(),send,recv,op); }
template<> int all_reduce( const std::pair<float,int>  &send, std::pair<float,int>  &recv, const Operation& op) { return all_reduce_pair(comm(),send,recv,op); }
template<> int all_reduce( const std::pair<double,int> &send, std::pair<double,int> &recv, const Operation& op) { return all_reduce_pair(comm(),send,recv,op); }

template<> int all_reduce( const Comm& comm, const std::vector< std::pair<int,int> >    &send, std::vector< std::pair<int,int> >    &recv, const Operation& op) { return all_reduce_vector_pair(comm,send,recv,op); }
template<> int all_reduce( const Comm& comm, const std::vector< std::pair<long,int> >   &send, std::vector< std::pair<long,int> >   &recv, const Operation& op) { return all_reduce_vector_pair(comm,send,recv,op); }
template<> int all_reduce( const Comm& comm, const std::vector< std::pair<float,int> >  &send, std::vector< std::pair<float,int> >  &recv, const Operation& op) { return all_reduce_vector_pair(comm,send,recv,op); }
template<> int all_reduce( const Comm& comm, const std::vector< std::pair<double,int> > &send, std::vector< std::pair<double,int> > &recv, const Operation& op) { return all_reduce_vector_pair(comm,send,recv,op); }

template<> int all_reduce( const std::vector< std::pair<int,int> >    &send, std::vector< std::pair<int,int> >    &recv, const Operation& op) { return all_reduce_vector_pair(comm(),send,recv,op); }
template<> int all_reduce( const std::vector< std::pair<long,int> >   &send, std::vector< std::pair<long,int> >   &recv, const Operation& op) { return all_reduce_vector_pair(comm(),send,recv,op); }
template<> int all_reduce( const std::vector< std::pair<float,int> >  &send, std::vector< std::pair<float,int> >  &recv, const Operation& op) { return all_reduce_vector_pair(comm(),send,recv,op); }
template<> int all_reduce( const std::vector< std::pair<double,int> > &send, std::vector< std::pair<double,int> > &recv, const Operation& op) { return all_reduce_vector_pair(comm(),send,recv,op); }


} // namespace mpi
} // namepsace eckit

