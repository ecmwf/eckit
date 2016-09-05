/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/mpi/Serial.h"

#include "eckit/exception/Exceptions.h"

namespace eckit {
namespace mpi {

//----------------------------------------------------------------------------------------------------------------------

Serial::Serial() {
}

Serial::~Serial() {
}

size_t Serial::getCount(Status& status, Data::Code type) const
{
    return status.count;
}

void Serial::broadcast(void* buffer, size_t count, Data::Code type, size_t root) const
{
    NOTIMP;
}

void Serial::gather(const void* sendbuf, size_t sendcount, void* recvbuf, size_t recvcount, Data::Code type, size_t root) const
{
    NOTIMP;
}

void Serial::scatter(const void* sendbuf, size_t sendcount, void* recvbuf, size_t recvcount, Data::Code type, size_t root) const
{
    NOTIMP;
}

void Serial::gatherv(const void* sendbuf, size_t sendcount, void* recvbuf, const int recvcounts[], const int displs[], Data::Code type, size_t root) const
{
    NOTIMP;
}

void Serial::scatterv(const void* sendbuf, const int sendcounts[], const int displs[], void* recvbuf, size_t recvcount, Data::Code type, size_t root) const
{
    NOTIMP;
}

void Serial::allReduce(const void* sendbuf, void* recvbuf, size_t count, Data::Code type, Operation::Code op) const
{
    NOTIMP;
}

void Serial::allReduceInPlace(void* sendrecvbuf, size_t count, Data::Code type, Operation::Code op) const
{
    NOTIMP;
}

void Serial::allGather(const void* sendbuf, size_t sendcount, void* recvbuf, size_t recvcount, Data::Code type) const
{
    NOTIMP;
}

void Serial::allGatherv(const void* sendbuf, size_t sendcount, void* recvbuf, const int recvcounts[], const int displs[], Data::Code type) const
{
    NOTIMP;
}

void Serial::allToAll(const void* sendbuf, size_t sendcount, void* recvbuf, size_t recvcount, Data::Code type) const
{
    NOTIMP;
}

void Serial::allToAllv(const void* sendbuf, const int sendcounts[], const int sdispls[], void* recvbuf, const int recvcounts[], const int rdispls[], Data::Code type) const
{
    NOTIMP;
}

Status Serial::receive(void* recv, size_t count, Data::Code type, int source, int tag) const
{
    NOTIMP;
}

Status Serial::send(const void* send, size_t count, Data::Code type, int dest, int tag) const
{
    NOTIMP;
}

Request Serial::iReceive(void* recv, size_t count, Data::Code type, int source, int tag) const
{
    NOTIMP;
}

Request Serial::iSend(const void* send, size_t count, Data::Code type, int dest, int tag) const
{
    NOTIMP;
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace mpi
} // namepsace eckit
