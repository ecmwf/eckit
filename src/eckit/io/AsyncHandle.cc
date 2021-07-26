/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include <cstring>


#include "eckit/exception/Exceptions.h"
#include "eckit/io/AsyncHandle.h"
#include "eckit/maths/Functions.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Thread.h"


namespace eckit {


#if 0
ClassSpec AsyncHandle::classSpec_ = {&DataHandle::classSpec(), "AsyncHandle",};
Reanimator<AsyncHandle> AsyncHandle::reanimator_;
#endif

class AsyncHandleWriter : public Thread {
    AsyncHandle& owner_;
    virtual void run();

public:
    AsyncHandleWriter(AsyncHandle& owner) :
        owner_(owner) {}
};

void AsyncHandleWriter::run() {
    while (!stopped()) {
        Buffer* delme = nullptr;
        try {
            AutoLock<MutexCond> lock(owner_.cond_);
            while (owner_.buffers_.empty() && !stopped()) {
                owner_.cond_.wait();
            }

            if (stopped()) {
                break;
            }

            ASSERT(!owner_.buffers_.empty());

            std::pair<size_t, Buffer*> p = owner_.buffers_.front();
            owner_.buffers_.pop_front();
            owner_.used_ -= p.second->size();
            delme = p.second;

            long written = owner_.handle().write(p.second->data(), p.first);
            if (written != static_cast<long>(p.first)) {
                std::ostringstream oss;
                oss << "AsyncHandleWriter: written " << written << " out of " << p.first << Log::syserr;
                throw WriteError(oss.str());
            }

            owner_.cond_.signal();
        }
        catch (std::exception& e) {
            eckit::Log::error() << "AsyncHandleWriter got an exception: " << e.what() << " " << owner_ << std::endl;
            AutoLock<MutexCond> lock(owner_.cond_);
            owner_.error_ = e.what();
            owner_.cond_.signal();
        }
        delete delme;
        delme = nullptr;
    }
}

AsyncHandle::AsyncHandle(DataHandle* h, size_t maxSize, size_t rounding) :
    HandleHolder(h),
    maxSize_(maxSize),
    used_(0),
    rounding_(rounding),
    error_(false),
    thread_(new AsyncHandleWriter(*this), false) {
    thread_.start();
}

AsyncHandle::AsyncHandle(DataHandle& h, size_t maxSize, size_t rounding) :
    HandleHolder(h),
    maxSize_(maxSize),
    used_(0),
    rounding_(rounding),
    error_(false),
    thread_(new AsyncHandleWriter(*this), false) {
    thread_.start();
}

AsyncHandle::~AsyncHandle() {
    {
        AutoLock<MutexCond> lock(cond_);
        while (!buffers_.empty()) {
            delete buffers_.front().second;
            buffers_.pop_front();
        }
        cond_.signal();
    }

    thread_.stop();
    cond_.signal();
    thread_.wait();
}

Length AsyncHandle::openForRead() {
    NOTIMP;
}

void AsyncHandle::openForWrite(const Length& length) {
    ASSERT(used_ == 0);
    ASSERT(buffers_.size() == 0);
    handle().openForWrite(length);
}

void AsyncHandle::openForAppend(const Length& length) {
    ASSERT(used_ == 0);
    ASSERT(buffers_.size() == 0);
    handle().openForAppend(length);
}

void AsyncHandle::skip(const Length&) {
    NOTIMP;
}

long AsyncHandle::read(void*, long) {
    NOTIMP;
}

long AsyncHandle::write(const void* buffer, long length) {
    AutoLock<MutexCond> lock(cond_);

    size_t size = eckit::round(length, rounding_);

    while (used_ + size >= maxSize_ && !error_) {
        // Special case for size > maxSize_
        if (buffers_.empty()) {
            break;
        }
        cond_.wait();
    }

    if (error_) {
        throw WriteError(message_);
    }

    std::pair<size_t, Buffer*> p(length, new Buffer(size));
    ::memcpy(p.second->data(), buffer, length);
    buffers_.push_back(p);
    used_ += size;

    cond_.signal();

    return length;
}

void AsyncHandle::close() {
    flush();
    handle().close();

    AutoLock<MutexCond> lock(cond_);

    if (error_) {
        throw WriteError(message_);
    }
}

void AsyncHandle::flush() {
    AutoLock<MutexCond> lock(cond_);
    while (!buffers_.empty() && !error_) {
        cond_.wait();
    }

    if (error_) {
        throw WriteError(message_);
    }

    handle().flush();
}

void AsyncHandle::rewind() {
    NOTIMP;
}

Offset AsyncHandle::seek(const Offset&) {
    NOTIMP;
}


void AsyncHandle::print(std::ostream& s) const {
    s << "AsyncHandle[";
    handle().print(s);
    s << ']';
}

Length AsyncHandle::estimate() {
    return handle().estimate();
}

Offset AsyncHandle::position() {
    NOTIMP;
}

std::string AsyncHandle::title() const {
    return std::string("{") + handle().title() + "}";
}

void AsyncHandle::collectMetrics(const std::string& what) const {
    handle().collectMetrics(what);
}

DataHandle* AsyncHandle::clone() const {
    return new AsyncHandle(handle().clone(), maxSize_, rounding_);
}


}  // namespace eckit
