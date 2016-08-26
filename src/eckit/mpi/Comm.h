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

#include "eckit/mpi/Environment.h"

namespace eckit {
namespace mpi {

template <class T>
class Type {
    static const char* name();
    static int type();
};

template<> const char* Type<double>::name() { return "double"; }
template<> int Type<double>::type() { return MPI_DOUBLE; }

//----------------------------------------------------------------------------------------------------------------------

class Comm : private eckit::NonCopyable {

protected: // methods

    virtual int broadcast(void *buffer, size_t count, size_t datatype, size_t root) = 0;

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

    /// @brief Wait for Request to be completed, ignoring the return status
    virtual void wait(Request&) const = 0;

    /// Broadcast, pointer to data (also covers scalars)

    virtual int broadcast( int    data[], size_t size, size_t root ) const = 0;
    virtual int broadcast( long   data[], size_t size, size_t root ) const = 0;
    virtual int broadcast( float  data[], size_t size, size_t root ) const = 0;
    virtual int broadcast( double data[], size_t size, size_t root ) const = 0;
    virtual int broadcast( size_t data[], size_t size, size_t root ) const = 0;


    virtual int broadcast( int*    first, int* last, size_t root ) const = 0;
    virtual int broadcast( long*   first, int* last, size_t root ) const = 0;
    virtual int broadcast( float*  first, int* last, size_t root ) const = 0;
    virtual int broadcast( double* first, int* last, size_t root ) const = 0;
    virtual int broadcast( size_t* first, int* last, size_t root ) const = 0;
    virtual int broadcast( char*   first, int* last, size_t root ) const = 0;

    template<class T>
    int broadcast(const T& value, size_t root) {
        const T* p = &value;
        return broadcast(value, value+1, root);
    }

    template<class T>
    int broadcast(T* first, T* last, size_t root) {
        return broadcast(first, (last-first), Type<T>::type(), root);
    }

    ///

    virtual int broadcast( std::vector<int>    &data, size_t root, bool resize = true ) const = 0;
    virtual int broadcast( std::vector<long>   &data, size_t root, bool resize = true ) const = 0;
    virtual int broadcast( std::vector<float>  &data, size_t root, bool resize = true ) const = 0;
    virtual int broadcast( std::vector<double> &data, size_t root, bool resize = true ) const = 0;
    virtual int broadcast( std::vector<size_t> &data, size_t root, bool resize = true ) const = 0;

    ///

    virtual int all_reduce( const int    &send, int    &recv, const Operation& op) const = 0;
    virtual int all_reduce( const long   &send, long   &recv, const Operation& op) const = 0;
    virtual int all_reduce( const float  &send, float  &recv, const Operation& op) const = 0;
    virtual int all_reduce( const double &send, double &recv, const Operation& op) const = 0;

    ///

    virtual int all_reduce( const std::vector<int>    &send, std::vector<int>    &recv, const Operation& op) const = 0;
    virtual int all_reduce( const std::vector<long>   &send, std::vector<long>   &recv, const Operation& op) const = 0;
    virtual int all_reduce( const std::vector<float>  &send, std::vector<float>  &recv, const Operation& op) const = 0;
    virtual int all_reduce( const std::vector<double> &send, std::vector<double> &recv, const Operation& op) const = 0;

    ///

    virtual int all_reduce( int    &sendrecv, const Operation& op) const = 0;
    virtual int all_reduce( long   &sendrecv, const Operation& op) const = 0;
    virtual int all_reduce( float  &sendrecv, const Operation& op) const = 0;
    virtual int all_reduce( double &sendrecv, const Operation& op) const = 0;

    ///

    virtual int all_reduce( int    sendrecv[], size_t size, const Operation& op) const = 0;
    virtual int all_reduce( long   sendrecv[], size_t size, const Operation& op) const = 0;
    virtual int all_reduce( float  sendrecv[], size_t size, const Operation& op) const = 0;
    virtual int all_reduce( double sendrecv[], size_t size, const Operation& op) const = 0;

    ///

    virtual int all_reduce( std::vector<int>    &sendrecv, const Operation& op) const = 0;
    virtual int all_reduce( std::vector<long>   &sendrecv, const Operation& op) const = 0;
    virtual int all_reduce( std::vector<float>  &sendrecv, const Operation& op) const = 0;
    virtual int all_reduce( std::vector<double> &sendrecv, const Operation& op) const = 0;

    ///

    virtual int all_reduce( const std::pair<int,int>    &send, std::pair<int,int>    &recv, const Operation& op) const = 0;
    virtual int all_reduce( const std::pair<long,int>   &send, std::pair<long,int>   &recv, const Operation& op) const = 0;
    virtual int all_reduce( const std::pair<float,int>  &send, std::pair<float,int>  &recv, const Operation& op) const = 0;
    virtual int all_reduce( const std::pair<double,int> &send, std::pair<double,int> &recv, const Operation& op) const = 0;

