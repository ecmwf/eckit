/*
 * (C) Copyright 1996- ECMWF.
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
#include <string>
#include <string_view>
#include <vector>

#include "eckit/filesystem/PathName.h"
#include "eckit/io/SharedBuffer.h"
#include "eckit/memory/NonCopyable.h"

#include "eckit/mpi/Buffer.h"
#include "eckit/mpi/DataType.h"
#include "eckit/mpi/Group.h"
#include "eckit/mpi/Operation.h"
#include "eckit/mpi/Request.h"
#include "eckit/mpi/Status.h"

namespace eckit::mpi {

//----------------------------------------------------------------------------------------------------------------------

class Environment;

/// @returns the communicator registered with associated name, or default communicator when NULL is
/// passed
Comm& comm(std::string_view name = {});

Comm& self();

/// Set a communicator as default
void setCommDefault(std::string_view name);

/// Register a communicator comming from Fortran code
void addComm(std::string_view name, int comm);

/// Register an existing communicator
void addComm(std::string_view name, Comm* comm);

/// Unregister and delete specific comm
/// @pre Comm is registered in the environment
void deleteComm(std::string_view name);

/// Check if a communicator is registered
bool hasComm(std::string_view name);

std::vector<std::string> listComms();

/// Finalises all the comms that are registered
///
/// @note This should not be necessary to be called, since all singletong Comms finalise themselves
/// on destruction
///       when application shutsdown. Currently there is a bug in OpenMPI on MacOSX (see ECKIT-166)
///       that implies that MPI_Finalize must be called explicitly before exiting main(). This is
///       the only current reason to use this function.
void finaliseAllComms();

namespace detail {
/// Assertions for eckit::mpi code
/// Don't use directly in client code
void Assert(int code, const char* msg, const char* file, int line, const char* func);
}  // namespace detail

//----------------------------------------------------------------------------------------------------------------------

class Comm : private eckit::NonCopyable {
    friend class Environment;

public:  // class methods

    static Comm& comm(std::string_view name = {});

public:  // methods

    std::string name() const { return name_; }

    /// @brief Returns name of processor according to MPI
    virtual std::string processorName() const = 0;

    /// @brief MPI rank of process in this communicator
    size_t rank() const { return rank_; }

    /// @brief MPI size of this communicator
    size_t size() const { return size_; }

    virtual size_t remoteSize() const = 0;

    /// @brief MPI barrier for this communicator
    virtual void barrier() const = 0;

    /// @brief MPI non-blocking barrier for this communicator
    virtual Request iBarrier() const = 0;

    /// @brief MPI abort for this communicator
    virtual void abort(int errorcode = -1) const = 0;

    /// @brief Wait for Request to be completed, ignoring the return status
    virtual Status wait(Request&) const = 0;

    /// @brief Wait for one request out of a vector of requests to finish
    ///
    /// @param[in]  requests The requests that one will be waited for.
    /// @param[out] index    The request index in requests that has been waited for.
    virtual Status waitAny(std::vector<Request>& requests, int& index) const = 0;

    /// @brief Wait for all given requests to finish
    virtual std::vector<Status> waitAll(std::vector<Request>&) const = 0;

    /// @brief Probe for incoming messages (blocking)
    virtual Status probe(int source, int tag) const = 0;

    /// @brief Probe for incoming messages (non-blocking)
    /// Use status.error() to check if there is an incoming message
    /// status.error() == 0 if there is an incoming message
    virtual Status iProbe(int source, int tag) const = 0;

    virtual int procNull() const = 0;

    virtual int undefined() const = 0;

    virtual int anySource() const = 0;

    virtual int anyTag() const = 0;

    virtual Status status() const = 0;

    virtual Request request(int) const = 0;

    virtual Group group(int) const = 0;

    virtual Group group() const = 0;

    virtual Group remoteGroup() const = 0;

    virtual Comm& create(const Group&, const std::string& name) const = 0;

    virtual Comm& create(const Group&, int tag, const std::string& name) const = 0;

    template <typename T>
    size_t getCount(Status& status) const;

    ///
    /// Broadcast, pointer to data (also covers scalars)
    ///

    template <typename T>
    void broadcast(T& value, size_t root) const;

    template <typename T>
    void broadcast(T* first, T* last, size_t root) const;

    template <typename T>
    void broadcast(T buffer[], size_t count, size_t root) const;

    template <typename T>
    void broadcast(typename std::vector<T>& v, size_t root) const;

    template <class Iter>
    void broadcast(Iter first, Iter last, size_t root) const;

    ///
    /// Gather methods to one root, equal data sizes per rank
    ///

    template <class CIter, class Iter>
    void gather(CIter first, CIter last, Iter rfirst, Iter rlast, size_t root) const;

    template <typename T>
    void gather(const T send, std::vector<T>& recv, size_t root) const;

    template <typename T>
    void gather(const std::vector<T>& send, std::vector<T>& recv, size_t root) const;

    ///
    /// Gather methods to one root, variable data sizes per rank
    ///

    template <typename Value>
    void gatherv(const Value* sendbuf, size_t sendcount, Value* recvbuf, const int recvcounts[], const int displs[],
                 size_t root) const;

    template <class CIter, class Iter>
    void gatherv(CIter first, CIter last, Iter rfirst, Iter rlast, const int recvcounts[], const int displs[],
                 size_t root) const;

    template <class CIter, class Iter>
    void gatherv(CIter first, CIter last, Iter rfirst, Iter rlast, const std::vector<int>& recvcounts,
                 const std::vector<int>& displs, size_t root) const;

    template <typename T>
    void gatherv(const std::vector<T>& send, std::vector<T>& recv, const std::vector<int>& recvcounts,
                 const std::vector<int>& displs, size_t root) const;

    ///
    /// Scatter methods from one root
    ///

    template <typename T>
    void scatter(const std::vector<T>& send, T& recv, size_t root) const;

    template <typename T>
    void scatter(const std::vector<T>& send, std::vector<T>& recv, size_t root) const;

    ///
    /// Scatter methods from one root, variable data sizes per rank, pointer to data (also covers
    /// scalar case)
    ///

    template <typename Value>
    void scatterv(const Value* sendbuf, const int sendcounts[], const int displs[], Value* recvbuf, int recvcount,
                  size_t root) const;

    template <class CIter, class Iter>
    void scatterv(CIter first, CIter last, const int sendcounts[], const int displs[], Iter rfirst, Iter rlast,
                  size_t root) const;

    template <class CIter, class Iter>
    void scatterv(CIter first, CIter last, const std::vector<int>& sendcounts, const std::vector<int>& displs,
                  Iter rfirst, Iter rlast, size_t root) const;

    ///
    /// Reduce operations, separate buffers
    ///

    template <typename T>
    void reduce(const T send, T& recv, Operation::Code op, size_t root) const;

    template <typename T>
    void reduce(const T* send, T* recv, size_t count, Operation::Code op, size_t root) const;

    template <typename T>
    void reduce(const std::vector<T>& send, std::vector<T>& recv, Operation::Code op, size_t root) const;

    ///
    /// Reduce operations, in place buffer
    ///

    template <typename T>
    void reduceInPlace(T* sendrecvbuf, size_t count, Operation::Code op, size_t root) const;

    template <typename T>
    void reduceInPlace(T& sendrecvbuf, Operation::Code op, size_t root) const;

    template <class Iter>
    void reduceInPlace(Iter first, Iter last, Operation::Code op, size_t root) const;

    ///
    /// All reduce operations, separate buffers
    ///

    template <typename T>
    void allReduce(const T send, T& recv, Operation::Code op) const;

    template <typename T>
    void allReduce(const T* send, T* recv, size_t count, Operation::Code op) const;

    template <typename T>
    void allReduce(const std::vector<T>& send, std::vector<T>& recv, Operation::Code op) const;

    ///
    /// All reduce operations, in place buffer
    ///

    template <typename T>
    void allReduceInPlace(T* sendrecvbuf, size_t count, Operation::Code op) const;

    template <typename T>
    void allReduceInPlace(T& sendrecvbuf, Operation::Code op) const;

    template <class Iter>
    void allReduceInPlace(Iter first, Iter last, Operation::Code op) const;

    ///
    /// Gather methods from all, equal data sizes per rank
    ///

    template <typename T, typename Iter>
    void allGather(const T sendval, Iter rfirst, Iter rlast) const;

    ///
    /// Gather methods from all, variable data sizes per rank
    ///

    template <typename CIter, typename Iter>
    void allGatherv(CIter first, CIter last, Iter recvbuf, const int recvcounts[], const int displs[]) const;

    template <typename T, typename CIter>
    void allGatherv(CIter first, CIter last, mpi::Buffer<T>& recv) const;

    ///
    /// All to all methods, fixed data size
    ///

    template <typename T>
    void allToAll(const std::vector<T>& send, std::vector<T>& recv) const;

    ///
    /// All to All, variable data size
    ///

    template <typename T>
    void allToAllv(const T* sendbuf, const int sendcounts[], const int sdispls[], T* recvbuf, const int recvcounts[],
                   const int rdispls[]) const;

    ///
    ///  Non-blocking receive
    ///

    template <typename T>
    Request iReceive(T* recv, size_t count, int source, int tag) const;

    template <typename T>
    Request iReceive(T& recv, int source, int tag) const;

    ///
    ///  Blocking receive
    ///

    template <typename T>
    Status receive(T* recv, size_t count, int source, int tag) const;

    template <typename T>
    Status receive(T& recv, int source, int tag) const;

    ///
    /// Blocking send
    ///

    template <typename T>
    void send(const T* sendbuf, size_t count, int dest, int tag) const;

    template <typename T>
    void send(const T& sendbuf, int dest, int tag) const;

    ///
    /// Blocking send, until message received
    ///

    template <typename T>
    void synchronisedSend(const T* sendbuf, size_t count, int dest, int tag) const;

    template <typename T>
    void synchronisedSend(const T& sendbuf, int dest, int tag) const;


    ///
    /// Non-blocking send
    ///

    template <typename T>
    Request iSend(const T* sendbuf, size_t count, int dest, int tag) const;

    template <typename T>
    Request iSend(const T& sendbuf, int dest, int tag) const;

    ///
    /// In place simultaneous send and receive
    ///

    template <typename T>
    Status sendReceiveReplace(T* sendrecv, size_t count, int dest, int sendtag, int source, int recvtag) const;

    template <typename T>
    Status sendReceiveReplace(T& sendrecv, int dest, int sendtag, int source, int recvtag) const;

    ///
    /// All to all of vector< vector<> >
    ///

    template <typename T>
    void allToAll(const std::vector<std::vector<T> >& sendvec, std::vector<std::vector<T> >& recvvec) const;

    ///
    /// Read file on one rank, and broadcast
    ///

    virtual eckit::SharedBuffer broadcastFile(const eckit::PathName& filepath, size_t root) const = 0;

    /// @brief Split the communicator based on color & give the new communicator a name
    virtual Comm& split(int color, const std::string& name) const = 0;

    /// @brief The communicator
    virtual int communicator() const = 0;

protected:  // methods

    virtual size_t getCount(Status& status, Data::Code datatype) const = 0;

    virtual void broadcast(void* buffer, size_t count, Data::Code datatype, size_t root) const = 0;

    virtual void gather(const void* sendbuf, size_t sendcount, void* recvbuf, size_t recvcount, Data::Code datatype,
                        size_t root) const = 0;

    virtual void scatter(const void* sendbuf, size_t sendcount, void* recvbuf, size_t recvcount, Data::Code datatype,
                         size_t root) const = 0;

    virtual void gatherv(const void* sendbuf, size_t sendcount, void* recvbuf, const int recvcounts[],
                         const int displs[], Data::Code datatype, size_t root) const = 0;

    virtual void scatterv(const void* sendbuf, const int sendcounts[], const int displs[], void* recvbuf,
                          size_t recvcount, Data::Code datatype, size_t root) const = 0;

    virtual void reduce(const void* sendbuf, void* recvbuf, size_t count, Data::Code datatype, Operation::Code op,
                        size_t root) const = 0;

    virtual void reduceInPlace(void* sendrecvbuf, size_t count, Data::Code datatype, Operation::Code op,
                               size_t root) const = 0;

    virtual void allReduce(const void* sendbuf, void* recvbuf, size_t count, Data::Code datatype,
                           Operation::Code op) const = 0;

    virtual void allReduceInPlace(void* sendrecvbuf, size_t count, Data::Code datatype, Operation::Code op) const = 0;

    virtual void allGather(const void* sendbuf, size_t sendcount, void* recvbuf, size_t recvcount,
                           Data::Code datatype) const = 0;

    virtual void allGatherv(const void* sendbuf, size_t sendcount, void* recvbuf, const int recvcounts[],
                            const int displs[], Data::Code datatype) const = 0;

    virtual void allToAll(const void* sendbuf, size_t sendcount, void* recvbuf, size_t recvcount,
                          Data::Code datatype) const = 0;

    virtual void allToAllv(const void* sendbuf, const int sendcounts[], const int sdispls[], void* recvbuf,
                           const int recvcounts[], const int rdispls[], Data::Code datatype) const = 0;

    virtual Status receive(void* recv, size_t count, Data::Code datatype, int source, int tag) const = 0;

    virtual void send(const void* send, size_t count, Data::Code datatype, int dest, int tag) const = 0;

    virtual void synchronisedSend(const void* send, size_t count, Data::Code datatype, int dest, int tag) const = 0;

    virtual Request iReceive(void* recv, size_t count, Data::Code datatype, int source, int tag) const = 0;

    virtual Request iSend(const void* send, size_t count, Data::Code datatype, int dest, int tag) const = 0;

    virtual Status sendReceiveReplace(void* sendrecv, size_t count, Data::Code datatype, int dest, int sendtag,
                                      int source, int recvtag) const = 0;

    /// @brief Call free on this communicator
    /// After calling this method, the communicator should not be used again
    /// This is protected so only Environment can call it, typically via the deleteComm() method
    virtual void free() = 0;

    /// @brief Creates a communicator to self
    virtual eckit::mpi::Comm* self() const = 0;

private:  // methods

    virtual void print(std::ostream&) const = 0;

    friend std::ostream& operator<<(std::ostream& s, const Comm& o) {
        o.print(s);
        return s;
    }

protected:  // methods

    Comm(std::string_view name);

    virtual ~Comm();

    std::string name_;
    size_t rank_;
    size_t size_;
};

//----------------------------------------------------------------------------------------------------------------------

class CommFactory {
    friend class eckit::mpi::Environment;

    std::string builder_;
    virtual Comm* make(std::string_view name)      = 0;
    virtual Comm* make(std::string_view name, int) = 0;

protected:

    CommFactory(std::string_view builder);
    virtual ~CommFactory();

    static Comm* build(std::string_view name, std::string_view builder);
    static Comm* build(std::string_view name, std::string_view builder, int);
};

template <class T>
class CommBuilder : public CommFactory {
    Comm* make(std::string_view name) override { return new T(name); }
    Comm* make(std::string_view name, int comm) override { return new T(name, comm); }

public:

    CommBuilder(const std::string& builder) : CommFactory(builder) {}
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::mpi

//----------------------------------------------------------------------------------------------------------------------

#if defined(__clang_analyzer__)
// By adding C assert, the analyzer can make assumptions on conditions, preventing false positive warnings
#include <cassert>
#define ECKIT_MPI_ASSERT(a) eckit::mpi::detail::Assert(!(a), #a, __FILE__, __LINE__, __func__); \
                            assert(a)
#else
#define ECKIT_MPI_ASSERT(a) eckit::mpi::detail::Assert(!(a), #a, __FILE__, __LINE__, __func__)
#endif

template <typename T>
size_t eckit::mpi::Comm::getCount(Status& status) const {
    return getCount(status, Data::Type<T>::code());
}

///
/// Broadcast, pointer to data (also covers scalars)
///

template <typename T>
void eckit::mpi::Comm::broadcast(T& value, size_t root) const {
    size_t commsize = size();
    ECKIT_MPI_ASSERT(root < commsize);

    T* p = &value;

    broadcast(p, p + 1, root);
}

template <typename T>
void eckit::mpi::Comm::broadcast(T* first, T* last, size_t root) const {
    size_t commsize = size();
    ECKIT_MPI_ASSERT(root < commsize);

    broadcast(first, (last - first), Data::Type<T>::code(), root);
}

template <typename T>
void eckit::mpi::Comm::broadcast(T buffer[], size_t count, size_t root) const {
    size_t commsize = size();
    ECKIT_MPI_ASSERT(root < commsize);

    broadcast(buffer, count, Data::Type<T>::code(), root);
}

template <typename T>
void eckit::mpi::Comm::broadcast(typename std::vector<T>& v, size_t root) const {
    size_t commsize = size();
    ECKIT_MPI_ASSERT(root < commsize);

    broadcast(v.begin(), v.end(), root);
}

template <class Iter>
void eckit::mpi::Comm::broadcast(Iter first, Iter last, size_t root) const {
    size_t commsize = size();
    ECKIT_MPI_ASSERT(root < commsize);

    typename std::iterator_traits<Iter>::difference_type n = std::distance(first, last);
    Data::Code type = Data::Type<typename std::iterator_traits<Iter>::value_type>::code();

    broadcast(&(*first), n, type, root);
}

///
/// Gather methods to one root, equal data sizes per rank
///

template <class CIter, class Iter>
void eckit::mpi::Comm::gather(CIter first, CIter last, Iter rfirst, Iter rlast, size_t root) const {
    typedef typename std::iterator_traits<CIter>::difference_type diff_t;

    const size_t commsize = size();
    ECKIT_MPI_ASSERT(commsize > 0);
    ECKIT_MPI_ASSERT(root < commsize);

    const diff_t sendcount = std::distance(first, last);
    const diff_t rsize     = std::distance(rfirst, rlast);
    ECKIT_MPI_ASSERT(rsize % commsize == 0); /* receiving size is multiple of comm().size() */
    const diff_t recvcount = rsize / commsize;
    ECKIT_MPI_ASSERT(sendcount == recvcount);

    using CValue     = typename std::iterator_traits<CIter>::value_type;
    using Value      = typename std::iterator_traits<Iter>::value_type;
    Data::Code ctype = Data::Type<CValue>::code();
    Data::Code type  = Data::Type<Value>::code();
    ECKIT_MPI_ASSERT(ctype == type);

    const CValue* sendbuf = (first != last) ? &(*first) : nullptr;
    Value* recvbuf        = (rfirst != rlast) ? &(*rfirst) : nullptr;
    gather(sendbuf, sendcount, recvbuf, recvcount, type, root);
}

