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
#include <string>

#include "eckit/exception/Exceptions.h"
#include "eckit/memory/NonCopyable.h"

#include "eckit/mpi/Buffer.h"
#include "eckit/mpi/DataType.h"
#include "eckit/mpi/Operation.h"
#include "eckit/mpi/Request.h"
#include "eckit/mpi/Status.h"

namespace eckit {
namespace mpi {

//----------------------------------------------------------------------------------------------------------------------

/// @returns the communicator registered with associated name, or default communicator when NULL is passed
Comm& comm(const char* name = 0);

bool isRunning();

//----------------------------------------------------------------------------------------------------------------------

template <typename T> class Collectives;

class Comm : private eckit::NonCopyable {

public: // class methods

    static Comm& comm(const char* name = 0);

//    static Comm& create(const char* name);

    static bool areMPIVarsSet();

private: // class methods

    static void initDefaultComm();

protected: // methods

    virtual void initialize() = 0;

    virtual void finalize() = 0;

    virtual size_t getCount(Status& status, Data::Code datatype) const = 0;

    virtual void broadcast(void* buffer, size_t count, Data::Code datatype, size_t root) const = 0;

    virtual void gather(const void* sendbuf, size_t sendcount, void* recvbuf, size_t recvcount, Data::Code datatype, size_t root) const = 0;

    virtual void scatter(const void* sendbuf, size_t sendcount, void* recvbuf, size_t recvcount, Data::Code datatype, size_t root) const = 0;

    virtual void gatherv(const void* sendbuf, size_t sendcount, void* recvbuf, const int recvcounts[], const int displs[], Data::Code datatype, size_t root) const = 0;

    virtual void scatterv(const void* sendbuf, const int sendcounts[], const int displs[], void* recvbuf, size_t recvcount, Data::Code datatype, size_t root) const = 0;

    virtual void allReduce(const void* sendbuf, void* recvbuf, size_t count, Data::Code datatype, Operation::Code op) const = 0;

    virtual void allReduceInPlace(void* sendrecvbuf, size_t count, Data::Code datatype, Operation::Code op) const = 0;

    virtual void allGather(const void *sendbuf, size_t sendcount, void *recvbuf, size_t recvcount, Data::Code datatype) const = 0;

    virtual void allGatherv(const void *sendbuf, size_t sendcount,
                            void *recvbuf, const int recvcounts[], const int displs[], Data::Code datatype) const = 0;

    virtual void allToAll(const void* sendbuf, size_t sendcount, void* recvbuf, size_t recvcount, Data::Code datatype) const = 0;

    virtual void allToAllv(const void *sendbuf, const int sendcounts[], const int sdispls[],
                           void *recvbuf, const int recvcounts[], const int rdispls[],
                           Data::Code datatype) const = 0;

    virtual Status receive(void* recv, size_t count, Data::Code datatype, int source, int tag) const = 0;

    virtual void send(const void* send, size_t count, Data::Code datatype, int dest, int tag) const = 0;

    virtual Request iReceive(void* recv, size_t count, Data::Code datatype, int source, int tag) const = 0;

    virtual Request iSend(const void* send, size_t count, Data::Code datatype, int dest, int tag) const = 0;

public:  // methods

    /// @brief Returns name of processor according to MPI
    virtual std::string processorName() const = 0;

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
    virtual Status wait(Request&) const = 0;

    /// @brief Probe for incomming messages
    virtual Status probe(int source, int tag) const = 0;

    virtual int anySource() const = 0;
    virtual int anyTag() const = 0;

    virtual Status status() const = 0;
    virtual Request request() const = 0;

    template<typename T>
    size_t getCount(Status& status) const {
        return getCount(status, Data::Type<T>::code());
    }

    ///
    /// Broadcast, pointer to data (also covers scalars)
    ///

    template <typename T>
    void broadcast(T& value, size_t root) const {
        T* p = &value;
        broadcast(p, p+1, root);
    }

    template <typename T>
    void broadcast(T* first, T* last, size_t root) const {
        broadcast(first, (last-first), Data::Type<T>::code(), root);
    }

    template <typename T>
    void broadcast(T buffer[], size_t count, size_t root) const {
        broadcast(buffer, count, Data::Type<T>::code(), root);
    }

    template <typename T>
    void broadcast(typename std::vector<T>& v, size_t root) const {
        broadcast(v.begin(), v.end(), root);
    }

