/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/mpi/Serial.h"

#include <errno.h>
#include <unistd.h>

#include <cstring>
#include <deque>
#include <limits>
#include <memory>

#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/io/DataHandle.h"
#include "eckit/maths/Functions.h"
#include "eckit/mpi/SerialData.h"
#include "eckit/mpi/SerialRequest.h"
#include "eckit/mpi/SerialStatus.h"
#include "eckit/runtime/Main.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Mutex.h"

namespace eckit {
namespace mpi {

//----------------------------------------------------------------------------------------------------------------------

class SerialRequestPool : private NonCopyable {
public:
    static SerialRequestPool& instance() {
        static SerialRequestPool request_pool;
        return request_pool;
    }

    Request createSendRequest(const void* buffer, size_t count, Data::Code type, int tag) {
        Request r = registerRequest(new SendRequest(buffer, count, type, tag));
        send_[tag].push_back(r);
        return r;
    }

    Request createReceiveRequest(void* buffer, size_t count, Data::Code type, int tag) {
        SerialRequest* request = new ReceiveRequest(buffer, count, type, tag);
        return registerRequest(request);
    }

    Request operator[](int request) { return requests_[request]; }

    SendRequest& matchingSendRequest(const ReceiveRequest& req) { return matchingSendRequest(req.tag()); }

    SendRequest& matchingSendRequest(int tag) {
        Request send;
        if (tag == anyTag()) {
            std::map<int, std::deque<Request> >::iterator it = send_.begin();
            for (; it != send_.end(); ++it) {
                std::deque<Request>& sends = it->second;
                if (sends.size()) {
                    Request send = sends.front();
                    sends.pop_front();
                    return send.as<SendRequest>();
                }
            }
            throw eckit::Exception("No send requests available", Here());
        }
        else {
            ASSERT(send_.count(tag) > 0);
            ASSERT(send_[tag].size());
            Request send = send_[tag].front();
            send_[tag].pop_front();
            return send.as<SendRequest>();
        }
    }

    SendRequest* matchNextSendRequest(int tag) {
        Request send;
        if (tag == anyTag()) {
            for (const auto& item : send_) {
                const auto& sends = item.second;
                if (sends.size()) {
                    send = sends.front();
                    return &send.as<SendRequest>();
                }
            }
            return nullptr;
        }

        ASSERT(send_.count(tag) > 0);
        ASSERT(send_[tag].size());
        if (send_.find(tag) != end(send_)) {
            send = send_.at(tag).front();
            return &send.as<SendRequest>();
        }

        return nullptr;
    }

    void lock() { mutex_.lock(); }
    void unlock() { mutex_.unlock(); }

    static constexpr int anyTag() { return Serial::Constants::anyTag(); }

private:
    Request registerRequest(SerialRequest* request) {
        ++n_;
        if (size_t(n_) == requests_.size())
            n_ = 0;
        request->request_ = n_;
        Request r(request);
        requests_[n_] = r;
        return r;
    }

    SerialRequestPool() {
        n_ = -1;
        requests_.resize(100);
    }

    ~SerialRequestPool() {}

    std::vector<Request> requests_;

    std::map<int, std::deque<Request> > send_;

    int n_;

