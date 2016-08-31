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
#include <iterator>
#include <vector>

#include "eckit/memory/NonCopyable.h"

#include "eckit/mpi/Environment.h"
#include "eckit/mpi/DataType.h"

namespace eckit {
namespace mpi {

//----------------------------------------------------------------------------------------------------------------------

class Comm : private eckit::NonCopyable {

protected: // methods

    virtual void broadcast(void* buffer, size_t count, Data::Code datatype, size_t root) const = 0;

    virtual void gather(const void* sendbuf, size_t sendcount, void* recvbuf, size_t recvcount, Data::Code datatype, size_t root) const = 0;

    virtual void scatter(const void* sendbuf, size_t sendcount, void* recvbuf, size_t recvcount, Data::Code datatype, size_t root) const = 0;

    virtual void gatherv(const void* sendbuf, size_t sendcount, void* recvbuf, const int recvcounts[], const int displs[], Data::Code datatype, size_t root) const = 0;

    virtual void scatterv(const void* sendbuf, const int sendcounts[], const int displs[], void* recvbuf, size_t recvcount, Data::Code datatype, size_t root) const = 0;

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
    virtual void abort(int errorcode = -1) const = 0;

    /// @brief Wait for Request to be completed, ignoring the return status
    virtual void wait(Request&) const = 0;

    ///
    /// Broadcast, pointer to data (also covers scalars)
    ///

    template<class T>
    void broadcast(T& value, size_t root) {
        T* p = &value;
        broadcast(p, p+1, root);
    }

    template<class T>
    void broadcast(T* first, T* last, size_t root) {
        broadcast(first, (last-first), Data::Type<T>::code(), root);
    }

    template<class T>
    void broadcast(typename std::vector<T>& v, size_t root) {
        broadcast(v.begin(), v.end(), root);
    }

    template<class Iter>
    void broadcast(Iter first, Iter last, size_t root) {
        typename std::iterator_traits<Iter>::difference_type n = std::distance(first, last);
        Data::Code type = Data::Type<typename std::iterator_traits<Iter>::value_type>::code();
        broadcast(&(*first), n, type, root);
    }

    ///
    /// Gather methods to one root, equal data sizes per rank
    ///

    template<class CIter, class Iter>
    void gather(CIter first, CIter last, Iter recv, size_t root) {
        typename std::iterator_traits<CIter>::difference_type n = std::distance(first, last);
        Data::Code ctype = Data::Type<typename std::iterator_traits<CIter>::value_type>::code();
        Data::Code  type = Data::Type<typename std::iterator_traits<Iter>::value_type>::code();
        ASSERT(ctype == type);
        gather(&(*first), n, &(*recv), n, type, root);
    }

    template <class T>
    void gather(const T* first, const T* last, T* result, size_t root ) const {
        size_t n = std::distance(first, last);
        gather(first, n, result, n, Data::Type<T>::code(), root);
    }

    template <class T>
    void gather(const T send[], size_t sendcnt, T recv[], size_t recvcnt, size_t root ) const {
        if(root == rank()) { ASSERT(recvcnt >= sendcnt); }
        gather(send, sendcnt, recv, sendcnt, Data::Type<T>::code(), root);
    }

    template <class T>
    void gather(const T send[], size_t sendcnt, std::vector<T>& recv, size_t root ) const {
        if(root == rank()) { ASSERT(recv.size() >= sendcnt); }
        gather(&send[0], sendcnt, &recv[0], sendcnt, Data::Type<T>::code(), root);
    }

    template <class T>
    void gather(const std::vector<T>& send, std::vector<T>& recv, size_t root ) const {
        if(root == rank()) { ASSERT(recv.size() >= send.size()); }
        gather(send.begin(), send.end(), recv.begin(), root);
    }

    ///
    /// Gather methods to one root, variable data sizes per rank
    ///

    template<class CIter, class Iter>
    void gatherv(CIter first, CIter last, Iter recvbuf, const int recvcounts[], const int displs[], size_t root) {
        typename std::iterator_traits<CIter>::difference_type n = std::distance(first, last);
        Data::Code ctype = Data::Type<typename std::iterator_traits<CIter>::value_type>::code();
        Data::Code  type = Data::Type<typename std::iterator_traits<Iter>::value_type>::code();
        ASSERT(ctype == type);
        gatherv(&(*first), n, &(*recvbuf), recvcounts, displs, type, root);
    }

    template<class CIter, class Iter>
    void gatherv(CIter first, CIter last, Iter recv, const std::vector<int>& recvcounts, const std::vector<int>& displs, size_t root) {
        gatherv(first, last, recv, recvcounts.data(), displs.data(), root);
    }

    template <class T>
    void gatherv(const std::vector<T>& send, std::vector<T>& recv, const std::vector<int>& recvcounts, const std::vector<int>& displs, size_t root ) const {
        ASSERT(recvcounts.size() == displs.size());
        gatherv(send.begin(), send.end(), recv.begin(), recvcounts.data(), displs.data(), root);
    }

    ///
    /// Scatter methods from one root, variable data sizes per rank, pointer to data (also covers scalar case)
    ///


    template<class CIter, class Iter>
    void scatterv(CIter first, const int sendcounts[], const int displs[], Iter recv, Iter recvend, size_t root) {
        typename std::iterator_traits<Iter>::difference_type recvcounts = std::distance(recv, recvend);
        Data::Code ctype = Data::Type<typename std::iterator_traits<CIter>::value_type>::code();
        Data::Code  type = Data::Type<typename std::iterator_traits<Iter>::value_type>::code();
        ASSERT(ctype == type);
        gatherv(&(*first), sendcounts, displs, &(*recv), recvcounts, type, root);
    }

    template<class CIter, class Iter>
    void scatterv(CIter first, const std::vector<int>& sendcounts, const std::vector<int>& displs, Iter recv, Iter recvend, size_t root) {
        scatterv(first, sendcounts.data(), displs.data(), recv, recvend, root);
    }

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
