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

#include <limits>
#include <sstream>

#include "eckit/exception/Exceptions.h"

#include "eckit/mpi/ParallelRequest.h"
#include "eckit/mpi/ParallelStatus.h"
#include "eckit/runtime/Main.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Mutex.h"

namespace eckit {
namespace mpi {

//----------------------------------------------------------------------------------------------------------------------

static pthread_once_t once = PTHREAD_ONCE_INIT;
static eckit::Mutex *localMutex = 0;
static size_t initCounter;

static void init() {
    localMutex = new eckit::Mutex();
    initCounter = 0;
}
//----------------------------------------------------------------------------------------------------------------------

static MPI_Datatype mpi_datacode [Data::MAX_DATA_CODE] = {
    /*[Data::CHAR]                 = */ MPI_CHAR,
    /*[Data::WCHAR]                = */ MPI_WCHAR,
    /*[Data::SHORT]                = */ MPI_SHORT,
    /*[Data::INT]                  = */ MPI_INT,
    /*[Data::LONG]                 = */ MPI_LONG,
    /*[Data::SIGNED_CHAR]          = */ MPI_SIGNED_CHAR,
    /*[Data::SIGNED_CHAR]          = */ MPI_UNSIGNED_CHAR,
    /*[Data::UNSIGNED_SHORT]       = */ MPI_UNSIGNED_SHORT,
    /*[Data::UNSIGNED]             = */ MPI_UNSIGNED,
    /*[Data::UNSIGNED_LONG]        = */ MPI_UNSIGNED_LONG,
    /*[Data::FLOAT]                = */ MPI_FLOAT,
    /*[Data::DOUBLE]               = */ MPI_DOUBLE,
    /*[Data::LONG_DOUBLE]          = */ MPI_LONG_DOUBLE,
//    /*[Data::BOOL]                 = */ MPI_BOOL,
    /*[Data::COMPLEX]              = */ MPI_COMPLEX,
    /*[Data::DOUBLE_COMPLEX]       = */ MPI_DOUBLE_COMPLEX,
//    /*[Data::LONG_DOUBLE_COMPLEX]  = */ MPI_LONG_DOUBLE_COMPLEX,
    /*[Data::BYTE]                 = */ MPI_BYTE,
    /*[Data::PACKED]               = */ MPI_PACKED,
    /*[Data::SHORT_INT]            = */ MPI_SHORT_INT,
    /*[Data::INT_INT]              = */ MPI_2INT,
    /*[Data::LONG_INT]             = */ MPI_LONG_INT,
    /*[Data::FLOAT_INT]            = */ MPI_FLOAT_INT,
    /*[Data::DOUBLE_INT]           = */ MPI_DOUBLE_INT,
    /*[Data::LONG_DOUBLE_INT]      = */ MPI_LONG_DOUBLE_INT
};

static MPI_Datatype toType(Data::Code code) {
    return mpi_datacode[code];
}

//----------------------------------------------------------------------------------------------------------------------

static MPI_Op mpi_opcode[Operation::MAX_OPERATION_CODE] = {
    /*[Data::SUM]       = */ MPI_SUM,
    /*[Data::PROD]      = */ MPI_PROD,
    /*[Data::MAX]       = */ MPI_MAX,
    /*[Data::MIN]       = */ MPI_MIN,
    /*[Data::MAXLOC]    = */ MPI_MAXLOC,
    /*[Data::MINLOC]    = */ MPI_MINLOC
};

static MPI_Op toOp(Operation::Code code) {
    return mpi_opcode[code];
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

Parallel::Parallel() /* don't use member initialisation list */ {

    pthread_once(&once, init);
    eckit::AutoLock<eckit::Mutex> lock(localMutex);

    if(initCounter == 0) { initialize(); }
    initCounter++;

    comm_ = MPI_COMM_WORLD;
}

Parallel::Parallel(MPI_Comm comm) /* don't use member initialisation list */ {

    pthread_once(&once, init);
    eckit::AutoLock<eckit::Mutex> lock(localMutex);

    if(initCounter == 0) { initialize(); }
    initCounter++;

    comm_ = comm;
}

Parallel::Parallel(int comm) {

    pthread_once(&once, init);
    eckit::AutoLock<eckit::Mutex> lock(localMutex);

    if( initCounter == 0 ) { initialize(); }
    initCounter++;

    comm_ = MPI_Comm_f2c(comm);
}

Parallel::~Parallel() {

    pthread_once(&once, init);
    eckit::AutoLock<eckit::Mutex> lock(localMutex);

    initCounter--;

    if(initCounter == 0) { finalize(); }
}

Comm* Parallel::self() const
{
    return new Parallel(MPI_COMM_SELF);
}

void Parallel::initialize() {

    if(!initialized()) {

        int    argc = eckit::Main::instance().argc();
        char **argv = eckit::Main::instance().argv();

        MPI_CALL( MPI_Init(&argc, &argv) );
    }
}

void Parallel::finalize() {
    if(not finalized()) {
        MPI_CALL( MPI_Finalize() );
    }
}

bool Parallel::initialized() {

    int result = 1;
    MPI_CALL( MPI_Initialized(&result) );
    return bool(result);
}

bool Parallel::finalized() {

    int result = 1;
    MPI_CALL( MPI_Finalized(&result) );
    return bool(result);
}

std::string Parallel::processorName() const
{
    char hostname[256];
    int size = sizeof(hostname);
    MPI_CALL( MPI_Get_processor_name(hostname, &size) );
    return hostname;
}

size_t Parallel::rank() const
{
    int rank;
    MPI_CALL( MPI_Comm_rank(comm_, &rank) );
    return size_t(rank);
}

size_t Parallel::size() const
{
    int size;
    MPI_CALL( MPI_Comm_size(comm_, &size) );
    return size_t(size);
}

void Parallel::barrier() const
{
    MPI_CALL( MPI_Barrier(comm_) );
}

void Parallel::abort(int errorcode) const
{
    MPI_CALL( MPI_Abort(comm_, errorcode) );
}

Status Parallel::wait(Request& req) const
{
    Status st   = createStatus();

    MPI_CALL( MPI_Wait(toRequest(req), toStatus(st)) );

    return st;
}

Status Parallel::probe(int source, int tag) const
{
    Status st = createStatus();

    MPI_CALL( MPI_Probe(source, tag, comm_, toStatus(st)) );

    return st;
}

int Parallel::anySource() const
{
    return MPI_ANY_SOURCE;
}

int Parallel::anyTag() const
{
    return MPI_ANY_TAG;
}

size_t Parallel::getCount(Status& st, Data::Code type) const
{
    int count = 0;

    MPI_Datatype mpitype = toType(type);

    MPI_CALL( MPI_Get_count(toStatus(st), mpitype, &count) );

    ASSERT(count >= 0);
    return size_t(count);
}

void Parallel::broadcast(void* buffer, size_t count, Data::Code type, size_t root) const
{
    ASSERT(root  < size_t(std::numeric_limits<int>::max()));
    ASSERT(count < size_t(std::numeric_limits<int>::max()));

    MPI_Datatype mpitype = toType(type);

    MPI_CALL( MPI_Bcast(buffer, int(count), mpitype, int(root), comm_) );
}

void Parallel::gather(const void* sendbuf, size_t sendcount, void* recvbuf, size_t recvcount, Data::Code type, size_t root) const
{
    ASSERT(sendcount  < size_t(std::numeric_limits<int>::max()));
    ASSERT(recvcount  < size_t(std::numeric_limits<int>::max()));
    ASSERT(root       < size_t(std::numeric_limits<int>::max()));

    MPI_Datatype mpitype = toType(type);

    MPI_CALL( MPI_Gather(const_cast<void*>(sendbuf), int(sendcount), mpitype, recvbuf, int(recvcount), mpitype, int(root), comm_) );
}

void Parallel::scatter(const void* sendbuf, size_t sendcount, void* recvbuf, size_t recvcount, Data::Code type, size_t root) const
{
    ASSERT(sendcount  < size_t(std::numeric_limits<int>::max()));
    ASSERT(recvcount  < size_t(std::numeric_limits<int>::max()));
    ASSERT(root       < size_t(std::numeric_limits<int>::max()));

    MPI_Datatype mpitype = toType(type);

    MPI_CALL( MPI_Scatter(const_cast<void*>(sendbuf), int(sendcount), mpitype, recvbuf, int(recvcount), mpitype, int(root), comm_) );
}

void Parallel::gatherv(const void* sendbuf, size_t sendcount, void* recvbuf, const int recvcounts[], const int displs[], Data::Code type, size_t root) const
{
    ASSERT(sendcount  < size_t(std::numeric_limits<int>::max()));
    ASSERT(root       < size_t(std::numeric_limits<int>::max()));

    MPI_Datatype mpitype = toType(type);

    MPI_CALL( MPI_Gatherv(const_cast<void*>(sendbuf), int(sendcount), mpitype, recvbuf, const_cast<int*>(recvcounts), const_cast<int*>(displs), mpitype, int(root), comm_) );
}

void Parallel::scatterv(const void* sendbuf, const int sendcounts[], const int displs[], void* recvbuf, size_t recvcount, Data::Code type, size_t root) const
{
    ASSERT(recvcount  < size_t(std::numeric_limits<int>::max()));
    ASSERT(root       < size_t(std::numeric_limits<int>::max()));

    MPI_Datatype mpitype = toType(type);

    MPI_CALL( MPI_Scatterv(const_cast<void*>(sendbuf), const_cast<int*>(sendcounts), const_cast<int*>(displs), mpitype, recvbuf, int(recvcount), mpitype, int(root), comm_) );
}

void Parallel::allReduce(const void* sendbuf, void* recvbuf, size_t count, Data::Code type, Operation::Code op) const
{
    ASSERT(count  < size_t(std::numeric_limits<int>::max()));

    MPI_Datatype mpitype = toType(type);
    MPI_Op       mpiop   = toOp(op);;

    MPI_CALL( MPI_Allreduce(const_cast<void*>(sendbuf), recvbuf, int(count), mpitype, mpiop, comm_) );
}

void Parallel::allReduceInPlace(void* sendrecvbuf, size_t count, Data::Code type, Operation::Code op) const
{
    ASSERT(count  < size_t(std::numeric_limits<int>::max()));

    MPI_Datatype mpitype = toType(type);
    MPI_Op       mpiop   = toOp(op);

    MPI_CALL( MPI_Allreduce(MPI_IN_PLACE, sendrecvbuf, int(count), mpitype, mpiop, comm_) );
}

void Parallel::allGather(const void* sendbuf, size_t sendcount, void* recvbuf, size_t recvcount, Data::Code type) const
{
    ASSERT(sendcount  < size_t(std::numeric_limits<int>::max()));
    ASSERT(recvcount  < size_t(std::numeric_limits<int>::max()));

    MPI_Datatype mpitype = toType(type);

    MPI_CALL( MPI_Allgather(const_cast<void*>(sendbuf), int(sendcount), mpitype, recvbuf, int(recvcount), mpitype, comm_) );
}

void Parallel::allGatherv(const void* sendbuf, size_t sendcount, void* recvbuf, const int recvcounts[], const int displs[], Data::Code type) const
{
    ASSERT(sendcount  < size_t(std::numeric_limits<int>::max()));

    MPI_Datatype mpitype = toType(type);

    MPI_CALL( MPI_Allgatherv(const_cast<void*>(sendbuf), int(sendcount), mpitype, recvbuf, const_cast<int*>(recvcounts), const_cast<int*>(displs), mpitype, comm_) );
}

void Parallel::allToAll(const void* sendbuf, size_t sendcount, void* recvbuf, size_t recvcount, Data::Code type) const
{
    ASSERT(sendcount  < size_t(std::numeric_limits<int>::max()));
    ASSERT(recvcount  < size_t(std::numeric_limits<int>::max()));

    MPI_Datatype mpitype = toType(type);

    MPI_CALL( MPI_Alltoall(const_cast<void*>(sendbuf), int(sendcount), mpitype, recvbuf, int(recvcount), mpitype, comm_) );
}

void Parallel::allToAllv(const void* sendbuf, const int sendcounts[], const int sdispls[], void* recvbuf, const int recvcounts[], const int rdispls[], Data::Code type) const
{
    MPI_Datatype mpitype = toType(type);

    MPI_CALL( MPI_Alltoallv(const_cast<void*>(sendbuf), const_cast<int*>(sendcounts), const_cast<int*>(sdispls), mpitype,
                            recvbuf, const_cast<int*>(recvcounts), const_cast<int*>(rdispls), mpitype, comm_) );
}

Status Parallel::receive(void* recv, size_t count, Data::Code type, int source, int tag) const
{
    ASSERT(count  < size_t(std::numeric_limits<int>::max()));

    MPI_Datatype mpitype = toType(type);

    Status status = createStatus();

    MPI_CALL( MPI_Recv(recv, int(count), mpitype, source, tag, comm_, toStatus(status)) );

    return status;
}

void Parallel::send(const void* send, size_t count, Data::Code type, int dest, int tag) const
{
    ASSERT(count  < size_t(std::numeric_limits<int>::max()));

    MPI_Datatype mpitype = toType(type);

    MPI_CALL( MPI_Send(const_cast<void*>(send), int(count), mpitype, dest, tag, comm_) );
}

Request Parallel::iReceive(void* recv, size_t count, Data::Code type, int source, int tag) const
{
    ASSERT(count  < size_t(std::numeric_limits<int>::max()));

    Request req = createRequest();

    MPI_Datatype mpitype = toType(type);

    MPI_CALL( MPI_Irecv(recv, int(count), mpitype, source, tag, comm_, toRequest(req)) );

    return req;
}

Request Parallel::iSend(const void* send, size_t count, Data::Code type, int dest, int tag) const
{
    ASSERT(count  < size_t(std::numeric_limits<int>::max()));

    Request req = createRequest();

    MPI_Datatype mpitype = toType(type);

    MPI_CALL( MPI_Isend(const_cast<void*>(send), int(count), mpitype, dest, tag, comm_, toRequest(req)) );

    return req;
}

void Parallel::print(std::ostream& os) const {
    os << "Parallel()";
    /// @note maybe add information about the MPI backend: opem-mpi? mpich? etc...
}

MPI_Status* Parallel::toStatus(Status& st) {
    return &(st.as<ParallelStatus>().status_);
}

Status Parallel::createStatus() {
    return Status(new ParallelStatus());
}

Request Parallel::createRequest() {
    return Request(new ParallelRequest());
}

MPI_Request* Parallel::toRequest(Request& req) {
    return &(req.as<ParallelRequest>().request_);
}

CommBuilder<Parallel> ParallelBuilder("parallel");

//----------------------------------------------------------------------------------------------------------------------

} // namespace mpi
} // namepsace eckit