    ///

    virtual int all_reduce( const std::vector< std::pair<int,int> >    &send, std::vector< std::pair<int,int> >    &recv, const Operation& op) const = 0;
    virtual int all_reduce( const std::vector< std::pair<long,int> >   &send, std::vector< std::pair<long,int> >   &recv, const Operation& op) const = 0;
    virtual int all_reduce( const std::vector< std::pair<float,int> >  &send, std::vector< std::pair<float,int> >  &recv, const Operation& op) const = 0;
    virtual int all_reduce( const std::vector< std::pair<double,int> > &send, std::vector< std::pair<double,int> > &recv, const Operation& op) const = 0;

    ///

    virtual int all_to_all( const std::vector< std::vector<int> >&    sendvec, std::vector< std::vector<int> >&    recvvec ) const = 0;
    virtual int all_to_all( const std::vector< std::vector<long> >&   sendvec, std::vector< std::vector<long> >&   recvvec ) const = 0;
    virtual int all_to_all( const std::vector< std::vector<float> >&  sendvec, std::vector< std::vector<float> >&  recvvec ) const = 0;
    virtual int all_to_all( const std::vector< std::vector<double> >& sendvec, std::vector< std::vector<double> >& recvvec ) const = 0;

    /// Gather methods from all, equal data sizes per rank, pointer to data (also covers scalar case)

    virtual int all_gather( const int    send[], int sendcnt, std::vector<int>&    recv ) const = 0;
    virtual int all_gather( const long   send[], int sendcnt, std::vector<long>&   recv ) const = 0;
    virtual int all_gather( const float  send[], int sendcnt, std::vector<float>&  recv ) const = 0;
    virtual int all_gather( const double send[], int sendcnt, std::vector<double>& recv ) const = 0;
    virtual int all_gather( const size_t send[], int sendcnt, std::vector<size_t>& recv ) const = 0;

    /// Gather methods from all, variable data sizes, std::vector of data

    virtual int all_gather( const int    send[], int sendcnt, mpi::Buffer<int>&    recv ) const = 0;
    virtual int all_gather( const long   send[], int sendcnt, mpi::Buffer<long>&   recv ) const = 0;
    virtual int all_gather( const float  send[], int sendcnt, mpi::Buffer<float>&  recv ) const = 0;
    virtual int all_gather( const double send[], int sendcnt, mpi::Buffer<double>& recv ) const = 0;

    ///

    virtual int all_gather( const std::vector<int>&    send, mpi::Buffer<int>&    recv ) const = 0;
    virtual int all_gather( const std::vector<long>&   send, mpi::Buffer<long>&   recv ) const = 0;
    virtual int all_gather( const std::vector<float>&  send, mpi::Buffer<float>&  recv ) const = 0;
    virtual int all_gather( const std::vector<double>& send, mpi::Buffer<double>& recv ) const = 0;

    ///

    virtual int all_gatherv(const std::vector<int>&    send, std::vector<int>&    recv, const std::vector<int>& displs) const = 0;
    virtual int all_gatherv(const std::vector<long>&   send, std::vector<long>&   recv, const std::vector<int>& displs) const = 0;
    virtual int all_gatherv(const std::vector<float>&  send, std::vector<float>&  recv, const std::vector<int>& displs) const = 0;
    virtual int all_gatherv(const std::vector<double>& send, std::vector<double>& recv, const std::vector<int>& displs) const = 0;

    /// Gather methods to one root, equal data sizes per rank, pointer to data (also covers scalar case)

    virtual int gather(const int* first, const int* last, int* result, size_t root ) const = 0;

    template <class T>
    int gather(const T send[], int sendcnt, std::vector<T>& recv, size_t root ) const {
        /// ASSERT something on recv ????
        return gather(&send[0], &send[sendcnt], &recv[0], root);
    }

    template <class T>
    int gather(const std::vector<T>& send, std::vector<T>& recv, size_t root ) const {
        /// ASSERT something on send ????
        /// ASSERT something on recv ????
        return gather(send.begin(), send.end(), recv.begin(), root);
    }

    virtual int gather(const float  send[], int sendcnt, std::vector<float>&  recv, size_t root ) const = 0;
    virtual int gather(const double send[], int sendcnt, std::vector<double>& recv, size_t root ) const = 0;
    virtual int gather(const size_t send[], int sendcnt, std::vector<size_t>& recv, size_t root ) const = 0;


    virtual int gather(const int    send[], int sendcnt, std::vector<int>&    recv, size_t root ) const = 0;
    virtual int gather(const long   send[], int sendcnt, std::vector<long>&   recv, size_t root ) const = 0;
    virtual int gather(const float  send[], int sendcnt, std::vector<float>&  recv, size_t root ) const = 0;
    virtual int gather(const double send[], int sendcnt, std::vector<double>& recv, size_t root ) const = 0;
    virtual int gather(const size_t send[], int sendcnt, std::vector<size_t>& recv, size_t root ) const = 0;

