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
#include <vector>

#include "eckit/memory/NonCopyable.h"

namespace eckit {
namespace mpi {

//----------------------------------------------------------------------------------------------------------------------

typedef int Operation;

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
  virtual void barrier() const = 0;

  /// @brief MPI abort for default communicator
  /// Equivalent to comm().abort()
  virtual void abort() const = 0;

  virtual int broadcast( int    &data, size_t root, bool resize ) const = 0;
  virtual int broadcast( float  &data, size_t root, bool resize ) const = 0;
  virtual int broadcast( double &data, size_t root, bool resize ) const = 0;
  virtual int broadcast( long   &data, size_t root, bool resize ) const = 0;

  virtual int broadcast( int    data[], size_t size, size_t root ) const = 0;
  virtual int broadcast( float  data[], size_t size, size_t root ) const = 0;
  virtual int broadcast( double data[], size_t size, size_t root ) const = 0;
  virtual int broadcast( long   data[], size_t size, size_t root ) const = 0;

  virtual int broadcast( std::vector<int>    &data, size_t root, bool resize ) const = 0;
  virtual int broadcast( std::vector<float>  &data, size_t root, bool resize ) const = 0;
  virtual int broadcast( std::vector<double> &data, size_t root, bool resize ) const = 0;
  virtual int broadcast( std::vector<long>   &data, size_t root, bool resize ) const = 0;

  virtual int all_reduce( const int    &send, int    &recv, const Operation& op) const = 0;
  virtual int all_reduce( const float  &send, float  &recv, const Operation& op) const = 0;
  virtual int all_reduce( const double &send, double &recv, const Operation& op) const = 0;
  virtual int all_reduce( const long   &send, long   &recv, const Operation& op) const = 0;

  virtual int all_reduce( const std::vector<int>    &send, std::vector<int>    &recv, const Operation& op) const = 0;
  virtual int all_reduce( const std::vector<float>  &send, std::vector<float>  &recv, const Operation& op) const = 0;
  virtual int all_reduce( const std::vector<double> &send, std::vector<double> &recv, const Operation& op) const = 0;
  virtual int all_reduce( const std::vector<long>   &send, std::vector<long>   &recv, const Operation& op) const = 0;

  virtual int all_reduce( int    &sendrecv, const Operation& op) const = 0;
  virtual int all_reduce( float  &sendrecv, const Operation& op) const = 0;
  virtual int all_reduce( double &sendrecv, const Operation& op) const = 0;
  virtual int all_reduce( long   &sendrecv, const Operation& op) const = 0;

  virtual int all_reduce( int    sendrecv[], size_t size, const Operation& op) const = 0;
  virtual int all_reduce( float  sendrecv[], size_t size, const Operation& op) const = 0;
  virtual int all_reduce( double sendrecv[], size_t size, const Operation& op) const = 0;
  virtual int all_reduce( long   sendrecv[], size_t size, const Operation& op) const = 0;

  virtual int all_reduce( std::vector<int>    &sendrecv, const Operation& op) const = 0;
  virtual int all_reduce( std::vector<float>  &sendrecv, const Operation& op) const = 0;
  virtual int all_reduce( std::vector<double> &sendrecv, const Operation& op) const = 0;
  virtual int all_reduce( std::vector<long>   &sendrecv, const Operation& op) const = 0;

  // virtual int all_reduce( const std::pair<int,int>    &send, std::pair<int,int>    &recv, const Operation& op) { return all_reduce_pair(comm(),send,recv,op); }
  // virtual int all_reduce( const std::pair<long,int>   &send, std::pair<long,int>   &recv, const Operation& op) { return all_reduce_pair(comm(),send,recv,op); }
  // virtual int all_reduce( const std::pair<float,int>  &send, std::pair<float,int>  &recv, const Operation& op) { return all_reduce_pair(comm(),send,recv,op); }
  // virtual int all_reduce( const std::pair<double,int> &send, std::pair<double,int> &recv, const Operation& op) { return all_reduce_pair(comm(),send,recv,op); }

  // virtual int all_reduce( const std::vector< std::pair<int,int> >    &send, std::vector< std::pair<int,int> >    &recv, const Operation& op) { return all_reduce_vector_pair(comm(),send,recv,op); }
  // virtual int all_reduce( const std::vector< std::pair<long,int> >   &send, std::vector< std::pair<long,int> >   &recv, const Operation& op) { return all_reduce_vector_pair(comm(),send,recv,op); }
  // virtual int all_reduce( const std::vector< std::pair<float,int> >  &send, std::vector< std::pair<float,int> >  &recv, const Operation& op) { return all_reduce_vector_pair(comm(),send,recv,op); }
  // virtual int all_reduce( const std::vector< std::pair<double,int> > &send, std::vector< std::pair<double,int> > &recv, const Operation& op) { return all_reduce_vector_pair(comm(),send,recv,op); }

  virtual Operation sum()  const = 0;
  virtual Operation prod() const = 0;
  virtual Operation max()  const = 0;
  virtual Operation min()  const = 0;
  virtual Operation maxloc() const = 0;
  virtual Operation minloc() const = 0;

protected:

  Comm();

  virtual ~Comm();

};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace mpi
}  // namespace eckit

#endif