template <typename T>
void eckit::mpi::Comm::gather(const T send, std::vector<T>& recv, size_t root) const {
    size_t commsize = size();
    ECKIT_MPI_ASSERT(commsize > 0);
    ECKIT_MPI_ASSERT(root < commsize);
    ECKIT_MPI_ASSERT(recv.size() % commsize == 0); /* receiving size is multiple of comm().size() */
    size_t recvcount = recv.size() / commsize;
    size_t sendcount = 1;
    ECKIT_MPI_ASSERT(recvcount == sendcount);

    gather(&send, sendcount, recv.data(), recvcount, Data::Type<T>::code(), root);
}

template <typename T>
void eckit::mpi::Comm::gather(const std::vector<T>& send, std::vector<T>& recv, size_t root) const {
    size_t commsize = size();
    ECKIT_MPI_ASSERT(commsize > 0);
    ECKIT_MPI_ASSERT(root < commsize);
    ECKIT_MPI_ASSERT(recv.size() % commsize == 0); /* receiving size is multiple of comm().size() */
    size_t recvcount = recv.size() / commsize;
    ECKIT_MPI_ASSERT(send.size() == recvcount);

    gather(send.data(), send.size(), recv.data(), recvcount, Data::Type<T>::code(), root);
}

///
/// Gather methods to one root, variable data sizes per rank
///


