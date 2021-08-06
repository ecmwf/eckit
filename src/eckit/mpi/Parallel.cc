/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/mpi/Parallel.h"

#include <errno.h>
#include <unistd.h>
#include <limits>
#include <sstream>

#include "eckit/exception/Exceptions.h"

#include "eckit/config/Resource.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/io/DataHandle.h"
#include "eckit/mpi/ParallelRequest.h"
#include "eckit/mpi/ParallelStatus.h"
#include "eckit/runtime/Main.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Mutex.h"

namespace eckit {
namespace mpi {

//----------------------------------------------------------------------------------------------------------------------

static pthread_once_t once      = PTHREAD_ONCE_INIT;
static eckit::Mutex* localMutex = 0;
static size_t initCounter;

static void init() {
    localMutex  = new eckit::Mutex();
    initCounter = 0;
}

//----------------------------------------------------------------------------------------------------------------------

class MPIError : public eckit::Exception {
public:
    MPIError(const std::string& msg, const eckit::CodeLocation& loc) :
        eckit::Exception(msg, loc) {
        std::ostringstream s;
        s << "MPI Error: " << msg << " in " << loc;
        reason(s.str());
    }
};

//----------------------------------------------------------------------------------------------------------------------

void MPICall(int code, const char* mpifunc, const eckit::CodeLocation& loc) {
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

#define MPI_CALL(a) MPICall(a, #a, Here())

//----------------------------------------------------------------------------------------------------------------------

namespace {

static MPI_Datatype PARALLEL_TWO_LONG() {
    static MPI_Datatype mpi_datatype = [] {
        MPI_Datatype mpi_datatype;
        MPI_Type_contiguous(2, MPI_LONG, &mpi_datatype);
        MPI_Type_commit(&mpi_datatype);
        return mpi_datatype;
    }();
    return mpi_datatype;
}

static MPI_Datatype PARALLEL_TWO_LONG_LONG() {
    static MPI_Datatype mpi_datatype = [] {
        MPI_Datatype mpi_datatype;
        MPI_Type_contiguous(2, MPI_LONG_LONG, &mpi_datatype);
        MPI_Type_commit(&mpi_datatype);
        return mpi_datatype;
    }();
    return mpi_datatype;
}

}


// define MPI_LONG_LONG if not existing to avoid compilation errors
#ifndef MPI_LONG_LONG
#define MPI_LONG_LONG MPI_LONG
#endif

static MPI_Datatype toType(Data::Code code) {
    static MPI_Datatype mpi_datatype_[Data::MAX_DATA_CODE] = {
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
        /*[Data::LONG_DOUBLE_INT]      = */ MPI_LONG_DOUBLE_INT,
        /*[Data::LONG_LONG]            = */ MPI_LONG_LONG,
        /*[Data::TWO_LONG]             = */ PARALLEL_TWO_LONG(),
        /*[Data::TWO_LONG_LONG]        = */ PARALLEL_TWO_LONG_LONG(),
        };
    return mpi_datatype_[code];
}

//----------------------------------------------------------------------------------------------------------------------

static MPI_Op mpi_opcode[Operation::MAX_OPERATION_CODE] = {
    /*[Data::SUM]       = */ MPI_SUM,
    /*[Data::PROD]      = */ MPI_PROD,
    /*[Data::MAX]       = */ MPI_MAX,
    /*[Data::MIN]       = */ MPI_MIN,
    /*[Data::MAXLOC]    = */ MPI_MAXLOC,
    /*[Data::MINLOC]    = */ MPI_MINLOC};

static MPI_Op toOp(Operation::Code code) {
    return mpi_opcode[code];
}

//----------------------------------------------------------------------------------------------------------------------

namespace {
size_t getRank(MPI_Comm comm) {
    int rank;
    MPI_CALL(MPI_Comm_rank(comm, &rank));
    return size_t(rank);
}

size_t getSize(MPI_Comm comm) {
    int size;
    MPI_CALL(MPI_Comm_size(comm, &size));
    return size_t(size);
}
}  // namespace

//----------------------------------------------------------------------------------------------------------------------

Parallel::Parallel(const std::string& name) :
    Comm(name) /* don't use member initialisation list */ {

    pthread_once(&once, init);
    eckit::AutoLock<eckit::Mutex> lock(localMutex);

    if (initCounter == 0) {
        initialize();
    }
    initCounter++;

    comm_ = MPI_COMM_WORLD;
    rank_ = getRank(comm_);
    size_ = getSize(comm_);
}

Parallel::Parallel(const std::string& name, MPI_Comm comm, bool) :
    Comm(name) /* don't use member initialisation list */ {

    pthread_once(&once, init);
    eckit::AutoLock<eckit::Mutex> lock(localMutex);

    if (initCounter == 0) {
        initialize();
    }
    initCounter++;

    comm_ = comm;
    rank_ = getRank(comm_);
    size_ = getSize(comm_);
}

Parallel::Parallel(const std::string& name, int comm) :
    Comm(name) {

    pthread_once(&once, init);
    eckit::AutoLock<eckit::Mutex> lock(localMutex);

    if (initCounter == 0) {
        initialize();
    }
    initCounter++;

    comm_ = MPI_Comm_f2c(comm);
    rank_ = getRank(comm_);
    size_ = getSize(comm_);
}

Parallel::~Parallel() {

    pthread_once(&once, init);
    eckit::AutoLock<eckit::Mutex> lock(localMutex);

    initCounter--;

    if (initCounter == 0) {
        finalize();
    }
}

Comm* Parallel::self() const {
    return new Parallel("self", MPI_COMM_SELF, true);
}

void Parallel::initialize() {

    if (!initialized()) {

        int argc(0);
        char** argv(0);

        if (eckit::Main::ready()) {
            argc = eckit::Main::instance().argc();
            argv = eckit::Main::instance().argv();
        }

        std::string MPIInitThread = eckit::Resource<std::string>("MPIInitThread;$ECKIT_MPI_INIT_THREAD", "NONE");

        if (MPIInitThread == "NONE") {
            MPI_CALL(MPI_Init(&argc, &argv));
        }
        else {
            int required;

            if (MPIInitThread == "MPI_THREAD_SINGLE")
                required = MPI_THREAD_SINGLE;  //< only one thread executes
            else if (MPIInitThread == "MPI_THREAD_FUNNELED")
                required = MPI_THREAD_FUNNELED;  //< process may be multi-threaded, but only the main thread will make
                                                 // MPI calls
            else if (MPIInitThread == "MPI_THREAD_SERIALIZED")
                required = MPI_THREAD_SERIALIZED;  //< the process may be multi-threaded, and multiple threads may make
                                                   // MPI calls, but only one at a time
            else if (MPIInitThread == "MPI_THREAD_MULTIPLE")
                required = MPI_THREAD_MULTIPLE;  //< multiple threads may call MPI, with no restrictions
            else {
                std::ostringstream msg;
                msg << "Value of mpiInitThread is unrecognised: " << MPIInitThread
                    << " -- Valid values are: NONE, MPI_THREAD_SINGLE, MPI_THREAD_FUNNELED, MPI_THREAD_SERIALIZED and "
                       "MPI_THREAD_MULTIPLE";
                throw eckit::UserError(msg.str(), Here());
            }

            int provided;

            MPI_CALL(MPI_Init_thread(&argc, &argv, required, &provided));

            if (provided != required) {
                std::ostringstream msg;
                msg << "MPI_Init_thread provides different thread support than requested -"
                    << " requested: " << MPIInitThread;
                if (provided == MPI_THREAD_SINGLE)
                    msg << " provided: "
                        << "MPI_THREAD_SINGLE";
                else if (provided == MPI_THREAD_FUNNELED)
                    msg << " provided: "
                        << "MPI_THREAD_FUNNELED";
                else if (provided == MPI_THREAD_SERIALIZED)
                    msg << " provided: "
                        << "MPI_THREAD_SERIALIZED";
                else if (provided == MPI_THREAD_MULTIPLE)
                    msg << " provided: "
                        << "MPI_THREAD_MULTIPLE";
                else
                    msg << " provided: "
                        << " UNKNOWN (" << provided << ")";

                throw eckit::UserError(msg.str(), Here());
            }
        }
    }
}

void Parallel::finalize() {
    if (not finalized()) {
        MPI_CALL(MPI_Finalize());
    }
}

bool Parallel::initialized() {

    int result = 1;
    MPI_CALL(MPI_Initialized(&result));
    return bool(result);
}

bool Parallel::finalized() {

    int result = 1;
    MPI_CALL(MPI_Finalized(&result));
    return bool(result);
}

std::string Parallel::processorName() const {
    char hostname[256];
    int size = sizeof(hostname);
    MPI_CALL(MPI_Get_processor_name(hostname, &size));
    return hostname;
}

void Parallel::barrier() const {
    MPI_CALL(MPI_Barrier(comm_));
}

Request Parallel::iBarrier() const {
    Request req(new ParallelRequest());
    MPI_CALL(MPI_Ibarrier(comm_, toRequest(req)));
    return req;
}

void Parallel::abort(int errorcode) const {
    MPI_CALL(MPI_Abort(comm_, errorcode));
}

Status Parallel::wait(Request& req) const {
    Status st = createStatus();

    MPI_CALL(MPI_Wait(toRequest(req), toStatus(st)));

    return st;
}

std::vector<Status> Parallel::waitAll(std::vector<Request>& req) const {
    int count = req.size();
    std::vector<Status> st(count);
    std::vector<MPI_Status> st_(count);
    std::vector<MPI_Request> req_(count);

    for (int i = 0; i < count; i++) {
        st[i]   = createStatus();
        st_[i]  = *(toStatus(st[i]));
        req_[i] = *(toRequest(req[i]));
    }

    MPI_CALL(MPI_Waitall(count, &req_[0], &st_[0]));

    for (int i = 0; i < count; i++) {
        *(toStatus(st[i]))   = st_[i];
        *(toRequest(req[i])) = req_[i];
    }

    return st;
}

Status Parallel::waitAny(std::vector<Request>& req, int& ireq) const {
    int count = req.size();
    Status st = createStatus();
    std::vector<MPI_Request> req_(count);

    for (int i = 0; i < count; i++) {
        req_[i] = *(toRequest(req[i]));
    }


    MPI_CALL(MPI_Waitany(count, &req_[0], &ireq, toStatus(st)));

    for (int i = 0; i < count; i++) {
        *(toRequest(req[i])) = req_[i];
    }

    return st;
}

Status Parallel::probe(int source, int tag) const {
    Status st = createStatus();

    MPI_CALL(MPI_Probe(source, tag, comm_, toStatus(st)));

    return st;
}

Status Parallel::iProbe(int source, int tag) const {
    Status st = createStatus();
    int flag  = 0;

    MPI_CALL(MPI_Iprobe(source, tag, comm_, &flag, toStatus(st)));

    return flag ? st : Status{};
}

int Parallel::anySource() const {
    return MPI_ANY_SOURCE;
}

int Parallel::anyTag() const {
    return MPI_ANY_TAG;
}

int Parallel::undefined() const {
    return MPI_UNDEFINED;
}

size_t Parallel::getCount(Status& st, Data::Code type) const {
    int count = 0;

    MPI_Datatype mpitype = toType(type);

    MPI_CALL(MPI_Get_count(toStatus(st), mpitype, &count));

    ASSERT(count >= 0);
    return size_t(count);
}

void Parallel::broadcast(void* buffer, size_t count, Data::Code type, size_t root) const {
    ASSERT(root < size_t(std::numeric_limits<int>::max()));
    ASSERT(count < size_t(std::numeric_limits<int>::max()));

    MPI_Datatype mpitype = toType(type);

    MPI_CALL(MPI_Bcast(buffer, int(count), mpitype, int(root), comm_));
}

void Parallel::gather(const void* sendbuf, size_t sendcount, void* recvbuf, size_t recvcount, Data::Code type,
                      size_t root) const {
    ASSERT(sendcount < size_t(std::numeric_limits<int>::max()));
    ASSERT(recvcount < size_t(std::numeric_limits<int>::max()));
    ASSERT(root < size_t(std::numeric_limits<int>::max()));

    MPI_Datatype mpitype = toType(type);

    MPI_CALL(MPI_Gather(const_cast<void*>(sendbuf), int(sendcount), mpitype, recvbuf, int(recvcount), mpitype,
                        int(root), comm_));
}

void Parallel::scatter(const void* sendbuf, size_t sendcount, void* recvbuf, size_t recvcount, Data::Code type,
                       size_t root) const {
    ASSERT(sendcount < size_t(std::numeric_limits<int>::max()));
    ASSERT(recvcount < size_t(std::numeric_limits<int>::max()));
    ASSERT(root < size_t(std::numeric_limits<int>::max()));

    MPI_Datatype mpitype = toType(type);

    MPI_CALL(MPI_Scatter(const_cast<void*>(sendbuf), int(sendcount), mpitype, recvbuf, int(recvcount), mpitype,
                         int(root), comm_));
}

void Parallel::gatherv(const void* sendbuf, size_t sendcount, void* recvbuf, const int recvcounts[], const int displs[],
                       Data::Code type, size_t root) const {
    ASSERT(sendcount < size_t(std::numeric_limits<int>::max()));
    ASSERT(root < size_t(std::numeric_limits<int>::max()));

    MPI_Datatype mpitype = toType(type);

    MPI_CALL(MPI_Gatherv(const_cast<void*>(sendbuf), int(sendcount), mpitype, recvbuf, const_cast<int*>(recvcounts),
                         const_cast<int*>(displs), mpitype, int(root), comm_));
}

void Parallel::scatterv(const void* sendbuf, const int sendcounts[], const int displs[], void* recvbuf,
                        size_t recvcount, Data::Code type, size_t root) const {
    ASSERT(recvcount < size_t(std::numeric_limits<int>::max()));
    ASSERT(root < size_t(std::numeric_limits<int>::max()));

    MPI_Datatype mpitype = toType(type);

    MPI_CALL(MPI_Scatterv(const_cast<void*>(sendbuf), const_cast<int*>(sendcounts), const_cast<int*>(displs), mpitype,
                          recvbuf, int(recvcount), mpitype, int(root), comm_));
}

void Parallel::allReduce(const void* sendbuf, void* recvbuf, size_t count, Data::Code type, Operation::Code op) const {
    ASSERT(count < size_t(std::numeric_limits<int>::max()));

    MPI_Datatype mpitype = toType(type);
    MPI_Op mpiop         = toOp(op);
    ;

    MPI_CALL(MPI_Allreduce(const_cast<void*>(sendbuf), recvbuf, int(count), mpitype, mpiop, comm_));
}

void Parallel::allReduceInPlace(void* sendrecvbuf, size_t count, Data::Code type, Operation::Code op) const {
    ASSERT(count < size_t(std::numeric_limits<int>::max()));

    MPI_Datatype mpitype = toType(type);
    MPI_Op mpiop         = toOp(op);

    MPI_CALL(MPI_Allreduce(MPI_IN_PLACE, sendrecvbuf, int(count), mpitype, mpiop, comm_));
}

void Parallel::allGather(const void* sendbuf, size_t sendcount, void* recvbuf, size_t recvcount,
                         Data::Code type) const {
    ASSERT(sendcount < size_t(std::numeric_limits<int>::max()));
    ASSERT(recvcount < size_t(std::numeric_limits<int>::max()));

    MPI_Datatype mpitype = toType(type);

    MPI_CALL(
        MPI_Allgather(const_cast<void*>(sendbuf), int(sendcount), mpitype, recvbuf, int(recvcount), mpitype, comm_));
}

void Parallel::allGatherv(const void* sendbuf, size_t sendcount, void* recvbuf, const int recvcounts[],
                          const int displs[], Data::Code type) const {
    ASSERT(sendcount < size_t(std::numeric_limits<int>::max()));

    MPI_Datatype mpitype = toType(type);

    MPI_CALL(MPI_Allgatherv(const_cast<void*>(sendbuf), int(sendcount), mpitype, recvbuf, const_cast<int*>(recvcounts),
                            const_cast<int*>(displs), mpitype, comm_));
}

void Parallel::allToAll(const void* sendbuf, size_t sendcount, void* recvbuf, size_t recvcount, Data::Code type) const {
    ASSERT(sendcount < size_t(std::numeric_limits<int>::max()));
    ASSERT(recvcount < size_t(std::numeric_limits<int>::max()));

    MPI_Datatype mpitype = toType(type);

    MPI_CALL(
        MPI_Alltoall(const_cast<void*>(sendbuf), int(sendcount), mpitype, recvbuf, int(recvcount), mpitype, comm_));
}

void Parallel::allToAllv(const void* sendbuf, const int sendcounts[], const int sdispls[], void* recvbuf,
                         const int recvcounts[], const int rdispls[], Data::Code type) const {
    MPI_Datatype mpitype = toType(type);

    MPI_CALL(MPI_Alltoallv(const_cast<void*>(sendbuf), const_cast<int*>(sendcounts), const_cast<int*>(sdispls), mpitype,
                           recvbuf, const_cast<int*>(recvcounts), const_cast<int*>(rdispls), mpitype, comm_));
}

Status Parallel::receive(void* recv, size_t count, Data::Code type, int source, int tag) const {
    ASSERT(count < size_t(std::numeric_limits<int>::max()));

    MPI_Datatype mpitype = toType(type);

    Status status = createStatus();

    MPI_CALL(MPI_Recv(recv, int(count), mpitype, source, tag, comm_, toStatus(status)));

    return status;
}

void Parallel::send(const void* send, size_t count, Data::Code type, int dest, int tag) const {
    ASSERT(count < size_t(std::numeric_limits<int>::max()));

    MPI_Datatype mpitype = toType(type);

    MPI_CALL(MPI_Send(const_cast<void*>(send), int(count), mpitype, dest, tag, comm_));
}

void Parallel::synchronisedSend(const void* send, size_t count, Data::Code type, int dest, int tag) const {
    ASSERT(count < size_t(std::numeric_limits<int>::max()));

    MPI_Datatype mpitype = toType(type);

    MPI_CALL(MPI_Ssend(const_cast<void*>(send), int(count), mpitype, dest, tag, comm_));
}

Request Parallel::iReceive(void* recv, size_t count, Data::Code type, int source, int tag) const {
    ASSERT(count < size_t(std::numeric_limits<int>::max()));

    Request req(new ParallelRequest());

    MPI_Datatype mpitype = toType(type);

    MPI_CALL(MPI_Irecv(recv, int(count), mpitype, source, tag, comm_, toRequest(req)));

    return req;
}

Request Parallel::iSend(const void* send, size_t count, Data::Code type, int dest, int tag) const {
    ASSERT(count < size_t(std::numeric_limits<int>::max()));

    Request req(new ParallelRequest());

    MPI_Datatype mpitype = toType(type);

    MPI_CALL(MPI_Isend(const_cast<void*>(send), int(count), mpitype, dest, tag, comm_, toRequest(req)));

    return req;
}

Comm& Parallel::split(int color, const std::string& name) const {

    if (hasComm(name.c_str())) {
        throw SeriousBug("Communicator with name " + name + " already exists");
    }

    MPI_Comm new_mpi_comm;
    MPI_CALL(MPI_Comm_split(comm_, color, rank(), &new_mpi_comm));
    Comm* newcomm = new Parallel(name, new_mpi_comm, true);
    addComm(name.c_str(), newcomm);
    return *newcomm;
}

void Parallel::free() {
    MPI_CALL(MPI_Comm_free(&comm_));
    rank_ = 0;
    size_ = 0;
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

Request Parallel::request(int request) const {
    return Request(new ParallelRequest(MPI_Request_f2c(request)));
}

MPI_Request* Parallel::toRequest(Request& req) {
    return &(req.as<ParallelRequest>().request_);
}

MPI_Comm Parallel::MPIComm() const {
    return comm_;
}

int Parallel::communicator() const {
    return MPI_Comm_c2f(comm_);
}

eckit::SharedBuffer Parallel::broadcastFile(const PathName& filepath, size_t root) const {

    ASSERT(root < size());

    bool isRoot = rank() == root;

    eckit::CountedBuffer* buffer = nullptr;

    struct BFileOp {
        int err_;
        size_t len_;
    } op = {0, 0};

    errno = 0;

    if (isRoot) {
        try {
            std::unique_ptr<DataHandle> dh(filepath.fileHandle());

            op.len_ = dh->openForRead();
            AutoClose closer(*dh);
            buffer = new eckit::CountedBuffer(op.len_);
            dh->read(buffer->data(), op.len_);

            if (filepath.isDir()) {
                op.err_ = EISDIR;
            }
        }
        catch (Exception&) {
            op.err_ = errno;
        }
    }

    broadcast(&op, sizeof(op), Data::BYTE, root);

    errno = op.err_;  // set errno to ensure consistent error messages across MPI tasks

    if (op.err_) {
        throw CantOpenFile(filepath);
    }

    if (not op.len_) {
        throw ShortFile(filepath);
    }

    if (!isRoot) {
        buffer = new eckit::CountedBuffer(op.len_);
    }

    broadcast(*buffer, op.len_, Data::BYTE, root);

    return eckit::SharedBuffer(buffer);
}

static CommBuilder<Parallel> ParallelBuilder("parallel");

//----------------------------------------------------------------------------------------------------------------------

}  // namespace mpi
}  // namespace eckit
