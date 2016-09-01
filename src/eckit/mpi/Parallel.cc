/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/mpi/Parallel.h"

#include <strstream>

#include <mpi.h>

#include "eckit/runtime/Context.h"

#include "eckit/exception/Exceptions.h"

//----------------------------------------------------------------------------------------------------------------------

class MPIError : public eckit::Exception {
public:
  MPIError(const std::string& msg, const eckit::CodeLocation& loc) : eckit::Exception(msg, loc)
  {
    std::ostringstream s;
    s << "MPI Error: " << msg << " in " << loc;
    reason(s.str());
  }
};

//----------------------------------------------------------------------------------------------------------------------

static inline void MPICall(int code, const char *msg, const eckit::CodeLocation& loc)
{
    if (code != MPI_SUCCESS) {

        char error[10240];
        int len = sizeof(error) - 1;
        MPI_Error_string(code, error, &len);
        error[len] = 0;

        std::ostringstream oss;
        oss << "MPI Call failed with error '" << error << "' -- " << msg;
        throw MPIError(oss.str(), loc);
    }
}

#define MPI_CALL(a) MPICall(a,#a,Here())

namespace eckit {
namespace mpi {

//----------------------------------------------------------------------------------------------------------------------

Parallel::Parallel() {
}

Parallel::~Parallel() {
}

void Parallel::initialize() {
    NOTIMP;
}

void Parallel::finalize() {
    NOTIMP;
}

size_t Parallel::getCount(Status& status, Data::Code datatype) const
{
    NOTIMP;
}

void Parallel::broadcast(void* buffer, size_t count, Data::Code datatype, size_t root) const
{
    NOTIMP;
}

void Parallel::gather(const void* sendbuf, size_t sendcount, void* recvbuf, size_t recvcount, Data::Code datatype, size_t root) const
{
    NOTIMP;
}

void Parallel::scatter(const void* sendbuf, size_t sendcount, void* recvbuf, size_t recvcount, Data::Code datatype, size_t root) const
{
    NOTIMP;
}

void Parallel::gatherv(const void* sendbuf, size_t sendcount, void* recvbuf, const int recvcounts[], const int displs[], Data::Code datatype, size_t root) const
{
    NOTIMP;
}

void Parallel::scatterv(const void* sendbuf, const int sendcounts[], const int displs[], void* recvbuf, size_t recvcount, Data::Code datatype, size_t root) const
{
    NOTIMP;
}

void Parallel::allReduce(const void* sendbuf, void* recvbuf, size_t count, Data::Code datatype, Operation::Code op) const
{
    NOTIMP;
}

void Parallel::allReduceInPlace(void* sendrecvbuf, size_t count, Data::Code datatype, Operation::Code op) const
{
    NOTIMP;
}

void Parallel::allGather(const void* sendbuf, size_t sendcount, void* recvbuf, size_t recvcount, Data::Code datatype) const
{
    NOTIMP;
}

void Parallel::allGatherv(const void* sendbuf, size_t sendcount, void* recvbuf, const int recvcounts[], const int displs[], Data::Code datatype) const
{
    NOTIMP;
}

void Parallel::allToAll(const void* sendbuf, size_t sendcount, void* recvbuf, size_t recvcount, Data::Code datatype) const
{
    NOTIMP;
}

void Parallel::allToAllv(const void* sendbuf, const int sendcounts[], const int sdispls[], void* recvbuf, const int recvcounts[], const int rdispls[], Data::Code datatype) const
{
    NOTIMP;
}

Status Parallel::receive(void* recv, size_t count, Data::Code datatype, int source, int tag) const
{
    NOTIMP;
}

Status Parallel::send(const void* send, size_t count, Data::Code datatype, int dest, int tag) const
{
    NOTIMP;
}

Request Parallel::iReceive(void* recv, size_t count, Data::Code datatype, int source, int tag) const
{
    NOTIMP;
}

Request Parallel::iSend(const void* send, size_t count, Data::Code datatype, int dest, int tag) const
{
    NOTIMP;
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace mpi
} // namepsace eckit
