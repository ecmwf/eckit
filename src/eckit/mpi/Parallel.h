/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_mpi_Parallel_h
#define eckit_mpi_Parallel_h

#include <mpi.h>

#include "eckit/mpi/Comm.h"

namespace eckit {
namespace mpi {

//----------------------------------------------------------------------------------------------------------------------

class Parallel : public eckit::mpi::Comm {

protected:  // methods

    template< class T> friend class CommBuilder;

    Parallel();
    Parallel(MPI_Comm comm, bool);
    Parallel(int comm);

    virtual ~Parallel();

    virtual eckit::mpi::Comm* self() const;

    virtual std::string processorName() const;

    virtual size_t rank() const;

    virtual size_t size() const;

    virtual void barrier() const;

    virtual void abort(int errorcode = -1) const;

    virtual Status wait(Request&) const;

    virtual Status probe(int source, int tag) const;

    virtual int anySource() const;

    virtual int anyTag() const;

    virtual size_t getCount(Status& st, Data::Code type) const;

    virtual void broadcast(void* buffer, size_t count, Data::Code type, size_t root) const;

    virtual void gather(const void* sendbuf, size_t sendcount, void* recvbuf, size_t recvcount, Data::Code type, size_t root) const;

    virtual void scatter(const void* sendbuf, size_t sendcount, void* recvbuf, size_t recvcount, Data::Code type, size_t root) const;

    virtual void gatherv(const void* sendbuf, size_t sendcount, void* recvbuf, const int recvcounts[], const int displs[], Data::Code type, size_t root) const;

    virtual void scatterv(const void* sendbuf, const int sendcounts[], const int displs[], void* recvbuf, size_t recvcount, Data::Code type, size_t root) const;

    virtual void allReduce(const void* sendbuf, void* recvbuf, size_t count, Data::Code type, Operation::Code op) const;

    virtual void allReduceInPlace(void* sendrecvbuf, size_t count, Data::Code type, Operation::Code op) const;

    virtual void allGather(const void *sendbuf, size_t sendcount, void *recvbuf, size_t recvcount, Data::Code type) const;

    virtual void allGatherv(const void *sendbuf, size_t sendcount,
                            void *recvbuf, const int recvcounts[], const int displs[], Data::Code type) const;

    virtual void allToAll(const void* sendbuf, size_t sendcount, void* recvbuf, size_t recvcount, Data::Code type) const;

    virtual void allToAllv(const void *sendbuf, const int sendcounts[], const int sdispls[],
                           void *recvbuf, const int recvcounts[], const int rdispls[],
                           Data::Code type) const;

    virtual Status receive(void* recv, size_t count, Data::Code type, int source, int tag) const;

    virtual void send(const void* send, size_t count, Data::Code type, int dest, int tag) const;

    virtual Request iReceive(void* recv, size_t count, Data::Code type, int source, int tag) const;

    virtual Request iSend(const void* send, size_t count, Data::Code type, int dest, int tag) const;

    virtual void print(std::ostream&) const;

    virtual Status status() const    { return createStatus(); }

    virtual Request request(int) const;

    virtual int communicator() const;

    static Status  createStatus();

    static MPI_Status* toStatus(Status&);

    static MPI_Request* toRequest(Request&);

private: // methods

    static void initialize();

    static void finalize();

    static bool initialized();

    static bool finalized();

private: // members

    MPI_Comm comm_;

};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace mpi
}  // namespace eckit

#endif