    template<class Iter>
    void broadcast(Iter first, Iter last, size_t root) const {
        typename std::iterator_traits<Iter>::difference_type n = std::distance(first, last);
        Data::Code type = Data::Type<typename std::iterator_traits<Iter>::value_type>::code();
        broadcast(&(*first), n, type, root);
    }

    ///
    /// Gather methods to one root, equal data sizes per rank
    ///

    template<class CIter, class Iter>
    void gather(CIter first, CIter last, Iter recv, size_t root) const {
        typename std::iterator_traits<CIter>::difference_type n = std::distance(first, last);
        Data::Code ctype = Data::Type<typename std::iterator_traits<CIter>::value_type>::code();
        Data::Code  type = Data::Type<typename std::iterator_traits<Iter>::value_type>::code();
        ASSERT(ctype == type);
        gather(&(*first), n, &(*recv), n, type, root);
    }

    template <typename T>
    void gather(const T* first, const T* last, T* result, size_t root ) const {
        size_t n = std::distance(first, last);
        gather(first, n, result, n, Data::Type<T>::code(), root);
    }

    template <typename T>
    void gather(const T send[], size_t sendcnt, T recv[], size_t recvcnt, size_t root ) const {
        if(root == rank()) { ASSERT(recvcnt >= sendcnt); }
        gather(send, sendcnt, recv, sendcnt, Data::Type<T>::code(), root);
    }

    template <typename T>
    void gather(const T send[], size_t sendcnt, std::vector<T>& recv, size_t root ) const {
        if(root == rank()) { ASSERT(recv.size() >= sendcnt); }
        gather(&send[0], sendcnt, &recv[0], sendcnt, Data::Type<T>::code(), root);
    }

    template <typename T>
    void gather(const std::vector<T>& send, std::vector<T>& recv, size_t root ) const {
        if(root == rank()) { ASSERT(recv.size() >= send.size()); }
        gather(send.begin(), send.end(), recv.begin(), root);
    }

    ///
    /// Gather methods to one root, variable data sizes per rank
    ///

    template<class CIter, class Iter>
    void gatherv(CIter first, CIter last, Iter recvbuf, const int recvcounts[], const int displs[], size_t root) const {
        typename std::iterator_traits<CIter>::difference_type n = std::distance(first, last);
        Data::Code ctype = Data::Type<typename std::iterator_traits<CIter>::value_type>::code();
        Data::Code  type = Data::Type<typename std::iterator_traits<Iter>::value_type>::code();
        ASSERT(ctype == type);
        gatherv(&(*first), n, &(*recvbuf), recvcounts, displs, type, root);
    }

    template<class CIter, class Iter>
    void gatherv(CIter first, CIter last, Iter recv, const std::vector<int>& recvcounts, const std::vector<int>& displs, size_t root) const {
        gatherv(first, last, recv, recvcounts.data(), displs.data(), root);
    }

    template <typename T>
    void gatherv(const std::vector<T>& send, std::vector<T>& recv, const std::vector<int>& recvcounts, const std::vector<int>& displs, size_t root ) const {
        ASSERT(recvcounts.size() == displs.size());
        gatherv(send.begin(), send.end(), recv.begin(), recvcounts.data(), displs.data(), root);
    }

    ///
    /// Scatter methods from one root, variable data sizes per rank, pointer to data (also covers scalar case)
    ///

    template<class CIter, class Iter>
    void scatterv(CIter first, const int sendcounts[], const int displs[], Iter recv, Iter recvend, size_t root) const {
        typename std::iterator_traits<Iter>::difference_type recvcounts = std::distance(recv, recvend);
        Data::Code ctype = Data::Type<typename std::iterator_traits<CIter>::value_type>::code();
        Data::Code  type = Data::Type<typename std::iterator_traits<Iter>::value_type>::code();
        ASSERT(ctype == type);
        scatterv(&(*first), sendcounts, displs, &(*recv), recvcounts, type, root);
    }

    template<class CIter, class Iter>
    void scatterv(CIter first, const std::vector<int>& sendcounts, const std::vector<int>& displs, Iter recv, Iter recvend, size_t root) const {
        scatterv(first, sendcounts.data(), displs.data(), recv, recvend, root);
    }

    ///
    /// All reduce operations, separate buffers
    ///

    template <typename T>
    void allReduce(const T* sendbuf, T* recvbuf, size_t count, Operation::Code op) const {
        allReduce(sendbuf, recvbuf, count, Data::Type<T>::code(), op);
    }

    template <typename T>
    void allReduce(T send, T& recv, Operation::Code op) const {
        allReduce(&send, &recv, 1, Data::Type<T>::code(), op);
    }

