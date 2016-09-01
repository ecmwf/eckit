/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_mpi_Parallel_h
#define eckit_mpi_Parallel_h

#include "eckit/mpi/Comm.h"

namespace eckit {
namespace mpi {

//----------------------------------------------------------------------------------------------------------------------

class Parallel : public eckit::mpi::Comm {

protected:  // methods

    Parallel();

    virtual ~Parallel();

    virtual void initialize();

    virtual void finalize();

    virtual size_t getCount(Status& status, Data::Code datatype) const;

    virtual void broadcast(void* buffer, size_t count, Data::Code datatype, size_t root) const;

    virtual void gather(const void* sendbuf, size_t sendcount, void* recvbuf, size_t recvcount, Data::Code datatype, size_t root) const;

    virtual void scatter(const void* sendbuf, size_t sendcount, void* recvbuf, size_t recvcount, Data::Code datatype, size_t root) const;

    virtual void gatherv(const void* sendbuf, size_t sendcount, void* recvbuf, const int recvcounts[], const int displs[], Data::Code datatype, size_t root) const;

    virtual void scatterv(const void* sendbuf, const int sendcounts[], const int displs[], void* recvbuf, size_t recvcount, Data::Code datatype, size_t root) const;

    virtual void allReduce(const void* sendbuf, void* recvbuf, size_t count, Data::Code datatype, Operation::Code op) const;

    virtual void allReduceInPlace(void* sendrecvbuf, size_t count, Data::Code datatype, Operation::Code op) const;

    virtual void allGather(const void *sendbuf, size_t sendcount, void *recvbuf, size_t recvcount, Data::Code datatype) const;

    virtual void allGatherv(const void *sendbuf, size_t sendcount,
                            void *recvbuf, const int recvcounts[], const int displs[], Data::Code datatype) const;

    virtual void allToAll(const void* sendbuf, size_t sendcount, void* recvbuf, size_t recvcount, Data::Code datatype) const;

    virtual void allToAllv(const void *sendbuf, const int sendcounts[], const int sdispls[],
                           void *recvbuf, const int recvcounts[], const int rdispls[],
                           Data::Code datatype) const;

    virtual Status receive(void* recv, size_t count, Data::Code datatype, int source, int tag) const;

    virtual Status send(const void* send, size_t count, Data::Code datatype, int dest, int tag) const;

    virtual Request iReceive(void* recv, size_t count, Data::Code datatype, int source, int tag) const;

    virtual Request iSend(const void* send, size_t count, Data::Code datatype, int dest, int tag) const;

};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace mpi
}  // namespace eckit

#endif
