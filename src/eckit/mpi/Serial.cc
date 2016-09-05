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

#include <unistd.h>

#include "eckit/exception/Exceptions.h"

namespace eckit {
namespace mpi {

//----------------------------------------------------------------------------------------------------------------------

Serial::Serial() {
}

Serial::~Serial() {
}

void Serial::initialize() {
}

void Serial::finalize() {
}

std::string Serial::processorName() const {
    char hostname[256];
    SYSCALL(::gethostname(hostname, sizeof(hostname)));
    return hostname;
}

size_t Serial::rank() const {
    return 0;
}

size_t Serial::size() const {
    return 1;
}

void Serial::barrier() const {
    return;
}

void Serial::abort(int) const
{
    throw Abort("MPI Abort called");
}

void Serial::wait(Request&) const
{
    NOTIMP; /// @todo implement
}

Status Serial::probe(int source, int tag) const
{
    NOTIMP; /// @todo implement
}

int Serial::anySource() const
{
    NOTIMP; /// @todo implement
}

int Serial::anyTag() const
{
    NOTIMP; /// @todo implement
}

size_t Serial::getCount(Status& status, Data::Code type) const
{
    NOTIMP;
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

void Serial::send(const void* send, size_t count, Data::Code type, int dest, int tag) const
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

void Serial::print(std::ostream& os) const {
    os << "Serial()";
}

CommBuilder<Serial> SerialBuilder("serial");

//----------------------------------------------------------------------------------------------------------------------

} // namespace mpi
} // namepsace eckit