    template <typename T>
    void allReduce(const std::vector<T>& send, std::vector<T>& recv, Operation::Code op) const {
        ASSERT(send.size() == recv.size());
        allReduce(send.data(), recv.data(), send.size(), Data::Type<T>::code(), op);
    }

    ///
    /// All reduce operations, in place buffer
    ///

    template <typename T>
    void allReduceInPlace(T* sendrecvbuf, size_t count, Operation::Code op) const {
        allReduceInPlace(sendrecvbuf, count, Data::Type<T>::code(), op);
    }

    template <typename T>
    void allReduceInPlace(T& sendrecvbuf, Operation::Code op) const {
        allReduceInPlace(&sendrecvbuf, 1, Data::Type<T>::code(), op);
    }

    template<class Iter>
    void allReduceInPlace(Iter first, Iter last, Operation::Code op) const {
        typename std::iterator_traits<Iter>::difference_type count = std::distance(first, last);
        Data::Code  type = Data::Type<typename std::iterator_traits<Iter>::value_type>::code();
        allReduceInPlace(&(*first), count, type, op);
    }

    ///
    /// Gather methods from all, equal data sizes per rank
    ///

    template <typename T>
    void allGather(const T* sendbuf, size_t sendcount, T* recvbuf, size_t recvcount) const {
        ASSERT(recvcount >= size());
        allGather(sendbuf, sendcount, recvbuf, recvcount, Data::Type<T>::code());
    }

    template <typename T, typename Iter>
    void allGather(T sendval, Iter first, Iter last) const {
        typename std::iterator_traits<Iter>::difference_type recvcount = std::distance(first, last);
        ASSERT(recvcount >= size());
        Data::Code ctype = Data::Type<T>::code();
        Data::Code  type = Data::Type<typename std::iterator_traits<Iter>::value_type>::code();
        ASSERT(ctype == type);
        allGather(&sendval, 1, &(*first), recvcount, type);
    }

    ///
    /// Gather methods from all, variable data sizes per rank
    ///

    template <typename CIter, typename Iter>
    void allGatherv(CIter first, CIter last, Iter recvbuf, const int recvcounts[], const int displs[]) const {

        typename std::iterator_traits<CIter>::difference_type sendcount = std::distance(first, last);
        Data::Code ctype = Data::Type<typename std::iterator_traits<CIter>::value_type>::code();
        Data::Code  type = Data::Type<typename std::iterator_traits<CIter>::value_type>::code();
        ASSERT(ctype == type);
        allGatherv(&(*first), sendcount, &(*recvbuf), recvcounts, displs, type);
    }


    template <typename T, typename CIter>
    void allGatherv(CIter first, CIter last, mpi::Buffer<T>& recv) const {

        int sendcnt = int(std::distance(first, last));

        allGather(&sendcnt, 1, recv.counts.data(), 1);

        recv.displs[0] = 0;
        recv.cnt = recv.counts[0];
        size_t mpiSize = size();
        for(size_t jpart = 1; jpart < mpiSize; ++jpart) {
            recv.displs[jpart] = recv.displs[jpart-1] + recv.counts[jpart-1];
            recv.cnt += recv.counts[jpart];
        }

        recv.buffer.resize(recv.cnt);

        allGatherv(first, last, recv.buffer.data(), recv.counts.data(), recv.displs.data());
    }

    ///
    /// All to all methods, fixed data size
    ///

    template <typename T>
    void allToAll(const T* sendbuf, size_t sendcount, T* recvbuf, size_t recvcount) const {
        allToAll(sendbuf, sendcount, recvbuf, recvcount, Data::Type<T>::code());
    }

    ///
    /// All to All, variable data size
    ///

    template <typename T>
    void allToAllv(const T* sendbuf, const int sendcounts[], const int sdispls[],
                         T* recvbuf, const int recvcounts[], const int rdispls[]) const {
        allToAllv(sendbuf, sendcounts, sdispls, recvbuf, recvcounts, rdispls, Data::Type<T>::code());
    }

    ///
    ///  Non-blocking receive
    ///

    template <typename T>
    Request iReceive(T* recv, size_t count, int source, int tag) const {
        return iReceive(recv, count, Data::Type<T>::code(), source, tag);
    }

    template <typename T>
    Request iReceive(T& recv, int source, int tag) const {
        return iReceive(&recv, 1, Data::Type<T>::code(), source, tag);
    }

    ///
    ///  Blocking receive
    ///

    template <typename T>
    Status receive(T* recv, size_t count, int source, int tag) const {
        return receive(recv, count, Data::Type<T>::code(), source, tag);
    }

