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

namespace eckit::mpi {

//----------------------------------------------------------------------------------------------------------------------

class Serial : public eckit::mpi::Comm {
public:

    struct Constants {
        static constexpr int anyTag() { return -1; }
        static constexpr int anySource() { return -1; }
        static constexpr int undefined() { return -32766; }
        static constexpr int procNull() { return -1; }
    };

protected:  // methods

    template <class T>
    friend class CommBuilder;

    Serial(std::string_view name);
    Serial(std::string_view name, int);

    ~Serial() override;

    eckit::mpi::Comm* self() const override;

    std::string processorName() const override;

    size_t remoteSize() const override;

    void barrier() const override;

    Request iBarrier() const override;

    void abort(int errorcode = -1) const override;

    Status wait(Request&) const override;

    Status waitAny(std::vector<Request>&, int&) const override;

    std::vector<Status> waitAll(std::vector<Request>&) const override;

    Status probe(int source, int tag) const override;

    Status iProbe(int source, int tag) const override;

    int anySource() const override;

    int anyTag() const override;

    int undefined() const override;

    int procNull() const override;

    size_t getCount(Status& st, Data::Code type) const override;

    void broadcast(void* buffer, size_t count, Data::Code type, size_t root) const override;

    virtual void gather(const void* sendbuf, size_t sendcount, void* recvbuf, size_t recvcount, Data::Code type,
                        size_t root) const override;

    virtual void scatter(const void* sendbuf, size_t sendcount, void* recvbuf, size_t recvcount, Data::Code type,
                         size_t root) const override;

    virtual void gatherv(const void* sendbuf, size_t sendcount, void* recvbuf, const int recvcounts[],
                         const int displs[], Data::Code type, size_t root) const override;

    virtual void scatterv(const void* sendbuf, const int sendcounts[], const int displs[], void* recvbuf,
                          size_t recvcount, Data::Code type, size_t root) const override;

    virtual void reduce(const void* sendbuf, void* recvbuf, size_t count, Data::Code type, Operation::Code op,
                        size_t root) const override;

    virtual void reduceInPlace(void* sendrecvbuf, size_t count, Data::Code type, Operation::Code op,
                               size_t root) const override;

    virtual void allReduce(const void* sendbuf, void* recvbuf, size_t count, Data::Code type,
                           Operation::Code op) const override;

    void allReduceInPlace(void* sendrecvbuf, size_t count, Data::Code type, Operation::Code op) const override;

    virtual void allGather(const void* sendbuf, size_t sendcount, void* recvbuf, size_t recvcount,
                           Data::Code type) const override;

    virtual void allGatherv(const void* sendbuf, size_t sendcount, void* recvbuf, const int recvcounts[],
                            const int displs[], Data::Code type) const override;

    virtual void allToAll(const void* sendbuf, size_t sendcount, void* recvbuf, size_t recvcount,
                          Data::Code type) const override;

    virtual void allToAllv(const void* sendbuf, const int sendcounts[], const int sdispls[], void* recvbuf,
                           const int recvcounts[], const int rdispls[], Data::Code type) const override;

    Status receive(void* recv, size_t count, Data::Code type, int source, int tag) const override;

    void send(const void* send, size_t count, Data::Code type, int dest, int tag) const override;

    void synchronisedSend(const void* send, size_t count, Data::Code type, int dest, int tag) const override;

    Request iReceive(void* recv, size_t count, Data::Code type, int source, int tag) const override;

    Request iSend(const void* send, size_t count, Data::Code type, int dest, int tag) const override;

    virtual Status sendReceiveReplace(void* sendrecv, size_t count, Data::Code type, int dest, int sendtag, int source,
                                      int recvtag) const override;

    Comm& split(int color, const std::string& name) const override;

    void free() override;

    eckit::SharedBuffer broadcastFile(const eckit::PathName& filepath, size_t root) const override;

    void print(std::ostream&) const override;

    Status status() const override { return createStatus(); }

    Request request(int tag) const override;

    // Not implemented - returns NullGroup
    Group group(int) const override;

    // Not implemented - returns NullGroup
    Group group() const override;

    // Not implemented - returns NullGroup
    Group remoteGroup() const override;

    // Not implemented
    Comm& create(const Group&, const std::string& name) const override;

    // Not implemented
    Comm& create(const Group&, int tag, const std::string& name) const override;


    static Status createStatus();

    int communicator() const override;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::mpi

#endif
