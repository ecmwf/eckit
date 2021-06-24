/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_mpi_Serial_h
#define eckit_mpi_Serial_h

#include "eckit/mpi/Comm.h"

namespace eckit {
namespace mpi {

//----------------------------------------------------------------------------------------------------------------------

class Serial : public eckit::mpi::Comm {
public:
    struct Constants {
        static constexpr int anyTag() { return -1; }
        static constexpr int anySource() { return -1; }
        static constexpr int undefined() { return -32766; }
    };

protected:  // methods
    template <class T>
    friend class CommBuilder;

    Serial(const std::string& name);
    Serial(const std::string& name, int);

    virtual ~Serial() override;

    virtual eckit::mpi::Comm* self() const override;

    virtual std::string processorName() const override;

    virtual void barrier() const override;

    virtual Request iBarrier() const override;

    virtual void abort(int errorcode = -1) const override;

    virtual Status wait(Request&) const override;

    virtual Status waitAny(std::vector<Request>&, int&) const override;

    virtual std::vector<Status> waitAll(std::vector<Request>&) const override;

    virtual Status probe(int source, int tag) const override;

    virtual Status iProbe(int source, int tag) const override;

    virtual int anySource() const override;

    virtual int anyTag() const override;

    virtual int undefined() const override;

    virtual size_t getCount(Status& st, Data::Code type) const override;

    virtual void broadcast(void* buffer, size_t count, Data::Code type, size_t root) const override;

    virtual void gather(const void* sendbuf, size_t sendcount, void* recvbuf, size_t recvcount, Data::Code type,
                        size_t root) const override;

    virtual void scatter(const void* sendbuf, size_t sendcount, void* recvbuf, size_t recvcount, Data::Code type,
                         size_t root) const override;

    virtual void gatherv(const void* sendbuf, size_t sendcount, void* recvbuf, const int recvcounts[],
                         const int displs[], Data::Code type, size_t root) const override;

    virtual void scatterv(const void* sendbuf, const int sendcounts[], const int displs[], void* recvbuf,
                          size_t recvcount, Data::Code type, size_t root) const override;

    virtual void allReduce(const void* sendbuf, void* recvbuf, size_t count, Data::Code type,
                           Operation::Code op) const override;

    virtual void allReduceInPlace(void* sendrecvbuf, size_t count, Data::Code type, Operation::Code op) const override;

    virtual void allGather(const void* sendbuf, size_t sendcount, void* recvbuf, size_t recvcount,
                           Data::Code type) const override;

    virtual void allGatherv(const void* sendbuf, size_t sendcount, void* recvbuf, const int recvcounts[],
                            const int displs[], Data::Code type) const override;

    virtual void allToAll(const void* sendbuf, size_t sendcount, void* recvbuf, size_t recvcount,
                          Data::Code type) const override;

    virtual void allToAllv(const void* sendbuf, const int sendcounts[], const int sdispls[], void* recvbuf,
                           const int recvcounts[], const int rdispls[], Data::Code type) const override;

    virtual Status receive(void* recv, size_t count, Data::Code type, int source, int tag) const override;

    virtual void send(const void* send, size_t count, Data::Code type, int dest, int tag) const override;

    virtual void synchronisedSend(const void* send, size_t count, Data::Code type, int dest, int tag) const override;

    virtual Request iReceive(void* recv, size_t count, Data::Code type, int source, int tag) const override;

    virtual Request iSend(const void* send, size_t count, Data::Code type, int dest, int tag) const override;

    virtual Comm& split(int color, const std::string& name) const override;

    virtual void free() override;

    virtual eckit::SharedBuffer broadcastFile(const eckit::PathName& filepath, size_t root) const override;

    virtual void print(std::ostream&) const override;

    virtual Status status() const override { return createStatus(); }

    virtual Request request(int tag) const override;

    static Status createStatus();

    virtual int communicator() const override;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace mpi
}  // namespace eckit

#endif
