/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Tiago Quintino
/// @author Simon Smart
/// @date   June 2018

#ifndef eckit_container_Queue_h
#define eckit_container_Queue_h

#include <condition_variable>
#include <exception>
#include <mutex>
#include <queue>
#include <thread>

#include "eckit/exception/Exceptions.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class QueueInterruptedError : public Exception {
public:

    QueueInterruptedError(const std::string& msg, const CodeLocation& here) :
        Exception(std::string("Threaded queue interrupted") + (msg.size() ? (std::string(": ") + msg) : std::string()),
                  here) {}
};

//----------------------------------------------------------------------------------------------------------------------

template <typename ELEM>
class Queue {

public:  // public

    Queue(size_t max) : max_(max), interrupt_(nullptr), closed_(false) { ASSERT(max > 0); }

    Queue(const Queue&)            = delete;
    Queue& operator=(const Queue&) = delete;

    // n.b. cannot move object with std::condition_variable
    Queue(Queue&& rhs)            = delete;
    Queue& operator=(Queue&& rhs) = delete;

    size_t maxSize() const { return max_; }

    void resize(size_t size) {
        ASSERT(size > 0);
        std::unique_lock<std::mutex> locker(mutex_);
        while (checkInterrupt() && queue_.size() > size) {
            cv_.wait(locker);
        }
        ASSERT(!closed_);
        max_ = size;
        cv_.notify_all();
    }

    void close() {
        std::unique_lock<std::mutex> locker(mutex_);
        closed_ = true;
        cv_.notify_all();
    }

    bool closed() {
        std::unique_lock<std::mutex> locker(mutex_);
        return closed_ || interrupt_;
    }

    bool empty() {
        std::unique_lock<std::mutex> locker(mutex_);
        return queue_.empty();
    }

    bool checkInterrupt() {
        if (interrupt_) {
            std::rethrow_exception(interrupt_);
        }
        return true;
    }

    void interrupt(std::exception_ptr expn) {
        std::unique_lock<std::mutex> locker(mutex_);
        interrupt_ = expn;
        cv_.notify_all();
    }

    long pop(ELEM& e) {
        std::unique_lock<std::mutex> locker(mutex_);
        while (checkInterrupt() && queue_.empty()) {
            if (closed_) {
                return -1;
            }
            cv_.wait(locker);
        }
        std::swap(e, queue_.front());
        queue_.pop();
        size_t size = queue_.size();
        cv_.notify_all();
        return long(size);
    }

    long pop(std::vector<ELEM>& elems) {
        std::unique_lock<std::mutex> locker(mutex_);
        while (checkInterrupt() && queue_.empty()) {
            if (closed_) {
                return -1;
            }
            cv_.wait(locker);
        }

        long count = std::min(elems.size(), queue_.size());
        for (long i = 0; i < count; i++) {
            std::swap(elems[i], queue_.front());
            queue_.pop();
        }
        cv_.notify_all();
        return count;
    }

    size_t push(const ELEM& e) {
        std::unique_lock<std::mutex> locker(mutex_);
        while (checkInterrupt() && queue_.size() >= max_) {
            cv_.wait(locker);
        }
        ASSERT(!closed_);
        queue_.push(e);
        size_t size = queue_.size();
        cv_.notify_all();
        return size;
    }

    template <typename... Args>
    size_t emplace(Args&&... args) {
        std::unique_lock<std::mutex> locker(mutex_);
        while (checkInterrupt() && queue_.size() >= max_) {
            cv_.wait(locker);
        }
        ASSERT(!closed_);
        queue_.emplace(std::forward<Args>(args)...);
        size_t size = queue_.size();
        cv_.notify_all();
        return size;
    }

private:  // members

    std::queue<ELEM> queue_;
    std::mutex mutex_;
    std::condition_variable cv_;
    size_t max_;
    std::exception_ptr interrupt_;
    bool closed_;
};


//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif  // eckit_container_Queue_h