    eckit::Mutex mutex_;  ///< instance() creation is thread safe, but access thereon isn't so we need a mutex
};

//----------------------------------------------------------------------------------------------------------------------

Serial::Serial(const std::string& name) :
    Comm(name) {
    rank_ = 0;
    size_ = 1;
}

Serial::Serial(const std::string& name, int) :
    Comm(name) {
    rank_ = 0;
    size_ = 1;
}

Serial::~Serial() {}

Comm* Serial::self() const {
    return new Serial("self");
}

std::string Serial::processorName() const {
    return Main::hostname();
}

void Serial::barrier() const {
    return;
}

Request Serial::iBarrier() const {
    return Request();
}

Comm& Serial::split(int /*color*/, const std::string& name) const {
    if (hasComm(name.c_str())) {
        throw SeriousBug("Communicator with name " + name + " already exists");
    }
    Comm* newcomm = new Serial(name);
    addComm(name.c_str(), newcomm);
    return *newcomm;
}

void Serial::free() {
    // nothing todo
}

void Serial::abort(int) const {
    // Don't use std::abort as it would raise SIGABRT.
    // MPI_Abort also does not raise SIGABRT
    std::exit(EXIT_FAILURE);
}

Status Serial::wait(Request& req) const {

    AutoLock<SerialRequestPool> lock(SerialRequestPool::instance());

    auto& serialRequest = req.as<SerialRequest>();

    // Return early if request was already handled.
    if (serialRequest.handled()) {
        return new SerialStatus{};
    }

    // Continue if request was not yet handled.
    serialRequest.handled(true);

    // Only do memcpy when waiting for a ReceiveRequest, and return status.
    if (req.as<SerialRequest>().isReceive()) {

        ReceiveRequest& recvReq = req.as<ReceiveRequest>();

        SendRequest& sendReq = SerialRequestPool::instance().matchingSendRequest(recvReq);

        if (sendReq.count() > 0) {
            ::memcpy(recvReq.buffer(), sendReq.buffer(), sendReq.count() * dataSize[sendReq.type()]);
        }

        SerialStatus* st = new SerialStatus();

        (*st).count_  = sendReq.count();
        (*st).source_ = 0;
        (*st).tag_    = sendReq.tag();
        (*st).error_  = 0;

        return Status(st);
    }

    // For SendRequests, do nothing and return a default SerialStatus
    return new SerialStatus{};
}

std::vector<Status> Serial::waitAll(std::vector<Request>& requests) const {
    std::vector<Status> statuses;
    statuses.reserve(requests.size());
    for (auto& req : requests) {
        statuses.push_back(wait(req));
    }
    return statuses;
}

Status Serial::waitAny(std::vector<Request>& requests, int& index) const {
    for (size_t i = 0; i < requests.size(); ++i) {
        if (!requests[i].as<SerialRequest>().handled()) {
            Status status = wait(requests[i]);
            index         = i;
            return status;
        }
    }

    index = undefined();
    return new SerialStatus{};
}

Status Serial::probe(int source, int tag) const {
    ASSERT(source == 0 || source == Serial::Constants::anySource());

    SerialStatus* st = new SerialStatus();

    SendRequest* req = nullptr;
    while (not req) {
        req = SerialRequestPool::instance().matchNextSendRequest(tag);
    }
    (*st).count_  = req->count();
    (*st).tag_    = req->tag();
    (*st).source_ = 0;
    (*st).error_  = 0;

    return Status(st);
}

Status Serial::iProbe(int source, int tag) const {
    ASSERT(source == 0 || source == Serial::Constants::anySource());

    SendRequest* req = SerialRequestPool::instance().matchNextSendRequest(tag);
    if (not req) {
        return Status{};  // Null status
    }

    SerialStatus* st = new SerialStatus{};

    (*st).count_  = req->count();
    (*st).tag_    = req->tag();
    (*st).source_ = 0;
    (*st).error_  = 0;

    return Status(st);
}

int Serial::anySource() const {
    return Serial::Constants::anySource();
}

int Serial::anyTag() const {
    return Serial::Constants::anyTag();
}

int Serial::undefined() const {
    return Serial::Constants::undefined();
}

size_t Serial::getCount(Status& st, Data::Code) const {
    return st.as<SerialStatus>().count_;
}

void Serial::broadcast(void*, size_t, Data::Code, size_t) const {
    return;
}

void Serial::gather(const void* sendbuf, size_t sendcount, void* recvbuf, size_t, Data::Code type, size_t) const {
    if (recvbuf != sendbuf && sendcount > 0) {
        memcpy(recvbuf, sendbuf, sendcount * dataSize[type]);
    }
}

void Serial::scatter(const void* sendbuf, size_t, void* recvbuf, size_t recvcount, Data::Code type, size_t) const {
    if (recvbuf != sendbuf && recvcount > 0) {
        memcpy(recvbuf, sendbuf, recvcount * dataSize[type]);
    }
}

void Serial::gatherv(const void* sendbuf, size_t sendcount, void* recvbuf, const int[], const int[], Data::Code type,
                     size_t) const {
    if (recvbuf != sendbuf && sendcount > 0) {
        memcpy(recvbuf, sendbuf, sendcount * dataSize[type]);
    }
}

void Serial::scatterv(const void* sendbuf, const int[], const int[], void* recvbuf, size_t recvcount, Data::Code type,
                      size_t) const {
    if (recvbuf != sendbuf && recvcount > 0) {
        memcpy(recvbuf, sendbuf, recvcount * dataSize[type]);
    }
}

void Serial::allReduce(const void* sendbuf, void* recvbuf, size_t count, Data::Code type, Operation::Code) const {
    if (recvbuf != sendbuf && count > 0) {
        memcpy(recvbuf, sendbuf, count * dataSize[type]);
    }
}

void Serial::allReduceInPlace(void*, size_t, Data::Code, Operation::Code) const {
    return;
}

void Serial::allGather(const void* sendbuf, size_t sendcount, void* recvbuf, size_t, Data::Code type) const {
    if (recvbuf != sendbuf && sendcount > 0) {
        memcpy(recvbuf, sendbuf, sendcount * dataSize[type]);
    }
}

void Serial::allGatherv(const void* sendbuf, size_t sendcount, void* recvbuf, const int[], const int[],
                        Data::Code type) const {
    if (recvbuf != sendbuf && sendcount > 0) {
        memcpy(recvbuf, sendbuf, sendcount * dataSize[type]);
    }
}

void Serial::allToAll(const void* sendbuf, size_t sendcount, void* recvbuf, size_t, Data::Code type) const {
    if (recvbuf != sendbuf && sendcount > 0)
        memcpy(recvbuf, sendbuf, sendcount * dataSize[type]);
}

void Serial::allToAllv(const void* sendbuf, const int sendcounts[], const int[], void* recvbuf, const int[],
                       const int[], Data::Code type) const {
    if (recvbuf != sendbuf && sendcounts[0] > 0)
        memcpy(recvbuf, sendbuf, sendcounts[0] * dataSize[type]);
}

Status Serial::receive(void* recv, size_t count, Data::Code type, int /*source*/, int tag) const {
    AutoLock<SerialRequestPool> lock(SerialRequestPool::instance());
    ReceiveRequest recv_request(recv, count, type, tag);
    SendRequest& send = SerialRequestPool::instance().matchingSendRequest(recv_request);
    if (tag != anyTag()) {
        ASSERT(tag == send.tag());
    }
    ASSERT(count == send.count());
    if (count > 0) {
        memcpy(recv, send.buffer(), send.count() * dataSize[send.type()]);
    }

    SerialStatus* st = new SerialStatus();
    (*st).count_     = send.count();
    (*st).source_    = 0;
    (*st).tag_       = send.tag();
    (*st).error_     = 0;

    return Status(st);
}

void Serial::send(const void* send, size_t count, Data::Code type, int /*dest*/, int tag) const {
    AutoLock<SerialRequestPool> lock(SerialRequestPool::instance());
    SerialRequestPool::instance().createSendRequest(send, count, type, tag);
}

void Serial::synchronisedSend(const void* send, size_t count, Data::Code type, int /*dest*/, int tag) const {
    // TODO: see if this is good enough
    AutoLock<SerialRequestPool> lock(SerialRequestPool::instance());
    SerialRequestPool::instance().createSendRequest(send, count, type, tag);
}

Request Serial::iReceive(void* recv, size_t count, Data::Code type, int /*source*/, int tag) const {
    AutoLock<SerialRequestPool> lock(SerialRequestPool::instance());
    return SerialRequestPool::instance().createReceiveRequest(recv, count, type, tag);
}

Request Serial::iSend(const void* send, size_t count, Data::Code type, int /*dest*/, int tag) const {
    AutoLock<SerialRequestPool> lock(SerialRequestPool::instance());
    return SerialRequestPool::instance().createSendRequest(send, count, type, tag);
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

eckit::SharedBuffer Serial::broadcastFile(const PathName& filepath, size_t) const {

    std::unique_ptr<DataHandle> dh(filepath.fileHandle());

    Length len = dh->openForRead();
    AutoClose closer(*dh);

    eckit::SharedBuffer buffer(len);
    dh->read(buffer->data(), len);

    if (not len) {
        throw ShortFile(filepath);
    }

    if (filepath.isDir()) {
        errno = EISDIR;
        throw CantOpenFile(filepath);
    }

    return buffer;
}


static CommBuilder<Serial> SerialBuilder("serial");

//----------------------------------------------------------------------------------------------------------------------

}  // namespace mpi
}  // namespace eckit