    /// Gather methods to one root, equal data sizes per rank, std::vector of data

    virtual int gather(const std::vector<int>&    send, std::vector<int>&    recv, size_t root ) const = 0;
    virtual int gather(const std::vector<long>&   send, std::vector<long>&   recv, size_t root ) const = 0;
    virtual int gather(const std::vector<float>&  send, std::vector<float>&  recv, size_t root ) const = 0;
    virtual int gather(const std::vector<double>& send, std::vector<double>& recv, size_t root ) const = 0;

    /// Gather methods to one root, variable data sizes per rank, pointer to data (also covers scalar case)

    virtual int gatherv(const int    send[], int sendcnt, int    recvbuf[], const std::vector<int>& recvcounts, const std::vector<int>& displs, size_t root ) const = 0;
    virtual int gatherv(const long   send[], int sendcnt, long   recvbuf[], const std::vector<int>& recvcounts, const std::vector<int>& displs, size_t root ) const = 0;
    virtual int gatherv(const float  send[], int sendcnt, float  recvbuf[], const std::vector<int>& recvcounts, const std::vector<int>& displs, size_t root ) const = 0;
    virtual int gatherv(const double send[], int sendcnt, double recvbuf[], const std::vector<int>& recvcounts, const std::vector<int>& displs, size_t root ) const = 0;

    /// Gather methods to one root, variable data sizes per rank, std::vector of data

    virtual int gatherv(const std::vector<int>&    send, mpi::Buffer<int>&    recv, size_t root ) const = 0;
    virtual int gatherv(const std::vector<long>&   send, mpi::Buffer<long>&   recv, size_t root ) const = 0;
    virtual int gatherv(const std::vector<float>&  send, mpi::Buffer<float>&  recv, size_t root ) const = 0;
    virtual int gatherv(const std::vector<double>& send, mpi::Buffer<double>& recv, size_t root ) const = 0;

    /// Scatter methods from one root, variable data sizes per rank, pointer to data (also covers scalar case)

    virtual int scatterv(const int    send[], const std::vector<int>& sendcounts, const std::vector<int>& displs, int    recvbuf[], int sendcnt, size_t root ) const = 0;
    virtual int scatterv(const long   send[], const std::vector<int>& sendcounts, const std::vector<int>& displs, long   recvbuf[], int sendcnt, size_t root ) const = 0;
    virtual int scatterv(const float  send[], const std::vector<int>& sendcounts, const std::vector<int>& displs, float  recvbuf[], int sendcnt, size_t root ) const = 0;
    virtual int scatterv(const double send[], const std::vector<int>& sendcounts, const std::vector<int>& displs, double recvbuf[], int sendcnt, size_t root ) const = 0;

    ///

    virtual int scatterv(const std::vector<int>&    send, const std::vector<int>& displs, mpi::Buffer<int>&    recv, size_t root ) const = 0;
    virtual int scatterv(const std::vector<long>&   send, const std::vector<int>& displs, mpi::Buffer<long>&   recv, size_t root ) const = 0;
    virtual int scatterv(const std::vector<float>&  send, const std::vector<int>& displs, mpi::Buffer<float>&  recv, size_t root ) const = 0;
    virtual int scatterv(const std::vector<double>& send, const std::vector<int>& displs, mpi::Buffer<double>& recv, size_t root ) const = 0;

    /// Non-blocking receive, pointer to data (also covers scalar case)
    virtual int receive(int    recv[], int count, int source, int tag, Request& req) const = 0;
    virtual int receive(long   recv[], int count, int source, int tag, Request& req) const = 0;
    virtual int receive(float  recv[], int count, int source, int tag, Request& req) const = 0;
    virtual int receive(double recv[], int count, int source, int tag, Request& req) const = 0;
    virtual int receive(size_t recv[], int count, int source, int tag, Request& req) const = 0;

    /// Non-blocking send, pointer to data (also covers scalar case)
    virtual int send(const int    send[], int count, int dest, int tag, Request& req) const = 0;
    virtual int send(const long   send[], int count, int dest, int tag, Request& req) const = 0;
    virtual int send(const float  send[], int count, int dest, int tag, Request& req) const = 0;
    virtual int send(const double send[], int count, int dest, int tag, Request& req) const = 0;
    virtual int send(const size_t send[], int count, int dest, int tag, Request& req) const = 0;

private: // methods

    virtual void print(std::ostream&) const = 0;

    friend std::ostream& operator<<(std::ostream& s, const Comm& o) {
        o.print(s); return s;
    }

protected: // methods

    Comm();

    virtual ~Comm();

};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace mpi
}  // namespace eckit

#endif