template <typename Value>
void eckit::mpi::Comm::gatherv(const Value* sendbuf, size_t sendcount, Value* recvbuf, const int recvcounts[],
                               const int displs[], size_t root) const {
    gatherv(sendbuf, sendcount, recvbuf, recvcounts, displs, Data::Type<Value>::code(), root);
}

template <class CIter, class Iter>
void eckit::mpi::Comm::gatherv(CIter first, CIter last, Iter rfirst, Iter rlast, const int recvcounts[],
                               const int displs[], size_t root) const {
    typename std::iterator_traits<CIter>::difference_type sendcount = std::distance(first, last);
    // typename std::iterator_traits<CIter>::difference_type recvsize  = std::distance(rfirst,
    // rlast);

    using CValue     = typename std::iterator_traits<CIter>::value_type;
    using Value      = typename std::iterator_traits<Iter>::value_type;
    Data::Code ctype = Data::Type<CValue>::code();
    Data::Code type  = Data::Type<Value>::code();
    ECKIT_MPI_ASSERT(ctype == type);

    const CValue* sendbuf = (first != last) ? &(*first) : nullptr;
    Value* recvbuf        = (rfirst != rlast) ? &(*rfirst) : nullptr;
    gatherv(sendbuf, sendcount, recvbuf, recvcounts, displs, type, root);
}