    template <typename T>
    Status receive(T& recv, int source, int tag) const {
        return receive(&recv, 1, Data::Type<T>::code(), source, tag);
    }

    ///
    /// Blocking send
    ///

    template <typename T>
    void send(T* sendbuf, size_t count, int dest, int tag) const {
        send(sendbuf, count, Data::Type<T>::code(), dest, tag);
    }

    template <typename T>
    void send(T sendbuf, int dest, int tag) const {
        send(&sendbuf, 1, Data::Type<T>::code(), dest, tag);
    }

    ///
    /// Non-blocking send
    ///

    template <typename T>
    Request iSend(T* sendbuf, size_t count, int dest, int tag) const {
        return iSend(sendbuf, count, Data::Type<T>::code(), dest, tag);
    }

    template <typename T>
    Request iSend(T sendbuf, int dest, int tag) const {
        return iSend(&sendbuf, 1, Data::Type<T>::code(), dest, tag);
    }

    ///
    /// All to all of vector< vector<> >
    ///
    template <typename T>
    void allToAll(const std::vector< std::vector<T> >& sendvec, std::vector< std::vector<T> >& recvvec) const;

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

class CommFactory {

    friend class eckit::mpi::Comm;

    std::string name_;
    virtual Comm* make() = 0;

  protected:

    CommFactory(const std::string &);
    virtual ~CommFactory();

    static void list(std::ostream &);
    static Comm* build(const std::string&);
};

template< class T>
class CommBuilder : public CommFactory {
    virtual Comm* make() {
        return new T();
    }
  public:
    CommBuilder(const std::string &name) : CommFactory(name) {}
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace mpi
}  // namespace eckit

//----------------------------------------------------------------------------------------------------------------------

#if 0

///
/// All reduce a pair
///

static void allReduce(const eckit::mpi::Comm& comm, const std::pair<T,int>& send, std::pair<T,int>& recv, Operation::Code op)  {
    NOTIMP;
}

///
/// All reduce a vector of pairs
///

static void allReduce(const eckit::mpi::Comm& comm, const std::vector< std::pair<T,int> >& send, std::vector< std::pair<T,int> >& recv, Operation::Code op)  {
    NOTIMP;
}

#endif

template <typename T>
void eckit::mpi::Comm::allToAll(const std::vector< std::vector<T> >& sendvec, std::vector< std::vector<T> >& recvvec) const {

    size_t mpi_size = size();

    ASSERT( sendvec.size() == mpi_size );
    ASSERT( recvvec.size() == mpi_size );

    // Get send-information
    std::vector<int> sendcounts(mpi_size);
    std::vector<int> senddispls(mpi_size);
    int sendcnt;
    senddispls[0] = 0;
    sendcounts[0] = sendvec[0].size();
    sendcnt = sendcounts[0];

    for(size_t jproc = 1; jproc < mpi_size; ++jproc)
    {
        senddispls[jproc] = senddispls[jproc-1] + sendcounts[jproc-1];
        sendcounts[jproc] = sendvec[jproc].size();
        sendcnt += sendcounts[jproc];
    }


    // Get recv-information
    std::vector<int> recvcounts(mpi_size);
    std::vector<int> recvdispls(mpi_size);
    int recvcnt;

    allToAll(sendcounts.data(), 1, recvcounts.data(), 1);

    recvdispls[0] = 0;
    recvcnt = recvcounts[0];
    for(size_t jproc = 1; jproc < mpi_size; ++jproc)
    {
        recvdispls[jproc] = recvdispls[jproc-1] + recvcounts[jproc-1];
        recvcnt += recvcounts[jproc];
    }

    // Communicate
    std::vector<T> sendbuf(sendcnt);
    std::vector<T> recvbuf(recvcnt);
    int cnt = 0;
    for(size_t jproc = 0; jproc < mpi_size; ++jproc)
    {
        for(int i = 0; i < sendcounts[jproc]; ++i)
        {
            sendbuf[cnt++] = sendvec[jproc][i];
        }
    }

    allToAllv( sendbuf.data(), sendcounts.data(), senddispls.data(),
               recvbuf.data(), recvcounts.data(), recvdispls.data());

    cnt=0;
    for(size_t jproc = 0; jproc < mpi_size; ++jproc)
    {
        recvvec[jproc].resize(recvcounts[jproc]);
        for(int i = 0; i < recvcounts[jproc]; ++i)
        {
            recvvec[jproc][i] = recvbuf[cnt++];
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------

#endif
