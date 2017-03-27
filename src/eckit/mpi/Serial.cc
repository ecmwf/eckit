/*
 * (C) Copyright 1996-2017 ECMWF.
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
#include <deque>

#include "eckit/exception/Exceptions.h"
#include "eckit/mpi/SerialData.h"
#include "eckit/mpi/SerialRequest.h"
#include "eckit/mpi/SerialStatus.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Mutex.h"
#include "eckit/maths/Functions.h"

namespace eckit {
namespace mpi {

//----------------------------------------------------------------------------------------------------------------------

class SerialSendReceive : private NonCopyable {
public:

    static SerialSendReceive& instance() {
        static SerialSendReceive inst;
        return inst;
    }

    void addSend( const Request& send_request )
    {
      send_queue_.push_back(send_request);
    }

    Request nextSend()
    {
      Request send = send_queue_.front();
      send_queue_.pop_front();
      return send;
    }

    void lock() { mutex_.lock(); }
    void unlock() { mutex_.unlock(); }

private:

    SerialSendReceive() {}
    ~SerialSendReceive() {}

    std::deque<Request> send_queue_;

    eckit::Mutex mutex_; ///< instance() creation is thread safe, but access thereon isn't so we need a mutex
};



class SerialRequestPool : private NonCopyable {
public:

    static SerialRequestPool& instance() {
        static SerialRequestPool request_pool;
        return request_pool;
    }

    Request createSendRequest(const void* buffer, size_t count, Data::Code type, int tag) {
        Request r = registerRequest(new SendRequest(buffer,count,type,tag));
        send_[tag] = r;
        return r;
    }

    Request createReceiveRequest(void* buffer, size_t count, Data::Code type, int tag) {
        SerialRequest* request = new ReceiveRequest(buffer,count,type,tag);
        return registerRequest(request);
    }

    Request operator[](int request) {
        return requests_[request];
    }

    Request sendRequest(int tag) {
        return send_[tag];
    }

    void lock() { mutex_.lock(); }
    void unlock() { mutex_.unlock(); }

private:

    Request registerRequest(SerialRequest* request) {
        ++n_;
        if( size_t(n_) == requests_.size() ) n_ = 0;
        request->request_ = n_;
        Request r(request);
        requests_[n_] = r;
        return r;
    }

    SerialRequestPool()
    {
        n_ = -1;
        requests_.resize(100);
    }

    ~SerialRequestPool() {}

    std::vector<Request> requests_;

    std::map<int,Request> send_;

    int n_;

    eckit::Mutex mutex_; ///< instance() creation is thread safe, but access thereon isn't so we need a mutex
};

//----------------------------------------------------------------------------------------------------------------------

Serial::Serial() {
}

Serial::Serial(int) {
}

Serial::~Serial() {
}

Comm* Serial::self() const {
    return new Serial();
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

void Serial::abort(int) const {
    throw Abort("MPI Abort called");
}

Status Serial::wait(Request& req) const {

    AutoLock<SerialRequestPool> lock(SerialRequestPool::instance());

    if( req.as<SerialRequest>().isReceive() ) {

      ReceiveRequest& recvReq = req.as<ReceiveRequest>();

      int tag = recvReq.tag();

      SendRequest& sendReq = SerialRequestPool::instance().sendRequest(tag).as<SendRequest>();

      memcpy( recvReq.buffer(), sendReq.buffer(), sendReq.count() * dataSize[sendReq.type()] );

      SerialStatus* st = new SerialStatus();

      (*st).count_  = sendReq.count();
      (*st).source_ = 0;
      (*st).error_  = 0;

      return Status(st);

    } else {

      SerialStatus* st = new SerialStatus();

      (*st).error_ = 0;

      return Status(st);
    }
}

Status Serial::probe(int source, int) const {
    ASSERT(source == 0);
    return status();
}

int Serial::anySource() const {
    return 0;
}

int Serial::anyTag() const {
    return std::numeric_limits<int>::max();
}

size_t Serial::getCount(Status& st, Data::Code) const {
    return st.as<SerialStatus>().count_;
}

void Serial::broadcast(void*, size_t, Data::Code, size_t) const {
    return;
}

void Serial::gather(const void* sendbuf, size_t sendcount, void* recvbuf, size_t, Data::Code type, size_t) const {
    if( recvbuf != sendbuf )
        memcpy( recvbuf, sendbuf, sendcount * dataSize[type] );
}

void Serial::scatter(const void* sendbuf, size_t, void* recvbuf, size_t recvcount, Data::Code type, size_t) const {
    if( recvbuf != sendbuf )
        memcpy( recvbuf, sendbuf, recvcount * dataSize[type] );
}

void Serial::gatherv(const void* sendbuf, size_t sendcount, void* recvbuf, const int [], const int [], Data::Code type, size_t) const {
    if( recvbuf != sendbuf )
        memcpy( recvbuf, sendbuf, sendcount * dataSize[type] );
}

void Serial::scatterv(const void* sendbuf, const int[], const int[], void* recvbuf, size_t recvcount, Data::Code type, size_t) const {
    if( recvbuf != sendbuf )
        memcpy( recvbuf, sendbuf, recvcount * dataSize[type] );
}

void Serial::allReduce(const void* sendbuf, void* recvbuf, size_t count, Data::Code type, Operation::Code) const {
    if( recvbuf != sendbuf )
        memcpy( recvbuf, sendbuf, count * dataSize[type] );
}

void Serial::allReduceInPlace(void*, size_t, Data::Code, Operation::Code) const {
    return;
}

void Serial::allGather(const void* sendbuf, size_t sendcount, void* recvbuf, size_t, Data::Code type) const {
    if( recvbuf != sendbuf )
        memcpy( recvbuf, sendbuf, sendcount * dataSize[type] );
}

void Serial::allGatherv(const void* sendbuf, size_t sendcount, void* recvbuf, const int [], const int [], Data::Code type) const {
    if( recvbuf != sendbuf )
        memcpy( recvbuf, sendbuf, sendcount * dataSize[type] );
}

void Serial::allToAll(const void* sendbuf, size_t sendcount, void* recvbuf, size_t, Data::Code type) const {
    if( recvbuf != sendbuf )
       memcpy( recvbuf, sendbuf, sendcount * dataSize[type] );
}

void Serial::allToAllv(const void* sendbuf, const int sendcounts[], const int[], void* recvbuf, const int[], const int[], Data::Code type) const {
    if( recvbuf != sendbuf )
       memcpy( recvbuf, sendbuf, sendcounts[0] * dataSize[type] );
}

Status Serial::receive(void* recv, size_t count, Data::Code type, int source, int tag) const
{
    AutoLock<SerialSendReceive> lock(SerialSendReceive::instance());

    Request send_request = SerialSendReceive::instance().nextSend();
    SendRequest& send = send_request.as<SendRequest>();
    if( tag != anyTag() ) {
        ASSERT( tag == send.tag() );
    }
    ASSERT( count == send.count()  );
    memcpy( recv, send.buffer(), send.count() * dataSize[send.type()] );

    SerialStatus* st = new SerialStatus();
    (*st).count_  = send.count();
    (*st).source_ = 0;
    (*st).tag_    = send.tag();
    (*st).error_  = 0;

    return Status(st);
}

void Serial::send(const void* send, size_t count, Data::Code type, int dest, int tag) const
{
    AutoLock<SerialSendReceive> lock(SerialSendReceive::instance());
    SerialSendReceive::instance().addSend( Request( new SendRequest(send,count,type,tag) ) );
}

void Serial::synchronisedSend(const void* send, size_t count, Data::Code type, int dest, int tag) const
{
    // TODO: see if this is good enough
    AutoLock<SerialSendReceive> lock(SerialSendReceive::instance());
    SerialSendReceive::instance().addSend( Request( new SendRequest(send,count,type,tag) ) );
}

Request Serial::iReceive(void* recv, size_t count, Data::Code type, int source, int tag) const {
    AutoLock<SerialRequestPool> lock(SerialRequestPool::instance());
    return SerialRequestPool::instance().createReceiveRequest(recv,count,type,tag);
}

Request Serial::iSend(const void* send, size_t count, Data::Code type, int dest, int tag) const {
    AutoLock<SerialRequestPool> lock(SerialRequestPool::instance());
    return SerialRequestPool::instance().createSendRequest(send,count,type,tag);
}

Status Serial::createStatus() {
    return Status(new SerialStatus());
}

Request Serial::request(int request) const {
    AutoLock<SerialRequestPool> lock(SerialRequestPool::instance());
    return SerialRequestPool::instance()[request];
}

void Serial::print(std::ostream& os) const {
    os << "Serial()";
}

int Serial::communicator() const {
    return 0;
}

CommBuilder<Serial> SerialBuilder("serial");

//----------------------------------------------------------------------------------------------------------------------

} // namespace mpi
} // namepsace eckit