template <class CIter, class Iter>
void eckit::mpi::Comm::gatherv(CIter first, CIter last, Iter rfirst, Iter rlast, const std::vector<int>& recvcounts,
                               const std::vector<int>& displs, size_t root) const {
    size_t commsize = size();
    ECKIT_MPI_ASSERT(root < commsize);
    ECKIT_MPI_ASSERT(recvcounts.size() == commsize);
    ECKIT_MPI_ASSERT(displs.size() == commsize);

    gatherv(first, last, rfirst, rlast, recvcounts.data(), displs.data(), root);
}

template <typename T>
void eckit::mpi::Comm::gatherv(const std::vector<T>& send, std::vector<T>& recv, const std::vector<int>& recvcounts,
                               const std::vector<int>& displs, size_t root) const {
    size_t commsize = size();
    ECKIT_MPI_ASSERT(root < commsize);
    if (rank() == root) {
        ECKIT_MPI_ASSERT(recvcounts.size() == commsize);
        ECKIT_MPI_ASSERT(displs.size() == commsize);
        ECKIT_MPI_ASSERT(recv.size() >= displs[commsize - 1] + recvcounts[commsize - 1]);
    }

    gatherv(send.begin(), send.end(), recv.begin(), recv.end(), recvcounts.data(), displs.data(), root);
}

