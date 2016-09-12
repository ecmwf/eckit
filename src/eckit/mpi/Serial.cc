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
#include <limits>
#include <complex>

#include "eckit/exception/Exceptions.h"
#include "eckit/mpi/SerialRequest.h"
#include "eckit/mpi/SerialStatus.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Mutex.h"
#include "eckit/maths/Functions.h"

namespace eckit {
namespace mpi {

//----------------------------------------------------------------------------------------------------------------------

static size_t dataSize [Data::MAX_DATA_CODE] = {
    /*[Data::CHAR]                 = */ sizeof(char),
    /*[Data::WCHAR]                = */ sizeof(wchar_t),
    /*[Data::SHORT]                = */ sizeof(short),
    /*[Data::INT]                  = */ sizeof(int),
    /*[Data::LONG]                 = */ sizeof(long),
    /*[Data::SIGNED_CHAR]          = */ sizeof(signed char),
    /*[Data::UNSIGNED_CHAR]        = */ sizeof(unsigned char),
    /*[Data::UNSIGNED_SHORT]       = */ sizeof(unsigned short),
    /*[Data::UNSIGNED]             = */ sizeof(unsigned int),
    /*[Data::UNSIGNED_LONG]        = */ sizeof(unsigned long),
    /*[Data::FLOAT]                = */ sizeof(float),
    /*[Data::DOUBLE]               = */ sizeof(double),
    /*[Data::LONG_DOUBLE]          = */ sizeof(long double),
//    /*[Data::BOOL]                 = */ sizeof(bool),
    /*[Data::COMPLEX]              = */ sizeof(std::complex<float>),
    /*[Data::DOUBLE_COMPLEX]       = */ sizeof(std::complex<double>),
//    /*[Data::LONG_DOUBLE_COMPLEX]  = */ sizeof(std::complex<long double>),
    /*[Data::BYTE]                 = */ sizeof(char),
    /*[Data::PACKED]               = */ sizeof(char),
    /*[Data::SHORT_INT]            = */ sizeof(std::pair<short,int>),
    /*[Data::INT_INT]              = */ sizeof(std::pair<int,int>),
    /*[Data::LONG_INT]             = */ sizeof(std::pair<long,int>),
    /*[Data::FLOAT_INT]            = */ sizeof(std::pair<float,int>),
    /*[Data::DOUBLE_INT]           = */ sizeof(std::pair<double,int>),
    /*[Data::LONG_DOUBLE_INT]      = */ sizeof(std::pair<long double,int>),
};

//----------------------------------------------------------------------------------------------------------------------

static pthread_once_t once = PTHREAD_ONCE_INIT;
static eckit::Mutex *localMutex = 0;
static std::vector<Request>* requests;

static void init() {
    localMutex = new eckit::Mutex();
    requests = new std::vector<Request>(10);
}

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

Status Serial::wait(Request& request) const
{
    int tag = request.as<SerialRequest>().tag_;

    SerialRequest& req = (*requests)[tag].as<SerialRequest>();

    memcpy( req.recvbuf_, req.sendbuf_, req.count_ * dataSize[req.type_] );

    SerialStatus* st = new SerialStatus();

    (*st).count_  = req.count_;
    (*st).source_ = 0;
    (*st).error_  = 0;

    return Status(st);
}

Status Serial::probe(int source, int tag) const
{
   ASSERT(source == 0);

   return status();
}

int Serial::anySource() const
{
    return 0;
}

int Serial::anyTag() const
{
    return std::numeric_limits<int>::max();
}

size_t Serial::getCount(Status& st, Data::Code) const
{
    return st.as<SerialStatus>().count_;
}

void Serial::broadcast(void*, size_t, Data::Code, size_t) const
{
    return;
}

void Serial::gather(const void* sendbuf, size_t sendcount, void* recvbuf, size_t, Data::Code type, size_t) const
{
    memcpy( recvbuf, sendbuf, sendcount * dataSize[type] );
}

void Serial::scatter(const void* sendbuf, size_t, void* recvbuf, size_t recvcount, Data::Code type, size_t) const
{
    memcpy( recvbuf, sendbuf, recvcount * dataSize[type] );
}

void Serial::gatherv(const void* sendbuf, size_t sendcount, void* recvbuf, const int [], const int [], Data::Code type, size_t) const
{
    memcpy( recvbuf, sendbuf, sendcount * dataSize[type] );
}

void Serial::scatterv(const void* sendbuf, const int[], const int[], void* recvbuf, size_t recvcount, Data::Code type, size_t) const
{
    memcpy( recvbuf, sendbuf, recvcount * dataSize[type] );
}

void Serial::allReduce(const void* sendbuf, void* recvbuf, size_t count, Data::Code type, Operation::Code) const
{
    memcpy( recvbuf, sendbuf, count * dataSize[type] );
}

void Serial::allReduceInPlace(void*, size_t, Data::Code, Operation::Code) const {
    return;
}

void Serial::allGather(const void* sendbuf, size_t sendcount, void* recvbuf, size_t, Data::Code type) const
{
    memcpy( recvbuf, sendbuf, sendcount * dataSize[type] );
}

void Serial::allGatherv(const void* sendbuf, size_t sendcount, void* recvbuf, const int [], const int [], Data::Code type) const
{
    memcpy( recvbuf, sendbuf, sendcount * dataSize[type] );
}

void Serial::allToAll(const void* sendbuf, size_t sendcount, void* recvbuf, size_t, Data::Code type) const
{
    memcpy( recvbuf, sendbuf, sendcount * dataSize[type] );
}

void Serial::allToAllv(const void* sendbuf, const int sendcounts[], const int[], void* recvbuf, const int[], const int[], Data::Code type) const
{
    memcpy( recvbuf, sendbuf, sendcounts[0] * dataSize[type] );
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
    pthread_once(&once, init);
    eckit::AutoLock<eckit::Mutex> lock(localMutex);
    if(tag >= requests->size()) { requests->resize( eckit::round(tag+1,10) ); }

    (*requests)[tag] = createRequest();

    SerialRequest& req = (*requests)[tag].as<SerialRequest>();

    req.recvbuf_ = recv;
    req.count_   = count;
    req.tag_     = tag;
    req.type_    = type;

    return (*requests)[tag];
}

Request Serial::iSend(const void* send, size_t count, Data::Code type, int dest, int tag) const
{
    pthread_once(&once, init);
    eckit::AutoLock<eckit::Mutex> lock(localMutex);
    if(tag >= requests->size()) { requests->resize( eckit::round(tag+1,10) ); }

    (*requests)[tag] = createRequest();

    SerialRequest& req = (*requests)[tag].as<SerialRequest>();

    req.sendbuf_ = const_cast<void*>(send);
    req.count_   = count;
    req.tag_     = tag;
    req.type_    = type;

    return (*requests)[tag];
}

Status Serial::createStatus() {
    return Status(new SerialStatus());
}

Request Serial::createRequest() {
    return Request(new SerialRequest());
}

void Serial::print(std::ostream& os) const {
    os << "Serial()";
}

CommBuilder<Serial> SerialBuilder("serial");

//----------------------------------------------------------------------------------------------------------------------

} // namespace mpi
} // namepsace eckit
