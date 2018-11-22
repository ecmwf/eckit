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

#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "eckit/exception/Exceptions.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

template <typename ELEM>
class Queue {

public: // public

    Queue(size_t max) : max_(max), done_(false) {
        ASSERT(max > 0);
    }

    Queue(const Queue&) = delete;
    Queue& operator=(const Queue&) = delete;

    Queue(Queue&&) = default;
    Queue& operator=(Queue&&) = default;

    void resize(size_t size) {
        ASSERT(size > 0);
        std::unique_lock<std::mutex> locker(mutex_);
        while (queue_.size() > size) {
            cv_.wait(locker);
        }
        max_ = size;
        locker.unlock();
        cv_.notify_one();
    }

    void set_done() {
        std::unique_lock<std::mutex> locker(mutex_);
        done_ = true;
        locker.unlock();
        cv_.notify_one();
    }

    bool done() {
        std::unique_lock<std::mutex> locker(mutex_);
        return done_;
    }

    // n.b. no done mechanism implemented here.
    ELEM pop() {
        std::unique_lock<std::mutex> locker(mutex_);
        while (queue_.empty()) {
            ASSERT(!done_);
            cv_.wait(locker);
        }
        auto e = queue_.front();
        queue_.pop();
        locker.unlock();
        cv_.notify_one();
        return e;
    }

    long pop(ELEM& e) {
        std::unique_lock<std::mutex> locker(mutex_);
        while (queue_.empty()) {
            if (done_) return -1;
            cv_.wait(locker);
        }
        std::swap(e, queue_.front());
        queue_.pop();
        size_t size = queue_.size();
        locker.unlock();
        cv_.notify_one();
        return size;
    }

    size_t push(const ELEM& e) {
        std::unique_lock<std::mutex> locker(mutex_);
        while (queue_.size() >= max_) {
            cv_.wait(locker);
        }
        ASSERT(!done_);
        queue_.push(e);
        size_t size = queue_.size();
        locker.unlock();
        cv_.notify_one();
        return size;
    }

    size_t emplace(ELEM&& e) {
        std::unique_lock<std::mutex> locker(mutex_);
        while (queue_.size() >= max_) {
            cv_.wait(locker);
        }
        ASSERT(!done_);
        queue_.emplace(std::move(e));
        size_t size = queue_.size();
        locker.unlock();
        cv_.notify_one();
        return size;
    }

private: // members

    std::queue<ELEM> queue_;
    std::mutex mutex_;
    std::condition_variable cv_;
    size_t max_;
    bool done_;
};


//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit

#endif // eckit_container_Queue_h