///
/// Scatter methods from one root
///

template <typename T>
void eckit::mpi::Comm::scatter(const std::vector<T>& send, T& recv, size_t root) const {
    size_t commsize = size();
    ECKIT_MPI_ASSERT(commsize > 0);
    ECKIT_MPI_ASSERT(root < commsize);
    ECKIT_MPI_ASSERT(send.size() % commsize == 0);
    size_t sendcount = send.size() / commsize;
    size_t recvcount = 1;
    ECKIT_MPI_ASSERT(recvcount == sendcount);

    scatter(send.data(), sendcount, &recv, recvcount, Data::Type<T>::code(), root);
}

template <typename T>
void eckit::mpi::Comm::scatter(const std::vector<T>& send, std::vector<T>& recv, size_t root) const {
    size_t commsize = size();
    ECKIT_MPI_ASSERT(commsize > 0);
    ECKIT_MPI_ASSERT(root < commsize);
    ECKIT_MPI_ASSERT(send.size() % commsize == 0);
    size_t sendcount = send.size() / commsize;
    size_t recvcount = recv.size();
    ECKIT_MPI_ASSERT(recvcount == sendcount);

    scatter(send.data(), sendcount, recv.data(), recvcount, Data::Type<T>::code(), root);
}

///
/// Scatter methods from one root, variable data sizes per rank, pointer to data (also covers scalar
/// case)
///

