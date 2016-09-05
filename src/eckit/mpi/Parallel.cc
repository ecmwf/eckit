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


namespace eckit {
namespace mpi {

//----------------------------------------------------------------------------------------------------------------------

#if 0
int datacode [Data::MAX_CODE_VALUE] = {
    [Data::CHAR]                 = MPI_CHAR,
    [Data::WCHAR]                = MPI_WCHAR,
    [Data::SHORT]                = MPI_SHORT,
    [Data::INT]                  = MPI_INT,
    [Data::LONG]                 = MPI_LONG,
    [Data::SIGNED_CHAR]          = MPI_SIGNED_CHAR,
    [Data::UNSIGNED_SHORT]       = MPI_UNSIGNED_SHORT,
    [Data::UNSIGNED]             = MPI_UNSIGNED,
    [Data::UNSIGNED_LONG]        = MPI_UNSIGNED_LONG,
    [Data::FLOAT]                = MPI_FLOAT,
    [Data::DOUBLE]               = MPI_DOUBLE,
    [Data::LONG_DOUBLE]          = MPI_LONG_DOUBLE,
//    [Data::BOOL]                 = MPI_BOOL,
    [Data::COMPLEX]              = MPI_COMPLEX,
    [Data::DOUBLE_COMPLEX]       = MPI_DOUBLE_COMPLEX,
//    [Data::LONG_DOUBLE_COMPLEX]  = MPI_LONG_DOUBLE_COMPLEX,
    [Data::BYTE]                 = MPI_BYTE,
    [Data::PACKED]               = MPI_PACKED,
    [Data::SHORT_INT]            = MPI_SHORT_INT,
    [Data::INT_INT]              = MPI_2INT,
    [Data::LONG_INT]             = MPI_LONG_INT,
    [Data::FLOAT_INT]            = MPI_FLOAT_INT,
    [Data::DOUBLE_INT]           = MPI_DOUBLE_INT,
    [Data::LONG_DOUBLE_INT]      = MPI_LONG_DOUBLE_INT
};
#endif

static MPI_Datatype toType(Data::Code code) {
    NOTIMP;
}

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

static inline void MPICall(int code, const char* mpifunc, const eckit::CodeLocation& loc)
{
    if (code != MPI_SUCCESS) {

        char error[10240];
        int len = sizeof(error) - 1;
        MPI_Error_string(code, error, &len);
        error[len] = 0;

        std::ostringstream oss;
        oss << "MPI call failed with error '" << error << "' while calling " << mpifunc;
        throw MPIError(oss.str(), loc);
    }
}

#define MPI_CALL(a) MPICall(a,#a,Here())

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

size_t Parallel::getCount(Status& status, Data::Code type) const
{
    int count = 0;
    MPI_Status st;
    NOTIMP; /// @todo implement Status to MPI_Status conversion
    MPI_CALL( MPI_Get_count(&st, toType(type), &count) );
    ASSERT(count >= 0);
    return size_t(count);
}

void Parallel::broadcast(void* buffer, size_t count, Data::Code type, size_t root) const
{
    NOTIMP;
}

void Parallel::gather(const void* sendbuf, size_t sendcount, void* recvbuf, size_t recvcount, Data::Code type, size_t root) const
{
    NOTIMP;
}

void Parallel::scatter(const void* sendbuf, size_t sendcount, void* recvbuf, size_t recvcount, Data::Code type, size_t root) const
{
    NOTIMP;
}

void Parallel::gatherv(const void* sendbuf, size_t sendcount, void* recvbuf, const int recvcounts[], const int displs[], Data::Code type, size_t root) const
{
    NOTIMP;
}

void Parallel::scatterv(const void* sendbuf, const int sendcounts[], const int displs[], void* recvbuf, size_t recvcount, Data::Code type, size_t root) const
{
    NOTIMP;
}

void Parallel::allReduce(const void* sendbuf, void* recvbuf, size_t count, Data::Code type, Operation::Code op) const
{
    NOTIMP;
}

void Parallel::allReduceInPlace(void* sendrecvbuf, size_t count, Data::Code type, Operation::Code op) const
{
    NOTIMP;
}

void Parallel::allGather(const void* sendbuf, size_t sendcount, void* recvbuf, size_t recvcount, Data::Code type) const
{
    NOTIMP;
}

void Parallel::allGatherv(const void* sendbuf, size_t sendcount, void* recvbuf, const int recvcounts[], const int displs[], Data::Code type) const
{
    NOTIMP;
}

void Parallel::allToAll(const void* sendbuf, size_t sendcount, void* recvbuf, size_t recvcount, Data::Code type) const
{
    NOTIMP;
}

void Parallel::allToAllv(const void* sendbuf, const int sendcounts[], const int sdispls[], void* recvbuf, const int recvcounts[], const int rdispls[], Data::Code type) const
{
    NOTIMP;
}

Status Parallel::receive(void* recv, size_t count, Data::Code type, int source, int tag) const
{
    NOTIMP;
}

Status Parallel::send(const void* send, size_t count, Data::Code type, int dest, int tag) const
{
    NOTIMP;
}

Request Parallel::iReceive(void* recv, size_t count, Data::Code type, int source, int tag) const
{
    NOTIMP;
}

Request Parallel::iSend(const void* send, size_t count, Data::Code type, int dest, int tag) const
{
    NOTIMP;
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace mpi
} // namepsace eckit