template <typename Value>
void eckit::mpi::Comm::scatterv(const Value* sendbuf, const int sendcounts[], const int displs[], Value* recvbuf,
                                int recvcount, size_t root) const {
    size_t commsize = size();
    ECKIT_MPI_ASSERT(root < commsize);
    scatterv(sendbuf, sendcounts, displs, recvbuf, recvcount, Data::Type<Value>::code(), root);
}

template <class CIter, class Iter>
void eckit::mpi::Comm::scatterv(CIter first, CIter last, const int sendcounts[], const int displs[], Iter rfirst,
                                Iter rlast, size_t root) const {
    size_t commsize = size();
    ECKIT_MPI_ASSERT(root < commsize);

    typename std::iterator_traits<Iter>::difference_type recvcounts = std::distance(rfirst, rlast);
    // typename std::iterator_traits<Iter>::difference_type sendsize   = std::distance(first, last);

    using CValue     = typename std::iterator_traits<CIter>::value_type;
    using Value      = typename std::iterator_traits<Iter>::value_type;
    Data::Code ctype = Data::Type<CValue>::code();
    Data::Code type  = Data::Type<Value>::code();
    ECKIT_MPI_ASSERT(ctype == type);

    const CValue* sendbuf = (first != last) ? &(*first) : nullptr;
    Value* recvbuf        = (rfirst != rlast) ? &(*rfirst) : nullptr;
    scatterv(sendbuf, sendcounts, displs, recvbuf, recvcounts, type, root);
}

template <class CIter, class Iter>
void eckit::mpi::Comm::scatterv(CIter first, CIter last, const std::vector<int>& sendcounts,
                                const std::vector<int>& displs, Iter rfirst, Iter rlast, size_t root) const {
    size_t commsize = size();
    ECKIT_MPI_ASSERT(root < commsize);
    ECKIT_MPI_ASSERT(sendcounts.size() == commsize);
    ECKIT_MPI_ASSERT(displs.size() == commsize);

    scatterv(first, last, sendcounts.data(), displs.data(), rfirst, rlast, root);
}

///
/// Reduce operations, separate buffers
///

template <typename T>
void eckit::mpi::Comm::reduce(const T send, T& recv, Operation::Code op, size_t root) const {
    reduce(&send, &recv, 1, Data::Type<T>::code(), op, root);
}

template <typename T>
void eckit::mpi::Comm::reduce(const T* send, T* recv, size_t count, Operation::Code op, size_t root) const {
    reduce(send, recv, count, Data::Type<T>::code(), op, root);
}

template <typename T>
void eckit::mpi::Comm::reduce(const std::vector<T>& send, std::vector<T>& recv, Operation::Code op, size_t root) const {
    ECKIT_MPI_ASSERT(send.size() == recv.size());
    reduce(send.data(), recv.data(), send.size(), Data::Type<T>::code(), op, root);
}

///
/// Reduce operations, in place buffer
///

template <typename T>
void eckit::mpi::Comm::reduceInPlace(T* sendrecvbuf, size_t count, Operation::Code op, size_t root) const {
    reduceInPlace(sendrecvbuf, count, Data::Type<T>::code(), op, root);
}

template <typename T>
void eckit::mpi::Comm::reduceInPlace(T& sendrecvbuf, Operation::Code op, size_t root) const {
    reduceInPlace(&sendrecvbuf, 1, Data::Type<T>::code(), op, root);
}

template <class Iter>
void eckit::mpi::Comm::reduceInPlace(Iter first, Iter last, Operation::Code op, size_t root) const {
    typename std::iterator_traits<Iter>::difference_type count = std::distance(first, last);
    Data::Code type = Data::Type<typename std::iterator_traits<Iter>::value_type>::code();
    reduceInPlace(&(*first), count, type, op, root);
}

///
/// All reduce operations, separate buffers
///

template <typename T>
void eckit::mpi::Comm::allReduce(const T send, T& recv, Operation::Code op) const {
    allReduce(&send, &recv, 1, Data::Type<T>::code(), op);
}

template <typename T>
void eckit::mpi::Comm::allReduce(const T* send, T* recv, size_t count, Operation::Code op) const {
    allReduce(send, recv, count, Data::Type<T>::code(), op);
}

template <typename T>
void eckit::mpi::Comm::allReduce(const std::vector<T>& send, std::vector<T>& recv, Operation::Code op) const {
    ECKIT_MPI_ASSERT(send.size() == recv.size());
    allReduce(send.data(), recv.data(), send.size(), Data::Type<T>::code(), op);
}

///
/// All reduce operations, in place buffer
///

template <typename T>
void eckit::mpi::Comm::allReduceInPlace(T* sendrecvbuf, size_t count, Operation::Code op) const {
    allReduceInPlace(sendrecvbuf, count, Data::Type<T>::code(), op);
}

template <typename T>
void eckit::mpi::Comm::allReduceInPlace(T& sendrecvbuf, Operation::Code op) const {
    allReduceInPlace(&sendrecvbuf, 1, Data::Type<T>::code(), op);
}

template <class Iter>
void eckit::mpi::Comm::allReduceInPlace(Iter first, Iter last, Operation::Code op) const {
    typename std::iterator_traits<Iter>::difference_type count = std::distance(first, last);
    Data::Code type = Data::Type<typename std::iterator_traits<Iter>::value_type>::code();
    allReduceInPlace(&(*first), count, type, op);
}

///
/// Gather methods from all, equal data sizes per rank
///

template <typename T, typename Iter>
void eckit::mpi::Comm::allGather(T sendval, Iter rfirst, Iter rlast) const {
    Data::Code ctype = Data::Type<T>::code();
    Data::Code type  = Data::Type<typename std::iterator_traits<Iter>::value_type>::code();
    ECKIT_MPI_ASSERT(ctype == type);

    typename std::iterator_traits<Iter>::difference_type rsize = std::distance(rfirst, rlast);
    ECKIT_MPI_ASSERT(rsize % size() == 0);
    size_t recvcount = rsize / size();
    ECKIT_MPI_ASSERT(recvcount == 1);

    allGather(&sendval, 1, &(*rfirst), recvcount, type);
}

///
/// Gather methods from all, variable data sizes per rank
///

template <typename CIter, typename Iter>
void eckit::mpi::Comm::allGatherv(CIter first, CIter last, Iter rfirst, const int recvcounts[],
                                  const int displs[]) const {
    typename std::iterator_traits<CIter>::difference_type sendcount = std::distance(first, last);
    int recvcount                                                   = 0;
    int commsize                                                    = static_cast<int>(size());
    for (int i = 0; i < commsize; ++i) {
        recvcount += recvcounts[i];
    }
    using Value      = typename std::iterator_traits<CIter>::value_type;
    Data::Code ctype = Data::Type<Value>::code();
    Data::Code type  = Data::Type<typename std::iterator_traits<CIter>::value_type>::code();
    ECKIT_MPI_ASSERT(ctype == type);

    const Value* sendbuf = (sendcount > 0) ? &(*first) : nullptr;
    Value* recvbuf       = (recvcount > 0) ? &(*rfirst) : nullptr;
    allGatherv(sendbuf, sendcount, recvbuf, recvcounts, displs, type);
}

///
/// All to all methods, fixed data size
///

template <typename T>
void eckit::mpi::Comm::allToAll(const std::vector<T>& send, std::vector<T>& recv) const {
    size_t commsize = size();
    ECKIT_MPI_ASSERT(commsize > 0);
    ECKIT_MPI_ASSERT(send.size() % commsize == 0);
    ECKIT_MPI_ASSERT(recv.size() % commsize == 0);

    size_t sendcount = send.size() / commsize;
    size_t recvcount = recv.size() / commsize;

    allToAll(send.data(), sendcount, recv.data(), recvcount, Data::Type<T>::code());
}

///
/// All to All, variable data size
///

template <typename T>
void eckit::mpi::Comm::allToAllv(const T* sendbuf, const int sendcounts[], const int sdispls[], T* recvbuf,
                                 const int recvcounts[], const int rdispls[]) const {
    allToAllv(sendbuf, sendcounts, sdispls, recvbuf, recvcounts, rdispls, Data::Type<T>::code());
}

///
///  Non-blocking receive
///

template <typename T>
eckit::mpi::Request eckit::mpi::Comm::iReceive(T* recv, size_t count, int source, int tag) const {
    return iReceive(recv, count, Data::Type<T>::code(), source, tag);
}

template <typename T>
eckit::mpi::Request eckit::mpi::Comm::iReceive(T& recv, int source, int tag) const {
    return iReceive(&recv, 1, Data::Type<T>::code(), source, tag);
}

///
///  Blocking receive
///

template <typename T>
eckit::mpi::Status eckit::mpi::Comm::receive(T* recv, size_t count, int source, int tag) const {
    return receive(recv, count, Data::Type<T>::code(), source, tag);
}

template <typename T>
eckit::mpi::Status eckit::mpi::Comm::receive(T& recv, int source, int tag) const {
    return receive(&recv, 1, Data::Type<T>::code(), source, tag);
}

///
/// Blocking send
///

template <typename T>
void eckit::mpi::Comm::send(const T* sendbuf, size_t count, int dest, int tag) const {
    send(sendbuf, count, Data::Type<T>::code(), dest, tag);
}

template <typename T>
void eckit::mpi::Comm::send(const T& sendbuf, int dest, int tag) const {
    send(&sendbuf, 1, Data::Type<T>::code(), dest, tag);
}

template <typename T>
void eckit::mpi::Comm::synchronisedSend(const T* sendbuf, size_t count, int dest, int tag) const {
    synchronisedSend(sendbuf, count, Data::Type<T>::code(), dest, tag);
}

template <typename T>
void eckit::mpi::Comm::synchronisedSend(const T& sendbuf, int dest, int tag) const {
    synchronisedSend(&sendbuf, 1, Data::Type<T>::code(), dest, tag);
}

///
/// In place simultaneous send and receive
///

template <typename T>
eckit::mpi::Status eckit::mpi::Comm::sendReceiveReplace(T* sendrecv, size_t count, int dest, int sendtag, int source,
                                                        int recvtag) const {
    return sendReceiveReplace(sendrecv, count, Data::Type<T>::code(), dest, sendtag, source, recvtag);
}

template <typename T>
eckit::mpi::Status eckit::mpi::Comm::sendReceiveReplace(T& sendrecv, int dest, int sendtag, int source,
                                                        int recvtag) const {
    return sendReceiveReplace(&sendrecv, 1, Data::Type<T>::code(), dest, sendtag, source, recvtag);
}

///
/// Non-blocking send
///

template <typename T>
eckit::mpi::Request eckit::mpi::Comm::iSend(const T* sendbuf, size_t count, int dest, int tag) const {
    return iSend(sendbuf, count, Data::Type<T>::code(), dest, tag);
}

template <typename T>
eckit::mpi::Request eckit::mpi::Comm::iSend(const T& sendbuf, int dest, int tag) const {
    return iSend(&sendbuf, 1, Data::Type<T>::code(), dest, tag);
}

template <typename T, typename CIter>
void eckit::mpi::Comm::allGatherv(CIter first, CIter last, mpi::Buffer<T>& recv) const {
    int sendcnt = int(std::distance(first, last));

    allGather(sendcnt, recv.counts.begin(), recv.counts.end());

    recv.displs[0] = 0;
    recv.cnt       = recv.counts[0];
    size_t mpiSize = size();
    for (size_t jpart = 1; jpart < mpiSize; ++jpart) {
        recv.displs[jpart] = recv.displs[jpart - 1] + recv.counts[jpart - 1];
        recv.cnt += recv.counts[jpart];
    }

    recv.buffer.resize(recv.cnt);

    allGatherv(first, last, recv.buffer.data(), recv.counts.data(), recv.displs.data());
}

template <typename T>
void eckit::mpi::Comm::allToAll(const std::vector<std::vector<T> >& sendvec,
                                std::vector<std::vector<T> >& recvvec) const {
    size_t commsize = size();
    ECKIT_MPI_ASSERT(sendvec.size() == commsize);
    ECKIT_MPI_ASSERT(recvvec.size() == commsize);

    // Get send-information
    std::vector<int> sendcounts(commsize);
    std::vector<int> senddispls(commsize);
    int sendcnt;
    senddispls[0] = 0;
    sendcounts[0] = sendvec[0].size();
    sendcnt       = sendcounts[0];

    for (size_t jproc = 1; jproc < commsize; ++jproc) {
        senddispls[jproc] = senddispls[jproc - 1] + sendcounts[jproc - 1];
        sendcounts[jproc] = sendvec[jproc].size();
        sendcnt += sendcounts[jproc];
    }

    // Get recv-information
    std::vector<int> recvcounts(commsize);
    std::vector<int> recvdispls(commsize);
    int recvcnt;

    allToAll(sendcounts, recvcounts);

    recvdispls[0] = 0;
    recvcnt       = recvcounts[0];
    for (size_t jproc = 1; jproc < commsize; ++jproc) {
        recvdispls[jproc] = recvdispls[jproc - 1] + recvcounts[jproc - 1];
        recvcnt += recvcounts[jproc];
    }

    // Communicate
    std::vector<T> sendbuf(sendcnt);
    std::vector<T> recvbuf(recvcnt);
    int cnt = 0;
    for (size_t jproc = 0; jproc < commsize; ++jproc) {
        for (int i = 0; i < sendcounts[jproc]; ++i) {
            sendbuf[cnt++] = sendvec[jproc][i];
        }
    }

    allToAllv(sendbuf.data(), sendcounts.data(), senddispls.data(), recvbuf.data(), recvcounts.data(),
              recvdispls.data());

    cnt = 0;
    for (size_t jproc = 0; jproc < commsize; ++jproc) {
        recvvec[jproc].resize(recvcounts[jproc]);
        for (int i = 0; i < recvcounts[jproc]; ++i) {
            recvvec[jproc][i] = recvbuf[cnt++];
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------

#undef ECKIT_MPI_ASSERT

#endif
